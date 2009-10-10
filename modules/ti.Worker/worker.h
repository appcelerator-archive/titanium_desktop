/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _WORKER_H_
#define _WORKER_H_

#include <kroll/kroll.h>
#include <Poco/Thread.h>
#include <Poco/RunnableAdapter.h>
#include <Poco/Condition.h>
#include <Poco/Mutex.h>
#include <Poco/ScopedLock.h>

#include "worker_context.h"
#include "javascript_module.h"

namespace ti
{
	class Worker : public KEventObject
	{
	public:
		Worker(Host *host, kroll::SharedKObject global, std::string& code);
		virtual ~Worker();

		inline JSObjectRef GetGlobalObject() { return global_object; }

	private:
		kroll::Host *host;
		kroll::SharedKObject global;
		JSObjectRef global_object;
		std::string code;
		bool stopped;

		Poco::Thread thread;
		Poco::RunnableAdapter<Worker>* adapter;
		Poco::Condition condition;
		Poco::Mutex condmutex;
		Poco::Mutex mutex;
		std::list<SharedValue> messages;
		kroll::SharedKObject context;

		void Run();
		void Bound(const char *name, SharedValue value);
		void Start(const ValueList& args, SharedValue result);
		void Terminate(const ValueList& args, SharedValue result);
		void PostMessage(const ValueList& args, SharedValue result);
		void CallOnMessageCallback(SharedKMethod onMessage, SharedValue message);

	};
}

#endif
