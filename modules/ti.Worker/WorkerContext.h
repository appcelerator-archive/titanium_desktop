/*
 * Copyright (c) 2009-2010 Appcelerator, Inc. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef WorkerContext_h
#define WorkerContext_h

#include <queue>

#include <JavaScriptCore/JSBase.h>
#include <kroll/kroll.h>
#include <Poco/Event.h>
#include <Poco/Mutex.h>
#include <Poco/RunnableAdapter.h>
#include <Poco/Thread.h>

namespace Titanium {

class Worker;

class WorkerContext : public KEventObject {
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
	Poco::Event messageEvent;
	Poco::Event terminateEvent;

	void DeliverMessage(KValueRef message);
	void MessageLoop();
};

} // namespace Titanium

#endif
