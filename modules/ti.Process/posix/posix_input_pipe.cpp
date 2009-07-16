/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */


#include "posix_input_pipe.h"

namespace ti
{
	PosixInputPipe::PosixInputPipe()
	{
		int fds[2];
		int rc = pipe(fds);
		if (rc == 0)
		{
			readHandle  = fds[0];
			writeHandle = fds[1];
		}
		else throw ValueException::FromString("Error creating input pipe");
	}
	
	PosixInputPipe::~PosixInputPipe()
	{
		this->Close();
	}
	
	void PosixInputPipe::Close()
	{
		if (!closed)
		{
			if (readHandle != -1)
			{
				close(readHandle);
				readHandle = -1;
			}
			if (writeHandle != -1)
			{
				close(writeHandle);
				writeHandle = -1;
			}
			MonitoredInputPipe::Close();
		}
	}
	
	int PosixInputPipe::RawRead(char *buffer, int size)
	{
		int n;
		do
		{
			n = read(readHandle, buffer, size);
		}
		while (n < 0 && errno == EINTR);
		
		if (n >= 0)
		{
			return n;
		}
		else
		{
			throw ValueException::FromString("Error reading from anonymous pipe");
		}
	}
	
}
