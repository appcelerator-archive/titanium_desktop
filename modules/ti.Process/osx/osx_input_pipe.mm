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
	kroll::Logger *logger = kroll::Logger::Get("Process.TiDataReady");
	NSData *data = [[aNotification userInfo] objectForKey:NSFileHandleNotificationDataItem];
	
	if ([data length]) {
		pipe->DataReady(data);
	}
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
		
		Logger::Get("Process.OSXInputPipe")->Debug("adding notification for dataReady");
		[[NSNotificationCenter defaultCenter] addObserver:dataReady 
			selector:@selector(dataReady:)
			name:NSFileHandleReadCompletionNotification 
			object: handle];
		
		Logger::Get("Process.OSXInputPipe")->Debug("read in background and notify");
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
		Logger *logger = Logger::Get("Process.OSXInputPipe");
		[buffer appendData:data];
		
		logger->Debug("dataReady: data %d bytes, buffer %d bytes", [data length], [buffer length]);
		if ([buffer length] > 0)
		{
			NSString *str = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
			logger->Debug("data: %s", [str UTF8String]);
			[str release];
			InputPipe::DataReady();
		}
	}
	
	void OSXInputPipe::Erase(int nBytes)
	{
		NSRange range;
		range.location = 0;
		range.length = nBytes;
		[buffer replaceBytesInRange:range withBytes:NULL length:0];
	}
	
	AutoPtr<Blob> OSXInputPipe::Read(int bufsize)
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
		
		AutoPtr<Blob> blob = new Blob((const char *)[buffer bytes], bufsize);
		this->Erase(bufsize);
		return blob;
	}
	
	AutoPtr<Blob> OSXInputPipe::ReadLine()
	{
		if (closed && [buffer length] == 0)
		{
			throw ValueException::FromString("Pipe is already closed");
		}
		
		int charsToErase;
		int newline = FindFirstLineFeed((char *)[buffer bytes], [buffer length], &charsToErase);
		if (newline == -1) return NULL;
		
		AutoPtr<Blob> blob = new Blob((const char *)[buffer bytes], newline-charsToErase+1);
		this->Erase(newline+1);
		return blob;
	}
}