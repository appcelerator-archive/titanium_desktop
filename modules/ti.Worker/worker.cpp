/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license. 
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */	
#include "worker.h"
#include <kroll/thread_manager.h>

namespace ti
{
	Worker::Worker(Host* host, kroll::KObjectRef global, std::string& code) :
		KEventObject("Worker.Worker"),
		host(host),
		global_object(0),
		code(code),
		stopped(true),
		context(0)
	{
		/**
		 * @tiapi(method=True,name=Worker.Worker.start,since=0.6)
		 * @tiapi Start the worker thread
		 */
		this->SetMethod("start",&Worker::Start);
		/**
		 * @tiapi(method=True,name=Worker.Worker.terminate,since=0.6)
		 * @tiapi Terminate the worker thread. The thread can be restarted with Worker.start()
		 */
		this->SetMethod("terminate",&Worker::Terminate);
		/**
		 * @tiapi(method=True,name=Worker.Worker.postMessage,since=0.6)
		 * @tiapi Post a message (async) into the worker thread's queue to be handled by onmessage
		 * @tiarg[any, data] Any JSON serializable type to pass to the child.
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
		START_KROLL_THREAD;

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
			KJSUtil::Evaluate(context, (char*) this->code.c_str());
		}
		catch(ValueException& e)
		{
			error = true;
			logger->Error("Error loading worker: %s\n", e.ToString().c_str());
			KValueRef onerror = this->Get("onerror");
			if (onerror->IsMethod())
			{
				KMethodRef method = onerror->ToMethod();
				ValueList args(e.GetValue());
				RunOnMainThread(method, args, false);
			}
		}

		AutoPtr<WorkerContext> wc = this->context.cast<WorkerContext>();
		
		if (!error)
		{
			// run this thread and wait for pending messages 
			// or to be woken up to stop
			for(;;)
			{
				// Wait for the code inside the worker to stop sleeping.
				wc->Yield();

				bool wait = true;
				{
					Poco::ScopedLock<Poco::Mutex> lock(mutex);
					if (!this->messages.empty())
							wait = false;
				}

				if (wait)
				{
					condmutex.lock(); // will unlock in wait
					condition.wait(condmutex);
					condmutex.unlock();
				}

				// check to see if the worker wants to receive messages - we do this 
				// each time since they could define at any time
				KValueRef mv = KJSUtil::GetProperty(global_object, "onmessage");
				if (mv->IsMethod())
				{
					KMethodRef onMessage(mv->ToMethod());
					// we have to make a copy since calling the onmessage could be re-entrant
					// which would cause the postMessage to deadlock. we hold the lock to 
					// make a copy of the contents of the list and then iterate w/o lock
					std::list<KValueRef> copy;
					{
						// lock inside block only to make copy
						Poco::ScopedLock<Poco::Mutex> lock(mutex);
						if (this->messages.size()>0)
						{
							std::list<KValueRef>::iterator i = this->messages.begin();
							while (i!=this->messages.end())
							{
								KValueRef message = (*i++);
								copy.push_back(message);
							}
							this->messages.clear();
						}
					}
					if (copy.size()>0)
					{
						std::list<KValueRef>::iterator i = copy.begin();
						while (i != copy.end())
						{
							KValueRef message(*i++);
							this->CallOnMessageCallback(onMessage, message);
						}
					}
				}

				// hold lock while we check to make sure we're stopped
				Poco::ScopedLock<Poco::Mutex> lock(mutex);
				if (stopped) 
				{
					logger->Debug("worker thread stopped detected, exiting...");
					break;
				}
			}
		}

		// terminate the context waking up any threads that might be waiting
		wc->Terminate();
		
		// go ahead and log while we cleanup
		Poco::ScopedLock<Poco::Mutex> lock(mutex);
		
		// make sure we unregister our global so we don't leak
		if (global_object!=NULL)
		{
			KJSUtil::UnregisterGlobalContext(global_object);
		}
		
		this->global_object = NULL;
		this->stopped = true;
		
		logger->Debug("exiting Worker thread");

		END_KROLL_THREAD;
	}

	void Worker::Start(const ValueList& args, KValueRef result)
	{
		Logger *logger = Logger::Get("Worker");
		logger->Debug("Start called");
		Poco::ScopedLock<Poco::Mutex> lock(mutex);
		if (!stopped)
		{
			throw ValueException::FromString("Worker already started");
			return;
		}
		this->stopped = false;
		this->context = new WorkerContext(host,this);
		this->adapter = new Poco::RunnableAdapter<Worker>(*this, &Worker::Run);
		this->thread.start(*adapter);
	}

	void Worker::Terminate(const ValueList& args, KValueRef result)
	{
		Logger *logger = Logger::Get("Worker");
		logger->Debug("Terminate called");
		
		// don't hold the lock after checking
		{
			Poco::ScopedLock<Poco::Mutex> lock(mutex);
			if (!stopped)
			{
				stopped=true;
			}
		}
		
		// cause the worker context to terminate if blocked in sleep
		AutoPtr<WorkerContext> c = this->context.cast<WorkerContext>();
		c->Terminate();
		
		this->condition.signal();
		if (this->thread.isRunning())
		{
			logger->Debug("Waiting for Worker Thread to finish");
			try
			{
				this->thread.join();
			}
			catch (Poco::Exception& e)
			{
				Logger *logger = Logger::Get("Worker");
				logger->Error("Exception while try to join with thread: %s",
					e.displayText().c_str());
			}
			logger->Debug("Worker Thread finished");
		}
		else
		{
			logger->Debug("Worker Thread already finished");
		}
	}

	void Worker::CallOnMessageCallback(KMethodRef onMessage, KValueRef message)
	{
		static Logger* logger = Logger::Get("Worker");
		AutoPtr<Event> event(this->CreateEvent("worker.message"));
		event->Set("message", message);
		ValueList args(Value::NewObject(event));

		try
		{
			RunOnMainThread(onMessage, args, false);
		}
		catch(ValueException& e)
		{
			logger->Error("Exception while during onMessage callback: %s",
				e.ToString().c_str());
		}
	}

	void Worker::PostMessage(const ValueList& args, KValueRef result)
	{
		// store the message in our queue (waiting for the lock) and
		// then signal the thread to wake up to process the message
		KValueRef message = args.at(0);
		{
			Poco::ScopedLock<Poco::Mutex> lock(mutex);
			this->messages.push_back(message);
		}
		this->condition.signal();
	}

	void Worker::Bound(const char *name, KValueRef value)
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
