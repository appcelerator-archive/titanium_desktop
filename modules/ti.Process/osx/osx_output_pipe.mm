/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#include "osx_output_pipe.h"

namespace ti
{
	OSXOutputPipe::OSXOutputPipe() : closed(false)
	{
		pipe = [NSPipe pipe];
		handle = [pipe fileHandleForWriting];
		[handle retain];
	}

	OSXOutputPipe::~OSXOutputPipe()
	{
		[handle release];
		handle = NULL;
	}

	void OSXOutputPipe::Close()
	{
		if (!IsClosed()) {
			closed = true;
			[handle closeFile];
			Pipe::Closed();
		}
	}
	
	bool OSXOutputPipe::IsClosed()
	{
		return closed;
	}
	
	int OSXOutputPipe::Write(AutoPtr<Blob> data)
	{
		if (closed)
		{
			throw ValueException::FromString("Pipe is already closed");
		}
		
		@try
		{
			[handle writeData:[NSData dataWithBytes:(void*)data->Get() length:data->Length()]];
			return data->Length();
		}
		@catch(NSException *e)
		{
			Logger::Get("OSXOutputPipe")->Error([[e reason] UTF8String]);
		}
		
		return 0;
	}
}