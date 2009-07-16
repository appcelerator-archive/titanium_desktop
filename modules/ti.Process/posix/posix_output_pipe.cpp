/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#include "posix_output_pipe.h"

namespace ti
{
	PosixOutputPipe::PosixOutputPipe() : closed(false)
	{
		int fds[2];
		int rc = pipe(fds);
		if (rc == 0)
		{
			readHandle  = fds[0];
			writeHandle = fds[1];
		}
		else throw ValueException::FromString("Error creating output pipe");
	}
	
	PosixOutputPipe::~PosixOutputPipe()
	{
		Close();
	}
	
	bool PosixOutputPipe::IsClosed()
	{
		return closed;	
	}

	void PosixOutputPipe::Close()
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
			closed = true;
			
			OutputPipe::Closed();
		}
	}
	
	int PosixOutputPipe::Write(AutoPtr<Blob> blob)
	{
		int n;
		do
		{
			n = write(writeHandle, blob->Get(), blob->Length());
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
	
	void PosixOutputPipe::Flush()
	{
		if (writeHandle != -1) {
			close(writeHandle);
		}
	}
}
