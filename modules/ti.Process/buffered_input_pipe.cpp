/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#include "buffered_input_pipe.h"
#include "input_pipe.h"

namespace ti
{
	BufferedInputPipe::BufferedInputPipe() : closed(false) { }
	
	SharedPtr<Blob> BufferedInputPipe::Read(int bufsize)
	{
		Poco::Mutex::ScopedLock lock(mutex);
		
		if (!closed)
		{
			if (bufsize == -1 || bufsize > buffer.size())
			{
				bufsize = (int) buffer.size();
			}
			
			SharedPtr<Blob> blob = new Blob(&(buffer[0]), bufsize);
			buffer.erase(buffer.begin(), buffer.begin()+bufsize);
			
			return blob;
		}
		throw ValueException::FromString("This pipe is closed.");
	}
	
	SharedPtr<Blob> BufferedInputPipe::ReadLine()
	{
		Poco::Mutex::ScopedLock lock(mutex);
		
		if (!closed)
		{
			int charsToErase;
			int newline = FindFirstLineFeed(&(buffer[0]), buffer.size(), &charsToErase);
			if (newline == -1) return NULL;
			
			SharedPtr<Blob> blob = new Blob(&(buffer[0]), newline-charsToErase+1);
			buffer.erase(buffer.begin(), buffer.begin()+newline);
			
			return blob;
		}
		throw ValueException::FromString("This pipe is closed.");
	}
	
	int BufferedInputPipe::GetSize()
	{
		Poco::Mutex::ScopedLock lock(mutex);
		
		if (!closed)
		{
			return buffer.size();
		}
		else throw ValueException::FromString("This pipe is closed.");
	}
	
	const char* BufferedInputPipe::GetBuffer()
	{
		Poco::Mutex::ScopedLock lock(mutex);
		return &buffer[0];
	}
	
	void BufferedInputPipe::Append(char *data, int length)
	{
		Poco::Mutex::ScopedLock lock(mutex);
		buffer.insert(buffer.end(), data, data+length);
		
		InputPipe::DataReady();
	}
	
	void BufferedInputPipe::Append(SharedPtr<Blob> blob)
	{
		this->Append((char *)blob->Get(), blob->Length());
	}
	
	void BufferedInputPipe::Close()
	{
		if (!closed)
		{
			buffer.clear();
			closed = true;
			
			InputPipe::Closed();
		}
	}
	
	bool BufferedInputPipe::IsClosed()
	{
		return closed;
	}
	
}