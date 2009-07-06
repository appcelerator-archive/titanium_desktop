/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#include "osx_input_pipe.h"

@implementation TiDataReady

-(id)initWithPipe:(ti::OSXInputPipe*)p
{
	self = [super init];
	if (self)
	{
		pipe = p;
	}
	return self;
}

-(void)dataReady:(NSNotification *)aNotification
{
	pipe->DataReady();
	if (!pipe->IsClosed())
	{
		// we need to schedule the file handle to wait for more data in the background again.
		[[aNotification object] waitForDataInBackgroundAndNotify];
	}
}

@end

namespace ti
{
	OSXInputPipe::OSXInputPipe () : closed(false)
	{
		pipe = [NSPipe pipe];
		handle = [pipe fileHandleForReading];
		[handle retain];
		
		dataReady = [[TiDataReady alloc] initWithPipe:this];
		[dataReady retain];
		[[NSNotificationCenter defaultCenter] addObserver:dataReady 
			selector:@selector(dataReady:)
			name:NSFileHandleDataAvailableNotification 
			object: handle];
		
		[handle waitForDataInBackgroundAndNotify];
	}
	
	OSXInputPipe::~OSXInputPipe ()
	{
		Close();
		[[NSNotificationCenter defaultCenter] removeObserver:dataReady
			name:NSFileHandleDataAvailableNotification
			object: handle];
		
		//[handle release];
		//[dataReady release];
		handle = NULL;
	}
	
	void OSXInputPipe::Close()
	{
		if (!IsClosed()) {
			closed = true;
			[handle closeFile];
			SetOnRead(NULL);
			Pipe::Closed();
		}
	}
	
	bool OSXInputPipe::IsClosed()
	{
		return closed;
	}
	
	SharedPtr<Blob> OSXInputPipe::Read(int bufsize)
	{
		if (closed)
		{
			throw ValueException::FromString("Pipe is already closed");
		}
		
		@try
		{
			NSData *data = nil;
			if (bufsize == -1)
			{
				data = [handle availableData];
			}
			else
			{
				data = [handle readDataOfLength:bufsize];
			}
		
			SharedPtr<Blob> blob = new Blob((const char*)[data bytes], [data length]);
			//[data release];
			return blob;
		}
		@catch(NSException *e)
		{
			Logger::Get("OSXInputPipe")->Error([[e reason] UTF8String]);
		}
		
		return NULL;
	}
}