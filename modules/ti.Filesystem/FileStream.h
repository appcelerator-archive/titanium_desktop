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

class FileStream : public Stream {
public:
    enum FileStreamMode {
        MODE_READ = 1,
        MODE_APPEND = 2,
        MODE_WRITE = 3
    };

    FileStream(std::string filename);
    virtual ~FileStream();

    bool Open(FileStreamMode mode, bool binary = false, bool append = false);
    bool IsOpen() const;
    void Close();

    void Seek(int offset, int direction);
    int Tell();

    virtual void Write(const char* buffer, size_t size);
    virtual bool IsWritable() const;
    virtual size_t Read(const char* buffer, size_t size);
    virtual bool IsReadable() const;

    // TODO: make this private once removed from File.
    void _Open(const ValueList& args, KValueRef result);

private:
    void _IsOpen(const ValueList& args, KValueRef result);
    void _Close(const ValueList& args, KValueRef result);
    void _Seek(const ValueList& args, KValueRef result);
    void _Tell(const ValueList& args, KValueRef result);
    void _Write(const ValueList& args, KValueRef result);
    void _Read(const ValueList& args, KValueRef result);

    void _ReadLine(const ValueList& args, KValueRef result);
    void _WriteLine(const ValueList& args, KValueRef result);
    void _Ready(const ValueList& args, KValueRef result);

    std::string filename;

    Poco::FileInputStream* istream;
    Poco::FileOutputStream* ostream;
    Poco::FileIOS* stream;
};

} // namespace Titanium

#endif 
