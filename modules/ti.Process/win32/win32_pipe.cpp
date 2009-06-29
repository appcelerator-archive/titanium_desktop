/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#include "win32_pipe.h"

namespace ti
{
	Win32Pipe::Win32Pipe() : StaticBoundObject("Pipe"), closed(false)
	{
		SECURITY_ATTRIBUTES attr;
		attr.nLength              = sizeof(attr);
		attr.lpSecurityDescriptor = NULL;
		attr.bInheritHandle       = FALSE;
		
		CreatePipe(&read, &write, &attr, 0);
		
		/**
		 * @tiapi(property=True,type=boolean,name=Process.Pipe.closed,since=0.2) Whether or not a pipe is closed
		 */
		this->Set("closed",Value::NewBool(false));
		/**
		 * @tiapi(method=True,name=Process.Pipe.close,since=0.2) Closes the pipe
		 */
		this->SetMethod("close",&Win32Pipe::Close);
		/**
		 * @tiapi(method=True,name=Process.Pipe.write,since=0.2) Writes data to the pipe
		 * @tiarg(for=Process.Pipe.write,type=string,name=data) data to write
		 */
		this->SetMethod("write",&Win32Pipe::Write);
		/**
		 * @tiapi(method=True,name=Process.Pipe.read,since=0.2) Reads data from the pipe
		 * @tiresult(for=Process.Pipe.read,type=string) result read from pipe
		 */
		this->SetMethod("read",&Win32Pipe::Read);
	}
	
	Win32Pipe::~Win32Pipe()
	{
		Close();
	}
	
	int Win32Pipe::Write(char *buffer, int length)
	{
		if (write != INVALID_HANDLE_VALUE) {
			DWORD bytesWritten;
			BOOL ok = WriteFile(this->GetWriteHandle(), buffer, length, &bytesWritten, NULL);
			if (ok || GetLastError() == ERROR_BROKEN_PIPE) {
				return bytesWritten;
			}
			else {
				throw ValueException::FromString("Error writing anonymous pipe");
			}
		}
		return -1;
	}
	
	void Win32Pipe::Write(const ValueList& args, SharedValue result)
	{
		if (closed)
		{
			throw ValueException::FromString("Pipe is already closed");
		}
		if (!args.at(0)->IsString())
		{
			throw ValueException::FromString("Can only write string data");
		}
		
		std::string str = args.at(0)->ToString();
		int written = this->Write((char *)str.c_str(), str.size());
		result->SetInt(written);
	}
	
	int Win32Pipe::Read(char *buffer, int length)
	{
		if (read != INVALID_HANDLE_VALUE) {
			DWORD bytesRead;
			BOOL ok = ReadFile(this->GetReadHandle(), buffer, length, &bytesRead, NULL);
			if (ok || GetLastError() == ERROR_BROKEN_PIPE) {
				return bytesRead;
			}
			else {
				throw ValueException::FromString("Error writing anonymous pipe");
			}
		}
		return -1;
	}
	
	void Win32Pipe::Read(const ValueList& args, SharedValue result)
	{
		if (closed)
		{
			throw ValueException::FromString("Pipe is already closed");
		}
		
		int bufferSize = 1024;
		if (args.size() > 0 && args.at(0)->IsInt())
		{
			bufferSize = args.at(0)->ToInt();
		}
		
		char *buffer = new char[bufferSize+1];
		int read = this->Read(buffer, bufferSize);
		if (read > 0) {
			buffer[read] = '\0';
			result->SetString(buffer);
		}
		delete [] buffer;
	}
	
	void Win32Pipe::Close(const ValueList& args, SharedValue result)
	{
		Close();
	}

	void Win32Pipe::Close()
	{
		if (!closed) {
			closed = true;
			
			if (read != INVALID_HANDLE_VALUE)
				CloseHandle(read);
			if (write != INVALID_HANDLE_VALUE)
				CloseHandle(write);
		}
	}
}