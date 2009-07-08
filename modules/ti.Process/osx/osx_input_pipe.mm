/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#include "osx_input_pipe.h"
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

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
	NSData *data = [[aNotification userInfo] objectForKey:NSFileHandleNotificationDataItem];
	if ([data length]) {
		pipe->DataReady(data);
	}
	
	// we need to schedule the file handle to wait for more data in the background again.
	[[aNotification object] readInBackgroundAndNotify];
}

@end

namespace ti
{
	OSXInputPipe::OSXInputPipe () : closed(false)
	{
		buffer = [NSMutableData dataWithCapacity:1024];
		[buffer retain];
		
		pipe = [NSPipe pipe];
		handle = [pipe fileHandleForReading];
		[handle retain];
		
		dataReady = [[TiDataReady alloc] initWithPipe:this];
		[dataReady retain];
		[[NSNotificationCenter defaultCenter] addObserver:dataReady 
			selector:@selector(dataReady:)
			name:NSFileHandleReadCompletionNotification 
			object: handle];
		
		[handle readInBackgroundAndNotify];
	}
	
	OSXInputPipe::~OSXInputPipe ()
	{
		Close();
		
		//[handle release];
		[dataReady release];
		[buffer release];
		handle = NULL;
	}
	
	void OSXInputPipe::Close()
	{
		if (!IsClosed()) {
			[[NSNotificationCenter defaultCenter] removeObserver:dataReady
				name:NSFileHandleReadCompletionNotification
				object:handle];
			
			closed = true;
			
			bool handleClosed = false;
			@try
			{
				[handle offsetInFile];
			}
			@catch (NSException* e)
			{
				handleClosed = true;
			}
			if (!handleClosed) {
				[handle closeFile];
			}
			
			SetOnRead(NULL);
			
			Pipe::Closed();
		}
	}
	
	bool OSXInputPipe::IsClosed()
	{
		return closed;
	}
	
	void OSXInputPipe::DataReady(NSData *data)
	{
		[buffer appendData:data];
		InputPipe::DataReady();
	}
	
	void OSXInputPipe::Erase(int nBytes)
	{
		NSRange range;
		range.location = 0;
		range.length = nBytes;
		[buffer replaceBytesInRange:range withBytes:NULL length:0];
	}
	
	SharedPtr<Blob> OSXInputPipe::Read(int bufsize)
	{
		// let danging onRead events pull the last data from the buffer
		if (closed && [buffer length] == 0)
		{
			throw ValueException::FromString("Pipe is already closed");
		}
		
		int currentLength = [buffer length];
		if (currentLength == 0)
		{
			return new Blob();
		}
		
		if (bufsize == -1 || bufsize > currentLength)
		{
			bufsize = currentLength;
		}
		
		SharedPtr<Blob> blob = new Blob((const char *)[buffer bytes], bufsize);
		this->Erase(bufsize);
		return blob;
	}
	
	SharedPtr<Blob> OSXInputPipe::ReadLine()
	{
		if (closed && [buffer length] == 0)
		{
			throw ValueException::FromString("Pipe is already closed");
		}
		
		int charsToErase;
		int newline = FindFirstLineFeed((char *)[buffer bytes], [buffer length], &charsToErase);
		if (newline == -1) return NULL;
		
		SharedPtr<Blob> blob = new Blob((const char *)[buffer bytes], newline-charsToErase+1);
		this->Erase(newline+1);
		return blob;
	}
}