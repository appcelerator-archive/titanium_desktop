/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _WORKER_CONTEXT_H_
#define _WORKER_CONTEXT_H_

#include <kroll/kroll.h>
#include <Poco/Thread.h>
#include <Poco/RunnableAdapter.h>
#include <Poco/Condition.h>
#include <Poco/Mutex.h>
#include <JavaScriptCore/JSBase.h>
#include <queue>

namespace ti
{
	class Worker;
	class WorkerContext : public KEventObject
	{
	public:
		WorkerContext(Worker* worker);
		~WorkerContext();
		virtual KValueRef Get(const char*);
		virtual void Set(const char*, KValueRef);
		void StartWorker(const std::string& code);
		void Terminate();
		void SendMessageToWorker(KValueRef message);
		void _PostMessage(const ValueList &args, KValueRef result);
		void _ImportScripts(const ValueList &args, KValueRef result);
		void _Sleep(const ValueList &args, KValueRef result);

	private:
		Worker* worker;
		JSGlobalContextRef jsContext;
		bool running;
		std::queue<KValueRef> inbox;
		Poco::Mutex inboxLock;
		Poco::Condition messageCondition;
		Poco::Condition terminateCondition;
		Poco::Mutex wakeupConditionMutex;

		void DeliverMessage(KValueRef message);
		void MessageLoop();
	};
}

#endif
