/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#include "blob_input_pipe.h"
#include "input_pipe.h"

namespace ti
{
	BlobInputPipe::BlobInputPipe() : closed(false) { }
	
	SharedPtr<Blob> BlobInputPipe::Read(int bufsize)
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
	
	int BlobInputPipe::GetSize()
	{
		Poco::Mutex::ScopedLock lock(mutex);
		
		if (!closed)
		{
			return buffer.size();
		}
		else throw ValueException::FromString("This pipe is closed.");
	}
	
	const char* BlobInputPipe::GetBuffer()
	{
		Poco::Mutex::ScopedLock lock(mutex);
		return &buffer[0];
	}
	
	void BlobInputPipe::Append(SharedPtr<Blob> blob)
	{
		Poco::Mutex::ScopedLock lock(mutex);
		buffer.insert(buffer.end(), blob->Get(), blob->Get()+blob->Length());
		
		InputPipe::DataReady();
	}
	
	void BlobInputPipe::Close()
	{
		if (!closed)
		{
			buffer.clear();
			closed = true;
			
			InputPipe::Closed();
		}
	}
	
	bool BlobInputPipe::IsClosed()
	{
		return closed;
	}
	
}