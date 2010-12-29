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

#ifndef Worker_h
#define Worker_h

#include <queue>

#include <kroll/kroll.h>
#include <Poco/Condition.h>
#include <Poco/Mutex.h>
#include <Poco/RunnableAdapter.h>
#include <Poco/ScopedLock.h>
#include <Poco/Thread.h>

namespace Titanium {

class WorkerContext;

class Worker : public KEventObject {
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

} // namespace Titanium

#endif
