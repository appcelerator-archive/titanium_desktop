/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _WORKER_CONTEXT_H_
#define _WORKER_CONTEXT_H_

#include <kroll/base.h>
#include <Poco/Mutex.h>
#include <Poco/ScopedLock.h>
#include <kroll/kroll.h>

#ifdef OS_WIN32
# undef Yield
#endif

#include "worker.h"

namespace ti
{
	class WorkerContext : public KEventObject
	{
	public:
		WorkerContext(Host *host, KObjectRef worker);
		virtual ~WorkerContext();
		void Terminate();
		void Yield();

	private:
		Host *host;
		KObjectRef worker;
		std::list<KValueRef> messages;
		Poco::Mutex mutex;
		Poco::Condition condition;
		Poco::Mutex condmutex;

		void SendQueuedMessages();
		void PostMessage(const ValueList &args, KValueRef result);
		void ImportScripts(const ValueList &args, KValueRef result);
		void Sleep(const ValueList &args, KValueRef result);
		void CallOnMessageCallback(KMethodRef onMessage, KValueRef message);

		friend class Worker;
	};
}

#endif
