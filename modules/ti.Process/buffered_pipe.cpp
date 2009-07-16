/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#include "buffered_pipe.h"
#include "pipe.h"

namespace ti
{
	BufferedPipe::BufferedPipe() : closed(false) { }
	
	AutoPtr<Blob> BufferedPipe::Read(int bufsize)
	{
		if (!closed)
		{
			if (buffer.size() == 0)
			{
				return new Blob();
			}
			
			if (bufsize == -1 || bufsize > (int) buffer.size())
			{
				bufsize = (int) buffer.size();
			}	
			
			mutex.lock();
			AutoPtr<Blob> blob = new Blob(&(buffer[0]), bufsize);
			buffer.erase(buffer.begin(), buffer.begin()+bufsize);
			mutex.unlock();
			
			return blob;
		}
		throw ValueException::FromString("This pipe is closed.");
	}
	
	AutoPtr<Blob> BufferedPipe::ReadLine()
	{
		Poco::Mutex::ScopedLock lock(mutex);
		
		if (!closed)
		{
			if (buffer.size() == 0) return NULL;
			
			int charsToErase;
			int newline = FindFirstLineFeed(&(buffer[0]), buffer.size(), &charsToErase);
			if (newline == -1) return NULL;
			
			AutoPtr<Blob> blob = new Blob(&(buffer[0]), newline-charsToErase+1);
			buffer.erase(buffer.begin(), buffer.begin()+newline+1);
			
			return blob;
		}
		throw ValueException::FromString("This pipe is closed.");
	}
	
	int BufferedPipe::GetSize()
	{
		Poco::Mutex::ScopedLock lock(mutex);
		
		if (!closed)
		{
			return buffer.size();
		}
		else throw ValueException::FromString("This pipe is closed.");
	}
	
	const char* BufferedPipe::GetBuffer()
	{
		Poco::Mutex::ScopedLock lock(mutex);
		return &buffer[0];
	}
	
	void BufferedPipe::Write(char *data, int length)
	{
		Poco::Mutex::ScopedLock lock(mutex);
		buffer.insert(buffer.end(), data, data+length);
		
		Pipe::DataReady();
	}
	
	int BufferedPipe::Write(AutoPtr<Blob> blob)
	{
		this->Write((char *)blob->Get(), blob->Length());
		return blob->Length();
	}
	
	void BufferedPipe::Close()
	{
		Poco::Mutex::ScopedLock lock(mutex);
		if (!closed)
		{
			buffer.clear();
			closed = true;
			
			Pipe::Closed();
		}
	}
	
	bool BufferedPipe::IsClosed()
	{
		return closed;
	}
	
	void BufferedPipe::Flush()
	{
	}
}
