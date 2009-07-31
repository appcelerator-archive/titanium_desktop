/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license. 
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */	
#include "worker.h"

namespace ti
{
	Worker::Worker(Host *host, kroll::SharedKObject global, std::string &code) : 
		KEventObject("Worker"), host(host), global_object(NULL), code(code), stopped(true), context(NULL)
	{
		/**
		 * @tiapi(method=True,name=Worker.Worker.start,since=0.5) start the worker thread
		 */
		this->SetMethod("start",&Worker::Start);
		/**
		 * @tiapi(method=True,name=Worker.Worker.terminate,since=0.5) terminate the worker thread. the thread can be restarted with start.
		 */
		this->SetMethod("terminate",&Worker::Terminate);
		/**
		 * @tiapi(method=True,name=Worker.Worker.postMessage,since=0.5) post a message (async) into the worker threads queue to be handled by onmessage
		 * @tiarg[Any<Object> data] any valid serializable Javascript data type to pass to the child (as in JSON types). 
		 */
		this->SetMethod("postMessage",&Worker::PostMessage);
	}
	Worker::~Worker()
	{
		Poco::ScopedLock<Poco::Mutex> lock(mutex);
		if (!this->stopped)
		{
			this->stopped=true;
			this->condition.signal();
		}
		if (this->thread.isRunning())
		{
			try
			{
				this->thread.join();
			}
			catch (Poco::Exception& e)
			{
				Logger *logger = Logger::Get("Worker");
				logger->Error(
					"Exception while try to join with thread: %s",
					e.displayText().c_str());
			}
		}
		delete this->adapter;
	}
	void Worker::Run()
	{
		Logger *logger = Logger::Get("Worker");
		
		bool error = false;
		JSGlobalContextRef context = NULL;
		
		try
		{
			// create a new global context
			this->global_object = KJSUtil::CreateNewGlobalContext(this->host);
			
			// bind the worker context properties
			KJSUtil::BindProperties(global_object,this->context);
			
			// evaluate the script
			context = KJSUtil::GetGlobalContext(global_object);
			KJSUtil::Evaluate(context, (char*)this->code.c_str());
		}
		catch(std::exception &e)
		{
			error = true;
			logger->Error("Error loading worker. Error = %s", e.what());
			SharedValue onerror = this->Get("onerror");
			if (onerror->IsMethod())
			{
				SharedKMethod method = onerror->ToMethod();
				ValueList args;
				args.push_back(Value::NewString(e.what()));
				this->host->InvokeMethodOnMainThread(method,args,false);
			}
		}
		
		if (!error)
		{
			// run this thread and wait for pending messages or to be 
			// woken up to stop
			for(;;)
			{
				bool wait = true;
				{
					Poco::ScopedLock<Poco::Mutex> lock(mutex);
					if (this->messages.size()>0)
					{
						wait=false;
					}
				}
				if (wait)
				{
					condmutex.lock(); // will unlock in wait
					condition.wait(condmutex);
				}
				// check to see if the worker wants to receive messages - we do this 
				// each time since they could define at any time
				SharedValue mv = KJSUtil::GetProperty(global_object,"onmessage");
				if (mv->IsMethod())
				{
					// we have to make a copy since calling the onmessage could be re-entrant
					// which would cause the postMessage to deadlock. we hold the lock to 
					// make a copy of the contents of the list and then iterate w/o lock
					std::list<SharedValue> copy;
					{
						// lock inside block only to make copy
						Poco::ScopedLock<Poco::Mutex> lock(mutex);
						if (this->messages.size()>0)
						{
							std::list<SharedValue>::iterator i = this->messages.begin();
							while (i!=this->messages.end())
							{
								SharedValue message = (*i++);
								copy.push_back(message);
							}
							this->messages.clear();
						}
					}
					if (copy.size()>0)
					{
						SharedKMethod onmessage = mv->ToMethod();
						std::list<SharedValue>::iterator i = copy.begin();
						while(i!=copy.end())
						{
							SharedValue message = (*i++);
						
							try
							{
								ValueList args;
								string name = "worker.message";
								AutoPtr<KEventObject> target = this;
								this->duplicate();
								AutoPtr<Event> event = new Event(target, name);
								event->Set("message", message);
								args.push_back(Value::NewObject(event));
								host->InvokeMethodOnMainThread(onmessage,args,false);
							}
							catch(std::exception &e)
							{
								logger->Error("Error dispatching worker message, exception = %s",e.what());
							}
						}
					}
				}
				if (stopped) break;
			}
		}
		
		// make sure we unregister our global so we don't leak
		if (global_object!=NULL)
		{
			KJSUtil::UnregisterGlobalContext(global_object);
		}
		
		this->global_object = NULL;
		this->stopped = true;
		
		logger->Debug("exiting Worker thread");
	}

	void Worker::Start(const ValueList& args, SharedValue result)
	{
		Logger *logger = Logger::Get("Worker");
		logger->Debug("Start called");
		if (!stopped)
		{
			throw ValueException::FromString("Worker already started");
			return;
		}
		this->context = new WorkerContext(host,this);
		this->adapter = new Poco::RunnableAdapter<Worker>(*this, &Worker::Run);
		this->thread.start(*adapter);
	}
	
	void Worker::Terminate(const ValueList& args, SharedValue result)
	{
		Logger *logger = Logger::Get("Worker");
		logger->Debug("Terminate called");
		
		Poco::ScopedLock<Poco::Mutex> lock(mutex);
		if (!stopped)
		{
			stopped=true;
			this->condition.signal();
		}
	}

	void Worker::PostMessage(const ValueList& args, SharedValue result)
	{
		// store the message in our queue (waiting for the lock) and
		// then signal the thread to wake up to process the message
		SharedValue message = args.at(0);
		{
			Poco::ScopedLock<Poco::Mutex> lock(mutex);
			this->messages.push_back(message);
		}
		this->condition.signal();
	}
	
	void Worker::Bound(const char *name, SharedValue value)
	{
		std::string n = name;
		
		if (this->context && n == "onmessage")
		{
			AutoPtr<WorkerContext> c = this->context.cast<WorkerContext>();
			c->SendQueuedMessages();
			this->condition.signal();
		}
	}
	
}