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

#ifndef FileStream_h
#define FileStream_h

#ifdef OS_OSX
#import <Foundation/Foundation.h>
#endif

#include <string>

#include <kroll/kroll.h>
#include <Poco/FileStream.h>

namespace Titanium {

enum FileStreamMode {
    MODE_READ = 1,
    MODE_APPEND = 2,
    MODE_WRITE = 3
};


class FileStream : public StaticBoundObject {
public:
    FileStream(std::string filename_);
    virtual ~FileStream();

    // Used by File.open()
    void Open(const ValueList& args, KValueRef result);

private:
    std::string filename;

    Poco::FileInputStream* istream;
    Poco::FileOutputStream* ostream;
    Poco::FileIOS* stream;

    bool Open(FileStreamMode mode, bool binary = false, bool append = false);
    void Close(const ValueList& args, KValueRef result);
    bool Close();
    void Write(const ValueList& args, KValueRef result);
    void Write(char *,int);
    void Read(const ValueList& args, KValueRef result);
    void ReadLine(const ValueList& args, KValueRef result);
    void WriteLine(const ValueList& args, KValueRef result);
    void Ready(const ValueList& args, KValueRef result);
    void IsOpen(const ValueList& args, KValueRef result);
    void Seek(const ValueList& args, KValueRef result);
    void Tell(const ValueList& args, KValueRef result);
};

} // namespace Titanium

#endif 
