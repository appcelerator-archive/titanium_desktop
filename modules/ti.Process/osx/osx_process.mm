/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#include "osx_process.h"
#include <Poco/Path.h>

@implementation TiOSXProcess

-(id)initWithPath:(NSString*)cmd args:(NSArray*)args host:(Host*)h bound:(KObject*)bo
{
	self = [super init];
	if (self)
	{
		host = h;
		onread = nil;
		onexit = nil;
		bound = bo;
		
		task = [[NSTask alloc] init];
		[task setLaunchPath:cmd];
		[task setArguments:args];
		[task setCurrentDirectoryPath: [[NSFileManager defaultManager] currentDirectoryPath]];

		NSProcessInfo *pi = [NSProcessInfo processInfo];
		[task setEnvironment:[pi environment]];
		
		[task setStandardOutput: [NSPipe pipe]];
		[task setStandardError: [NSPipe pipe]];
		[task setStandardInput: [NSPipe pipe]];
		
		// Here we register as an observer of the NSFileHandleReadCompletionNotification, which lets
		// us know when there is data waiting for us to grab it in the task's file handle (the pipe
		// to which we connected stdout and stderr above). methods will be called when there
		// is data waiting.  The reason we need to do this is because if the file handle gets
		// filled up, the task will block waiting to send data and we'll never get anywhere.
		// So we have to keep reading data from the file handle as we go.
		[[NSNotificationCenter defaultCenter] addObserver:self 
			selector:@selector(getOutData:) 
			name: NSFileHandleReadCompletionNotification 
			object: [[task standardOutput] fileHandleForReading]];
	
		[[NSNotificationCenter defaultCenter] addObserver:self 
			selector:@selector(getErrData:) 
			name: NSFileHandleReadCompletionNotification 
			object: [[task standardError] fileHandleForReading]];

		[[NSNotificationCenter defaultCenter] addObserver:self 
			selector:@selector(terminated:) 
			name: NSTaskDidTerminateNotification 
			object: task];
	
		input = new ti::OSXPipe([[task standardInput] fileHandleForWriting]);
		output = new ti::OSXPipe([[task standardOutput] fileHandleForReading]);
		error = new ti::OSXPipe([[task standardError] fileHandleForReading]);
		
		shared_input = new SharedKObject(input);
		shared_output = new SharedKObject(output);
		shared_error = new SharedKObject(error);
		
		bound->Set("in", Value::NewObject(*shared_input));
		bound->Set("err", Value::NewObject(*shared_error));
		bound->Set("out", Value::NewObject(*shared_output));
	
		// schedule reads
		[[[task standardOutput] fileHandleForReading] readInBackgroundAndNotify];
		[[[task standardError] fileHandleForReading] readInBackgroundAndNotify];

		bound->Set("running",Value::NewBool(true));
	}
	return self;
}
-(void)dealloc
{
	if ([task isRunning])
	{
		[self stop];
	}
	delete shared_input;
	shared_input=NULL;
	delete shared_output;
	shared_output=NULL;
	delete shared_error;
	shared_error=NULL;
	if (onread)
	{
		delete onread;
		onexit = NULL;
	}
	if (onexit)
	{
		delete onexit;
		onexit = NULL;
	}
	[task release];
	[super dealloc];
}
-(NSTask*)task
{
	return task;
}
-(void)start
{
	[task launch];
}
-(void)stop
{
	Logger *logger = Logger::Get("OSXProcess");
	logger->Info("Process stop for task %d",[task isRunning]);
	
	if ([task isRunning])
	{
		[task terminate];
	}
}
-(void)setRead: (SharedKMethod*)method
{
	if (onread)
	{
		delete onread;
	}
	onread = method;
}
-(void)setExit: (SharedKMethod*)method
{
	if (onexit)
	{
		delete onexit;
	}
	onexit = method;
}
-(void)terminated: (NSNotification *)aNotification
{
	Logger *logger = Logger::Get("OSXProcess");
	logger->Info("Process has terminated");

	[[NSNotificationCenter defaultCenter] removeObserver:self name:NSFileHandleReadCompletionNotification object: [[task standardOutput] fileHandleForReading]];
	[[NSNotificationCenter defaultCenter] removeObserver:self name:NSFileHandleReadCompletionNotification object: [[task standardError] fileHandleForReading]];
	[[NSNotificationCenter defaultCenter] removeObserver:self name:NSTaskDidTerminateNotification object: task];

	bound->Set("running",Value::NewBool(false));
	bound->Set("exitCode",Value::NewInt([task terminationStatus]));

	if (onexit && !onexit->isNull())
	{
		ValueList args;
		args.push_back(Value::NewInt([task terminationStatus]));
		try
		{
			host->InvokeMethodOnMainThread(*onexit,args,false);
		}
		catch(...)
		{
		}
	}
	
	// close the streams after our onexit in case they want to read
	// one last time from the stream in the callback
	
	input->Close();
	output->Close();
	error->Close();
}
-(void)getOutData: (NSNotification *)aNotification
{
	NSData *data = [[aNotification userInfo] objectForKey:NSFileHandleNotificationDataItem];
    // If the length of the data is zero, then the task is basically over - there is nothing
    // more to get from the handle so we may as well shut down.
    if ([data length])
    {
        // Send the data on to the controller; we can't just use +stringWithUTF8String: here
        // because -[data bytes] is not necessarily a properly terminated string.
        // -initWithData:encoding: on the other hand checks -[data length]
		NSString *str = [[[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding] autorelease];

		if (onread && !onread->isNull())
		{
			ValueList args;
			args.push_back(Value::NewString([str UTF8String]));
			args.push_back(Value::NewBool(false));
			try
			{
				host->InvokeMethodOnMainThread(*onread,args,false);
			}
			catch(std::exception &e)
			{
				Logger *logger = Logger::Get("OSXProcess");
				logger->Error("Caught exception on sending process output stream. Error = %s",e.what());
			}
		}
		else
		{
			output->OnData(str);
		}
    } 

    // we need to schedule the file handle go read more data in the background again.
    [[aNotification object] readInBackgroundAndNotify];
}
- (void) getErrData: (NSNotification *)aNotification
{
	NSData *data = [[aNotification userInfo] objectForKey:NSFileHandleNotificationDataItem];
    // If the length of the data is zero, then the task is basically over - there is nothing
    // more to get from the handle so we may as well shut down.
    if ([data length])
    {
        // Send the data on to the controller; we can't just use +stringWithUTF8String: here
        // because -[data bytes] is not necessarily a properly terminated string.
        // -initWithData:encoding: on the other hand checks -[data length]
		NSString *str = [[[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding] autorelease];

		if (onread && !onread->isNull())
		{
			ValueList args;
			args.push_back(Value::NewString([str UTF8String]));
			args.push_back(Value::NewBool(true));
			try
			{
				host->InvokeMethodOnMainThread(*onread,args,false);
			}
			catch(std::exception &e)
			{
				Logger *logger = Logger::Get("OSXProcess");
				logger->Error("Caught exception on sending process error stream. Error = %s",e.what());
			}
		}
		else
		{
			error->OnData(str);
		}
    } 

    // we need to schedule the file handle go read more data in the background again.
    [[aNotification object] readInBackgroundAndNotify];
}
-(ti::OSXPipe*) output
{
	return output;
}
-(ti::OSXPipe*) error
{
	return error;
}
@end

namespace ti
{
	OSXProcess::OSXProcess(ProcessBinding* parent, std::string& cmd, std::vector<std::string>& args) :
		StaticBoundObject("Process")
	{
		NSMutableArray *arguments = [[NSMutableArray alloc] init];
		if (args.size()>0)
		{
			std::vector<std::string>::iterator i = args.begin();
			while(i!=args.end())
			{
				std::string arg = (*i++);
				[arguments addObject:[NSString stringWithCString:arg.c_str()]];
			}
		}

		// this is a simple check to see if the path passed
		// in is an actual .app folder and not the full path
		// to the binary. in this case, we'll instead invoke
		// the fullpath to the binary
		size_t found = cmd.rfind(".app");
		if (found!=std::string::npos)
		{
			Poco::Path p(cmd);
			std::string fn = p.getFileName();
			found = fn.find(".app");
			fn = fn.substr(0,found);
			fn = kroll::FileUtils::Join(cmd.c_str(),"Contents","MacOS",fn.c_str(),NULL);
			if (FileUtils::IsFile(fn))
			{
				cmd = fn;
			}
		}
		host = parent->GetHost();
		process = [[TiOSXProcess alloc] initWithPath:[NSString stringWithCString:cmd.c_str() encoding:NSUTF8StringEncoding] args:arguments host:parent->GetHost() bound:this];
		
		this->Set("command",Value::NewString(cmd));
		
		this->SetBool("running", false);
		this->SetNull("exitCode");
		this->SetNull("onread");
		this->SetNull("onexit");
		this->SetMethod("terminate",&OSXProcess::Terminate);
		this->SetInt("pid", -1);

		[arguments release];

		// start the process
		@try
		{
			[process start];
			this->SetInt("pid",[[process task] processIdentifier]);
		}
		@catch(NSException *e)
		{
			throw ValueException::FromString([[e reason] UTF8String]);
		}
	}
	OSXProcess::~OSXProcess()
	{
		Logger *logger = Logger::Get("OSXProcess");
		logger->Debug("OSXProcess destructor called");
		
		[process stop];
		[process release];
	}
	void OSXProcess::Terminate(const ValueList& args, SharedValue result)
	{
		Logger *logger = Logger::Get("OSXProcess");
		logger->Info("Terminate process called");
		
		[process stop];
		[process setRead:NULL];
		[process setExit:NULL];
	}
	void OSXProcess::Bound(const char *name, SharedValue value)
	{
		std::string fn(name);
		if (fn == "onread" && !value.isNull() && value->IsMethod())
		{
			if (process && [[process task] isRunning])
			{
				[process setRead:new SharedKMethod(value->ToMethod())];
			}
			else
			{
				// if we get here, we need to pull the data from the 
				// pipe(s) and send it to the listener since we've exited
				// before we can attach the read listeners
				NSString *stdout = [process output]->GetData();
				if ([stdout length] > 0)
				{
					try
					{
						ValueList args;
						args.push_back(Value::NewString([stdout UTF8String]));
						host->InvokeMethodOnMainThread(value->ToMethod(),args,true);
					}
					catch(std::exception &e)
					{
						Logger *logger = Logger::Get("OSXProcess");
						logger->Error("Caught exception on sending stdout onexit. Error = %s",e.what());
					}
				}
				NSString *stderr = [process error]->GetData();
				if ([stderr length] > 0)
				{
					try
					{
						ValueList args;
						args.push_back(Value::NewString([stderr UTF8String]));
						host->InvokeMethodOnMainThread(value->ToMethod(),args,true);
					}
					catch(std::exception &e)
					{
						Logger *logger = Logger::Get("OSXProcess");
						logger->Error("Caught exception on sending stderr onexit. Error = %s",e.what());
					}
				}
			}
		}
		else if (fn == "onexit" && !value.isNull() && value->IsMethod())
		{
			if (process && [[process task] isRunning])
			{
				[process setExit:new SharedKMethod(value->ToMethod())];
			}
			else
			{
				// if we get here, we've exited before we attach the 
				// onexit listener in which case we need to immediately
				// notify the listener
				try
				{
					SharedValue exitCode = this->Get("exitCode");
					ValueList args;
					args.push_back(exitCode);
					host->InvokeMethodOnMainThread(value->ToMethod(),args,true);
				}
				catch(std::exception &e)
				{
					Logger *logger = Logger::Get("OSXProcess");
					logger->Error("Caught exception on sending immediate onexit. Error = %s",e.what());
				}
			}
		}
	}
}
