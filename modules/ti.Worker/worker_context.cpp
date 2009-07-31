/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license. 
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */	
#include "worker_context.h"

namespace ti
{
	WorkerContext::WorkerContext(Host *host, SharedKObject worker) : KEventObject("Worker"), host(host), worker(worker)
	{
		//NOTE: don't really doc these since they are injected into the scope of the worker script and exposed that way
		this->SetMethod("postMessage",&WorkerContext::PostMessage);
		this->SetMethod("importScript",&WorkerContext::ImportScripts); // this is just a convenience map
		this->SetMethod("importScripts",&WorkerContext::ImportScripts);
	}
	WorkerContext::~WorkerContext()
	{
		worker = NULL;
		host = NULL;
	}
	void WorkerContext::SendQueuedMessages()
	{
		Logger *logger = Logger::Get("WorkerContext");
		logger->Debug("SendQueuedMessages called");
		
		SharedValue onmessage = worker->Get("onmessage");
		Poco::ScopedLock<Poco::Mutex> lock(mutex);
		if (onmessage->IsMethod())
		{
			if (messages.size()>0)
			{
				std::list<SharedValue>::iterator i = messages.begin();
				while(i!=messages.end())
				{
					SharedValue v = (*i++);
					ValueList _args;
					string name = "worker.message";
					AutoPtr<KEventObject> target = this;
					this->duplicate();
					AutoPtr<Event> event = new Event(target, name);
					event->Set("message", v);
					_args.push_back(Value::NewObject(event));
					host->InvokeMethodOnMainThread(onmessage->ToMethod(),_args,false);
				}
				messages.clear();
			}
		}
	}
	void WorkerContext::PostMessage(const ValueList &args, SharedValue result)
	{
		Logger *logger = Logger::Get("WorkerContext");
		logger->Debug("PostMessage called");
		try
		{
			Poco::ScopedLock<Poco::Mutex> lock(mutex);
			messages.push_back(args.at(0));
			SendQueuedMessages();
		}
		catch(std::exception &e)
		{
			logger->Error("Error calling onmessage for worker. Error = %s",e.what());
		}
	}
	void WorkerContext::ImportScripts(const ValueList &args, SharedValue result)
	{
		Logger *logger = Logger::Get("WorkerContext");
		
		SharedKMethod appURLToPath = host->GetGlobalObject()->GetNS("App.appURLToPath")->ToMethod();
		AutoPtr<Worker> _worker = worker.cast<Worker>();
		JSGlobalContextRef context = KJSUtil::GetGlobalContext(_worker->GetGlobalObject());
		
		for (size_t c = 0; c < args.size(); c++)
		{
			// first convert the path to a full URL file path
			ValueList a;
			a.push_back(args.at(c));
			SharedValue result = appURLToPath->Call(a);
			const char *path = result->ToString();

			logger->Debug("attempting to import worker script = %s",path);
			KJSUtil::EvaluateFile(context, (char*)path);
		}
	}
}	