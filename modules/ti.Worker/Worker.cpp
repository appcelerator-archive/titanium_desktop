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

#include "Worker.h"

#include <kroll/thread_manager.h>

#include "WorkerContext.h"

namespace Titanium {

static Logger* GetLogger()
{
    static Logger* logger = Logger::Get("WorkerContext");
    return logger;
}

Worker::Worker(std::string& code) :
    KEventObject("Worker.Worker"),
    code(code),
    workerContext(new WorkerContext(this)),
    adapter(0)
{
    /**
     * @tiapi(method=True,name=Worker.Worker.start,since=0.6)
     * @tiapi Start the worker thread
     */
    this->SetMethod("start", &Worker::_Start);

    /**
     * @tiapi(method=True,name=Worker.Worker.terminate,since=0.6)
     * @tiapi Terminate the worker thread. The thread can be restarted with Worker.start()
     */
    this->SetMethod("terminate", &Worker::_Terminate);

    /**
     * @tiapi(method=True,name=Worker.Worker.postMessage,since=0.6)
     * @tiapi Post a message (async) into the worker thread's queue to be handled by onmessage
     * @tiarg[any, data] Any JSON serializable type to pass to the child.
     */
    this->SetMethod("postMessage", &Worker::_PostMessage);

    this->adapter = new Poco::RunnableAdapter<Worker>(*this, &Worker::Run);
}

Worker::~Worker()
{
    KValueRef result(0);
    this->_Terminate(ValueList(), result);
    delete this->adapter;
}

void Worker::Error(KValueRef error)
{
    KValueRef onError = this->Get("onerror");
    if (!onError->IsMethod())
        return;

    RunOnMainThread(onError->ToMethod(), ValueList(error), false);
}

void Worker::_Start(const ValueList& args, KValueRef result)
{
    if (this->thread.isRunning())
        throw ValueException::FromString("Worker already started");

    this->thread.start(*adapter);
}

void Worker::Run()
{
    START_KROLL_THREAD;

    // The worker manages the lifetime of the worker context, so we
    // can just pass a pointer to ourselves instead of an AutoPtr.
    workerContext->StartWorker(this->code);

    END_KROLL_THREAD;
}

void Worker::SendMessageToMainThread(KValueRef message)
{
    {
        Poco::Mutex::ScopedLock lock(inboxLock);
        inbox.push(message);
    }

    HandleInbox();
}

void Worker::HandleInbox()
{
    while (this->Get("onmessage")->IsMethod() && !inbox.empty())
    {
        KValueRef message(0);
        {
            Poco::Mutex::ScopedLock lock(inboxLock);
            message = inbox.front();
            inbox.pop();
        }

        this->DeliverMessage(message);
    }
}

void Worker::DeliverMessage(KValueRef message)
{
    AutoPtr<Event> event(this->CreateEvent("worker.message"));
    event->Set("message", message);

    try
    {
        RunOnMainThread(this->Get("onmessage")->ToMethod(), 
            ValueList(Value::NewObject(event)), false);
    }
    catch (ValueException& e)
    {
        GetLogger()->Error("Exception while during onMessage callback: %s",
            e.ToString().c_str());
    }
}

void Worker::_Terminate(const ValueList& args, KValueRef result)
{
    if (!this->thread.isRunning())
        return;

    this->workerContext->Terminate();

    try
    {
        this->thread.join();
    }
    catch (Poco::Exception& e)
    {
        GetLogger()->Error("Exception while try to join with thread: %s",
            e.displayText().c_str());
        throw ValueException::FromString(e.displayText());
    }
}

void Worker::_PostMessage(const ValueList& args, KValueRef result)
{
    workerContext->SendMessageToWorker(args.GetValue(0));
}

void Worker::Set(const char* name, KValueRef value)
{
    KEventObject::Set(name, value);

    // We now have an onMessage target. Send all our queued
    // messages to this method.
    if (std::string(name) == "onmessage")
        this->HandleInbox();
}

} // namespace Titanium
