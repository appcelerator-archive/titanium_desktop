/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#include "linux_output_pipe.h"

namespace ti
{
	LinuxOutputPipe::LinuxOutputPipe() : closed(false)
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
	
	LinuxOutputPipe::~LinuxOutputPipe()
	{
		Close();
	}
	
	bool LinuxOutputPipe::IsClosed()
	{
		return closed;	
	}

	void LinuxOutputPipe::Close()
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
			OutputPipe::Closed();
		}
	}
	
	int LinuxOutputPipe::Write(AutoPtr<Blob> blob)
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
}
