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

#ifndef Process_h
#define Process_h

#include <sstream>

#include <kroll/kroll.h>

namespace Titanium {

class NativePipe;
class Pipe;

class Process : public KEventMethod {
public:
    Process();
    virtual ~Process();

    static AutoPtr<Process> CreateProcess();
    virtual KObjectRef CloneEnvironment();
    virtual void LaunchAsync();
    virtual BytesRef LaunchSync();
    virtual void ExitMonitorAsync();
    virtual void ExitMonitorSync();
    std::string ArgumentsToString();
    void SetOnRead(KMethodRef method);
    void SetOnExit(KMethodRef onExit);
    void Exited(bool async);
    void ExitCallback(const ValueList& args, KValueRef result);
    virtual KValueRef Call(const ValueList& args);
    static KObjectRef GetCurrentEnvironment();

    void SetStdin(AutoPtr<Pipe> stdinPipe);
    void SetStdout(AutoPtr<Pipe> stdoutPipe);
    void SetStderr(AutoPtr<Pipe> stderrPipe);
    inline bool IsRunning() { return running; }
    inline void SetPID(int pid) { this->pid = pid; }
    virtual inline void SetArguments(KListRef args) { this->args = args; }
    inline void SetEnvironment(KObjectRef env) { this->environment = env; }
    inline int GetPID() { return this->pid; }
    inline AutoPtr<Pipe> GetStdin() { return this->stdinPipe; }
    inline AutoPtr<Pipe> GetStdout() { return this->stdoutPipe; }
    inline AutoPtr<Pipe> GetStderr() { return this->stderrPipe; }
    inline KListRef GetArgs() { return this->args; };
    inline KObjectRef GetEnvironment() { return this->environment; }
    void SetEnvironment(const char *name, const char *value)
    {
        environment->SetString(name, value);
    }

    virtual void Terminate() = 0;
    virtual void Kill() = 0;
    virtual void SendSignal(int signal) = 0;
    virtual void ForkAndExec() = 0;
    virtual void MonitorAsync() = 0;
    virtual BytesRef MonitorSync() = 0;
    virtual int Wait() = 0;
    virtual void RecreateNativePipes() = 0;
    virtual AutoPtr<NativePipe> GetNativeStdin() = 0;
    virtual AutoPtr<NativePipe> GetNativeStdout() = 0;
    virtual AutoPtr<NativePipe> GetNativeStderr() = 0;
    void AttachPipes(bool async);

protected:
    void _GetPID(const ValueList& args, KValueRef result);
    void _GetExitCode(const ValueList& args, KValueRef result);
    void _GetArguments(const ValueList& args, KValueRef result);
    void _GetEnvironment(const ValueList& args, KValueRef result);
    void _SetEnvironment(const ValueList& args, KValueRef result);
    void _CloneEnvironment(const ValueList& args, KValueRef result);
    void _Launch(const ValueList& args, KValueRef result);
    void _Terminate(const ValueList& args, KValueRef result);
    void _Kill(const ValueList& args, KValueRef result);
    void _SendSignal(const ValueList& args, KValueRef result);
    void _GetStdin(const ValueList& args, KValueRef result);
    void _GetStdout(const ValueList& args, KValueRef result);
    void _GetStderr(const ValueList& args, KValueRef result);
    void _IsRunning(const ValueList& args, KValueRef result);
    void _SetOnRead(const ValueList& args, KValueRef result);
    void _SetOnExit(const ValueList& args, KValueRef result);
    void _ToString(const ValueList& args, KValueRef result);
    void DetachPipes();

    AutoPtr<Pipe> stdoutPipe;
    AutoPtr<Pipe> stderrPipe;
    AutoPtr<Pipe> stdinPipe;
    KObjectRef environment;
    KListRef args;
    int pid;
    KValueRef exitCode;
    KMethodRef onRead;
    KMethodRef onExit;
    Poco::RunnableAdapter<Process>* exitMonitorAdapter;
    Poco::Thread exitMonitorThread;
    KMethodRef exitCallback;
    bool running;
};

} // namespace Titanium

#endif
