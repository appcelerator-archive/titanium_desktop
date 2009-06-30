/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#include "osx_pipe.h"

// max size of buffer we keep until you call read
#define MAX_MEMORY_BUFFER 16384

namespace ti
{
	OSXPipe::OSXPipe(NSFileHandle *handle) : StaticBoundObject("Pipe"), handle(handle), closed(false)
	{
		[handle retain];
		data = [[NSMutableString alloc] init];
		[data setString:@""];
		
		// NOTE: don't doc these, since they're already doc'd in
		// Pipe
		this->Set("closed",Value::NewBool(false));
		this->SetMethod("close",&OSXPipe::Close);
		this->SetMethod("write",&OSXPipe::Write);
		this->SetMethod("read",&OSXPipe::Read);
	}
	OSXPipe::~OSXPipe()
	{
		Close();
		[handle release];
		handle = NULL;
		[data release];
	}
	void OSXPipe::Write(const ValueList& args, SharedValue result)
	{
		if (closed)
		{
			throw ValueException::FromString("Pipe is already closed");
		}
		if (!args.at(0)->IsString())
		{
			throw ValueException::FromString("Can only write string data");
		}
		const char *buf = args.at(0)->ToString();
		try
		{
			@try
			{
				NSString *str = [NSString stringWithCString:buf encoding:NSUTF8StringEncoding];
				NSData *data = [str dataUsingEncoding:NSUTF8StringEncoding];
				[handle writeData:data];
				result->SetInt([data length]);
			}
			@catch(NSException *e)
			{
				result->SetInt(0);
			}
		}
		catch(...)
		{
			result->SetInt(0);
		}
	}
	void OSXPipe::OnData(NSString *str)
	{
		if ([data length] > MAX_MEMORY_BUFFER)
		{
			[data setString:str];
		}
		else
		{
			[data appendString:str];
		}
	}
	NSString* OSXPipe::GetData()
	{
		return data;
	}
	
	void OSXPipe::Read(const ValueList& args, SharedValue result)
	{
		if (closed)
		{
			throw ValueException::FromString("Pipe is already closed");
		}
		try
		{
			@try
			{
				result->SetString([data UTF8String]);
				
				// reset on each read
				[data setString:@""];
			}
			@catch(NSException *e)
			{
				result->SetNull();
			}
		}
		catch(...)
		{
			result->SetNull();
		}
	}
	void OSXPipe::Close(const ValueList& args, SharedValue result)
	{
		Close();
	}
	void OSXPipe::Close()
	{
		if (!closed)
		{
			closed=true;
			this->Set("closed",Value::NewBool(true));
			[handle closeFile];
		}
	}
}
