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
 
#ifndef ProcessWin_h
#define ProcessWin_h

#include <sstream>

#include "PipeWin.h"
#include "../Process.h"
#include "../NativePipe.h"

namespace Titanium {

class ProcessWin : public Process {
public:
    ProcessWin();
    virtual ~ProcessWin();

    inline virtual AutoPtr<NativePipe> GetNativeStdin() { return nativeIn; }
    inline virtual AutoPtr<NativePipe> GetNativeStdout() { return nativeOut; }
    inline virtual AutoPtr<NativePipe> GetNativeStderr() { return nativeErr; }

    virtual int GetPID();
    virtual void Terminate();
    virtual void Kill();
    virtual void SendSignal(int signal);
    virtual void ForkAndExec();
    virtual void MonitorAsync();
    virtual BytesRef MonitorSync();
    virtual int Wait();
    virtual std::string ArgumentsToString();
    void ReadCallback(const ValueList& args, KValueRef result);
    virtual void RecreateNativePipes();
    
protected:
    std::string ArgListToString(KListRef argList);
    
    Poco::Thread exitMonitorThread;
    Poco::RunnableAdapter<ProcessWin>* exitMonitorAdapter;
    AutoPtr<PipeWin> nativeIn, nativeOut, nativeErr;
    Poco::Mutex mutex;
    
    Poco::Mutex processOutputMutex;
    std::vector<BytesRef> processOutput;
    
    int pid;
    HANDLE process;
    
    Logger* logger;
};

} // namespace Titanium

#endif
