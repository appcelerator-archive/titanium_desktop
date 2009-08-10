/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _WORKER_CONTEXT_H_
#define _WORKER_CONTEXT_H_

#include <kroll/kroll.h>
#include <Poco/Mutex.h>
#include <Poco/ScopedLock.h>
#include "worker.h"

namespace ti
{
	class WorkerContext : public KEventObject
	{
	public:
		WorkerContext(Host *host, SharedKObject worker);
		virtual ~WorkerContext();
	private:
		Host *host;
		SharedKObject worker;
		std::list<SharedValue> messages;
		Poco::Mutex mutex;

		void SendQueuedMessages();

		void PostMessage(const ValueList &args, SharedValue result);
		void ImportScripts(const ValueList &args, SharedValue result);
		
		
		friend class Worker;
	};
}

#endif
