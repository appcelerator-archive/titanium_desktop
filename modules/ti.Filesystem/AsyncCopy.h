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

#ifndef AsyncCopy_h
#define AsyncCopy_h

#include <kroll/kroll.h>

#ifdef OS_WIN32
#include <windows.h>
#elif OS_OSX
#import <Foundation/Foundation.h>
#endif

#include <string>
#include <vector>

#include <Poco/Thread.h>
#include <Poco/Exception.h>
#include <Poco/Path.h>
#include <Poco/File.h>

#include "Filesystem.h"

namespace Titanium {

class AsyncCopy : public StaticBoundObject {
public:
    AsyncCopy(Filesystem* parent,kroll::Host *host,std::vector<std::string> files, std::string destination, KMethodRef callback);
    virtual ~AsyncCopy();

private:
    static void Run(void*);

    void ToString(const ValueList& args, KValueRef result);
    void Cancel(const ValueList& args, KValueRef result);
    void Copy(Poco::Path &src, Poco::Path &dest);

    Filesystem* parent;
    Host *host;
    std::vector<std::string> files;
    std::string destination;
    KMethodRef callback;
    Poco::Thread *thread;
    bool stopped;
};

} // namespace Titanium

#endif
