/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */


#include "posix_pipe.h"

namespace ti
{
	PosixPipe::PosixPipe(bool isReader) :
		NativePipe(isReader)
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

	PosixPipe::~PosixPipe()
	{
		this->Close();
	}

	void PosixPipe::Close()
	{
		// Close should only be called if the process (or whatever)
		// it is attached to closes. Thus we want to make sure all
		// the output is cleared from the pipe before closing them.
		// A widowed pipe will eventually stop reading data.
		NativePipe::Close();

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
	}

	int PosixPipe::RawRead(char *buffer, int size)
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

	int PosixPipe::RawWrite(const char *buffer, int size)
	{
		int n;
		do
		{
			n = write(writeHandle, buffer, size);
		}
		while (n < 0 && errno == EINTR);
		if (n >= 0)
		{
			return n;
		}
		else
		{
			throw ValueException::FromString("Error writing blob data to pipe");
		}
	}
}
