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

#include "FileStream.h"

#include <cstring>
#include <sstream>

#include <Poco/LineEndingConverter.h>

namespace Titanium {

FileStream::FileStream(std::string filename) :
    Stream("Filesystem.FileStream"),
    istream(0), ostream(0), stream(0)
{
#ifdef OS_OSX
    // in OSX, we need to expand ~ in paths to their absolute path value
    // we do that with a nifty helper method in NSString
    this->filename = [[[NSString stringWithUTF8String:filename.c_str()]
         stringByExpandingTildeInPath] fileSystemRepresentation];
#else
    this->filename = filename;
#endif

    this->SetMethod("open", &FileStream::_Open);
    this->SetMethod("isOpen", &FileStream::_IsOpen);
    this->SetMethod("close", &FileStream::_Close);
    this->SetMethod("seek", &FileStream::_Seek);
    this->SetMethod("tell", &FileStream::_Tell);
    this->SetMethod("write", &FileStream::_Write);
    this->SetMethod("read", &FileStream::_Read);

    this->SetMethod("readLine", &FileStream::_ReadLine);
    this->SetMethod("writeLine", &FileStream::_WriteLine);
    this->SetMethod("ready", &FileStream::_Ready);

    // These should be depricated and no longer used.
    // All constants should be kept on Ti.Filesystem object.
    this->Set("MODE_READ", Value::NewInt(MODE_READ));
    this->Set("MODE_APPEND", Value::NewInt(MODE_APPEND));
    this->Set("MODE_WRITE", Value::NewInt(MODE_WRITE));
}

FileStream::~FileStream()
{
    this->Close();
}

bool FileStream::Open(FileStreamMode mode, bool binary, bool append)
{
    // close the prev stream if needed
    this->Close();

    try
    {
        std::ios::openmode flags = (std::ios::openmode) 0;
        bool output = false;

        if (binary)
        {
            flags|=std::ios::binary;
        }

        if (mode == MODE_APPEND)
        {
            flags|=std::ios::out|std::ios::app;
            output = true;
        }
        else if (mode == MODE_WRITE)
        {
            flags |= std::ios::out|std::ios::trunc;
            output = true;
        }
        else if (mode == MODE_READ)
        {
            flags |= std::ios::in;
        }

        if (output)
        {
            this->ostream = new Poco::FileOutputStream(this->filename,flags);
            this->stream = this->ostream;
#ifndef OS_WIN32
            chmod(this->filename.c_str(),S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
#endif      
        }
        else
        {
            this->istream = new Poco::FileInputStream(this->filename,flags);
            this->stream = this->istream;
        }

        return true;
    }
    catch (Poco::Exception& exc)
    {
        Logger* logger = Logger::Get("Filesystem.FileStream");
        logger->Error("Error in open. Exception: %s",exc.displayText().c_str());
        throw ValueException::FromString(exc.displayText());
    }
}

bool FileStream::IsOpen() const
{
    return this->stream != NULL;
}

void FileStream::Close()
{
    try
    {
        if (this->stream)
        {
            if (this->ostream)
            {
                this->ostream->flush();
            }

            this->stream->close();
            delete this->stream;
            this->stream = NULL;
            this->istream = NULL;
            this->ostream = NULL;
        }
    }
    catch (Poco::Exception& exc)
    {
        Logger* logger = Logger::Get("Filesystem.FileStream");
        logger->Error("Error in close. Exception: %s",exc.displayText().c_str());
        throw ValueException::FromString(exc.displayText());
    }
}

void FileStream::Seek(int offset, int direction)
{
    if (this->istream)
        this->istream->seekg(offset, (std::ios::seekdir)direction);
    else if (this->ostream)
        this->ostream->seekp(offset, (std::ios::seekdir)direction);
    else
        throw ValueException::FromString("FileStream must be opened before seeking");
}

int FileStream::Tell()
{
    if (this->istream)
        return this->istream->tellg();
    else if (this->ostream)
        return this->ostream->tellp();

    throw ValueException::FromString("FileStream must be opend before using tell");
}

void FileStream::Write(const char* buffer, size_t size)
{
    if(!this->ostream)
        throw ValueException::FromString("FileStream must be opened for writing before calling write");

    try {
        this->ostream->write(buffer, size);
    }
    catch (Poco::Exception& ex) {
        Logger* logger = Logger::Get("Filesystem.FileStream");
        logger->Error("Error in write. Exception: %s",ex.displayText().c_str());
        throw ValueException::FromString(ex.displayText());
    }
}

bool FileStream::IsWritable() const
{
    return this->ostream != NULL;
}

size_t FileStream::Read(const char* buffer, size_t size)
{
    if(!this->istream)
        throw ValueException::FromString("FileStream must be opened for writing before calling write");

    try {
        this->istream->read((char*)buffer, size);
        return this->istream->gcount();
    }
    catch (Poco::Exception& ex) {
        Logger* logger = Logger::Get("Filesystem.FileStream");
        logger->Error("Error in read. Exception: %s",ex.displayText().c_str());
        throw ValueException::FromString(ex.displayText());
    }
}

bool FileStream::IsReadable() const
{
    return this->istream != NULL;
}

void FileStream::_Open(const ValueList& args, KValueRef result)
{
    args.VerifyException("open", "?ibb");

    FileStreamMode mode = (FileStreamMode) args.GetInt(0, MODE_READ);
    bool binary = args.GetBool(1, false);
    bool append = args.GetBool(2, false);

    bool opened = this->Open(mode, binary, append);
    result->SetBool(opened);
}

void FileStream::_IsOpen(const ValueList& args, KValueRef result)
{
    result->SetBool(this->IsOpen());
}

void FileStream::_Close(const ValueList& args, KValueRef result)
{
    this->Close();
}

void FileStream::_Seek(const ValueList& args, KValueRef result)
{
    args.VerifyException("seek", "i?i");

    int offset = args.GetInt(0);
    int direction = args.GetInt(1, std::ios::beg);
    this->Seek(offset, direction);
}

void FileStream::_Tell(const ValueList& args, KValueRef result)
{
    result->SetInt(this->Tell());
}

void FileStream::_Write(const ValueList& args, KValueRef result)
{
    args.VerifyException("write", "s|o|n");

    char *text = NULL;
    int size = 0;
    if (args.at(0)->IsObject())
    {
        KObjectRef b = args.at(0)->ToObject();
        AutoPtr<Bytes> bytes = b.cast<Bytes>();
        if (!bytes.isNull())
        {
            text = (char*)bytes->Pointer();
            size = (int)bytes->Length();
        }
    }
    else if (args.at(0)->IsString())
    {
        text = (char*)args.at(0)->ToString();
    }
    else if (args.at(0)->IsInt())
    {
        std::stringstream ostr;
        ostr << args.at(0)->ToInt();
        text = (char*)ostr.str().c_str();
        size = ostr.str().length();
    }
    else if (args.at(0)->IsDouble())
    {
        std::stringstream ostr;
        ostr << args.at(0)->ToDouble();
        text = (char*)ostr.str().c_str();
        size = ostr.str().length();
    }
    else
    {
        throw ValueException::FromString("Could not write with type passed");
    }

    if (size==0)
    {
        size = strlen(text);
    }

    if (text == NULL || size <= 0)
    {
        result->SetBool(false);
        return;
    }

    Write(text,size);
    result->SetBool(true);
}

void FileStream::_Read(const ValueList& args, KValueRef result)
{
    args.VerifyException("read", "?i");

    try
    {
        if (args.size() >= 1)
        {
            int size = args.GetInt(0);
            if (size <= 0)
                throw ValueException::FromString("File.read() size must be greater than zero");

            BytesRef buffer = new Bytes(size + 1);
            int readCount = this->Read(buffer->Pointer(), size);

            if (readCount > 0)
            {
                buffer->Write("\0", 1, readCount);
                result->SetObject(buffer);
            }
            else
            {
                // No data read, must be at EOF
                result->SetNull();
            }
        }
        else
        {
            // If no read size is provided, read the entire file.
            // TODO: this is not a very efficent method and should be deprecated
            // and replaced by buffered stream transports
            std::vector<char> buffer;
            char data[4096];

            while (!this->istream->eof())
            {
                this->istream->read((char*)&data, 4095);
                int length = this->istream->gcount();
                if (length > 0)
                {
                    buffer.insert(buffer.end(), data, data+length);
                }
                else break;
            }

            result->SetObject(new Bytes(&(buffer[0]), buffer.size()));
        }
    }
    catch (Poco::Exception& exc)
    {
        Logger* logger = Logger::Get("Filesystem.FileStream");
        logger->Error("Error in read. Exception: %s",exc.displayText().c_str());
        throw ValueException::FromString(exc.displayText());
    }
}

void FileStream::_ReadLine(const ValueList& args, KValueRef result)
{
    try
    {
        if (!this->istream)
        {
            Logger* logger = Logger::Get("Filesystem.FileStream");
            logger->Error("Error in readLine. FileInputStream is null");
            throw ValueException::FromString("FileStream must be opened for reading before calling readLine");
        }

        if (this->istream->eof())
        {
            // close the file
            result->SetNull();
        }
        else
        {
            std::string line;
            std::getline(*this->istream, line);
#ifdef OS_WIN32
            // In some cases std::getline leaves a CR on the end of the line in win32 -- why God, why?
            if (!line.empty())
            {
                char lastChar = line.at(line.size()-1);
                if (lastChar == '\r') {
                    line = line.substr(0, line.size()-1);
                }
            }
#endif
            if (line.empty() || line.size()==0)
            {
                if (this->istream->eof())
                {
                    // if this is EOF, return null
                    result->SetNull();
                }
                else
                {
                    // this is an empty line, just empty Bytes object.
                    result->SetObject(new Bytes());
                }
            }
            else
            {
                result->SetObject(new Bytes(line));
            }
        }
    }
    catch (Poco::Exception& exc)
    {
        Logger* logger = Logger::Get("Filesystem.FileStream");
        logger->Error("Error in readLine. Exception: %s",exc.displayText().c_str());
        throw ValueException::FromString(exc.displayText());
    }
}

void FileStream::_WriteLine(const ValueList& args, KValueRef result)
{
    args.VerifyException("writeLine", "s|o|n");

    if(! this->stream)
    {
        throw ValueException::FromString("FileStream must be opened before calling readLine");
    }

    char *text = NULL;
    int size = 0;
    if (args.at(0)->IsObject())
    {
        KObjectRef b = args.at(0)->ToObject();
        AutoPtr<Bytes> bytes = b.cast<Bytes>();
        if (!bytes.isNull())
        {
            text = (char*)bytes->Pointer();
            size = (int)bytes->Length();
        }
    }
    else if (args.at(0)->IsString())
    {
        text = (char*)args.at(0)->ToString();
    }
    else if (args.at(0)->IsInt())
    {
        std::stringstream ostr;
        ostr << args.at(0)->ToInt();
        text = (char*)ostr.str().c_str();
        size = ostr.str().length();
    }
    else if (args.at(0)->IsDouble())
    {
        std::stringstream ostr;
        ostr << args.at(0)->ToDouble();
        text = (char*)ostr.str().c_str();
        size = ostr.str().length();
    }
    else
    {
        throw ValueException::FromString("Could not write with type passed");
    }

    if (size==0)
    {
        size = strlen(text);
    }

    if (text == NULL || size <= 0)
    {
        result->SetBool(false);
        return;
    }

    std::string astr = text;
#ifdef OS_WIN32
    astr += "\r\n";
#else
    astr += "\n";
#endif
    Write((char*)astr.c_str(),astr.length());
    result->SetBool(true);
}

void FileStream::_Ready(const ValueList& args, KValueRef result)
{
    if(!this->stream)
    {
        result->SetBool(false);
    }
    else
    {
        result->SetBool(this->stream->eof()==false);
    }
}

} // namespace Titanium
