/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license. 
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */	
#include "worker_context.h"

namespace ti
{
	WorkerContext::WorkerContext(Host *host, KObjectRef worker) :
		KEventObject("Worker.WorkerContext"),
		host(host),
		worker(worker)
	{
		// NOTE: don't really doc these since they are injected into the
		// scope of the worker script and exposed that way
		this->SetMethod("postMessage", &WorkerContext::PostMessage);

		// this is just a convenience map
		this->SetMethod("importScript", &WorkerContext::ImportScripts); 
		this->SetMethod("importScripts", &WorkerContext::ImportScripts);
		this->SetMethod("sleep", &WorkerContext::Sleep);
	}

	WorkerContext::~WorkerContext()
	{
		worker = NULL;
		host = NULL;
	}

	void WorkerContext::Terminate()
	{
		// if we're blocked in a sleep, signal him to wake up
		condition.signal();
	}

	void WorkerContext::Yield()
	{
		// This method will block until sleeping has stopped.
		Poco::ScopedLock<Poco::Mutex> lock(condmutex);
	}

	void WorkerContext::SendQueuedMessages()
	{
		Logger *logger = Logger::Get("WorkerContext");
		logger->Debug("SendQueuedMessages called");

		if (messages.size() <= 0)
			return;

		KValueRef onMessageValue = worker->Get("onmessage");
		if (!onMessageValue->IsMethod())
			return;

		KMethodRef onMessage(onMessageValue->ToMethod());
		Poco::ScopedLock<Poco::Mutex> lock(mutex);
		std::list<KValueRef>::iterator i = messages.begin();
		while (i != messages.end())
		{
			KValueRef message(*i++);
			this->CallOnMessageCallback(onMessage, message);
		}
		messages.clear();
	}

	void WorkerContext::CallOnMessageCallback(KMethodRef onMessage, KValueRef message)
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

	void WorkerContext::PostMessage(const ValueList &args, KValueRef result)
	{
		Logger *logger = Logger::Get("WorkerContext");
		KValueRef message(args.at(0));

		logger->Debug("PostMessage called with %s", message->DisplayString()->c_str());
		{
			Poco::ScopedLock<Poco::Mutex> lock(mutex);
			messages.push_back(message);
		}
		SendQueuedMessages();
	}

	void WorkerContext::Sleep(const ValueList &args, KValueRef result)
	{
		Logger *logger = Logger::Get("WorkerContext");
		long ms = args.at(0)->ToInt();
		logger->Debug("worker is sleeping for %d ms", ms);
		condmutex.lock();
		if (condition.tryWait(condmutex, ms))
		{
			logger->Debug("worker sleep was interrupted");
			condmutex.unlock();
			throw ValueException::FromString("interrupted");
			return;
		}
		condmutex.unlock();
		logger->Debug("worker sleep completed");
	}

	void WorkerContext::ImportScripts(const ValueList &args, KValueRef result)
	{
		Logger *logger = Logger::Get("WorkerContext");
		AutoPtr<Worker> _worker = worker.cast<Worker>();
		JSGlobalContextRef context = KJSUtil::GetGlobalContext(_worker->GetGlobalObject());

		for (size_t c = 0; c < args.size(); c++)
		{
			std::string path(URLUtils::URLToPath(args.GetString(c)));
			logger->Debug("Attempting to import worker script = %s", path.c_str());
			KJSUtil::EvaluateFile(context, path.c_str());
		}
	}
}
