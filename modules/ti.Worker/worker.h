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
#include <queue>

namespace ti
{
	class WorkerContext;

	class Worker : public KEventObject
	{
	public:
		Worker(std::string& code);
		~Worker();
		void Error(KValueRef value);
		void SendMessageToMainThread(KValueRef message);
		virtual void Set(const char* name, KValueRef value);

	private:
		std::string code;
		AutoPtr<WorkerContext> workerContext;
		Poco::Thread thread;
		Poco::RunnableAdapter<Worker>* adapter;
		std::queue<KValueRef> inbox;
		Poco::Mutex inboxLock;

		void Run();
		void HandleInbox();
		void DeliverMessage(KValueRef message);
		void _Start(const ValueList& args, KValueRef result);
		void _Terminate(const ValueList& args, KValueRef result);
		void _PostMessage(const ValueList& args, KValueRef result);
	};
}

#endif
