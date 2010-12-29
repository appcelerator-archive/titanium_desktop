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

#ifndef PipeWin_h
#define PipeWin_h

#include <kroll/kroll.h>

#include "../NativePipe.h"

namespace Titanium {

class PipeWin : public NativePipe {
public:
    PipeWin(bool isReader);

    virtual void CreateHandles();
    virtual void Close();
    virtual void CloseNativeRead();
    virtual void CloseNativeWrite();
    void DuplicateWrite(HANDLE process, LPHANDLE handle);
    void DuplicateRead(HANDLE process, LPHANDLE handle);
    HANDLE GetReadHandle() { return readHandle; }
    HANDLE GetWriteHandle() { return writeHandle; }
    
protected:
    virtual int RawRead(char *buffer, int size);
    virtual int RawWrite(const char *buffer, int size);
    
    Poco::Mutex mutex;
    HANDLE readHandle, writeHandle;
    Logger *logger;
};

} // namespace Titanium

#endif
