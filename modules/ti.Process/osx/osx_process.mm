/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#include "osx_process.h"
#include <signal.h>
#include <Poco/Path.h>

@implementation TiOSXProcess

-(id)initWithProcess: (ti::OSXProcess*)p
{
	self = [super init];
	if (self)
	{
		process = p;
		
		task = [[NSTask alloc] init];
		SharedKList args = process->GetArgs();
		std::string cmd = args->At(0)->ToString();
		// this is a simple check to see if the path passed
		// in is an actual .app folder and not the full path
		// to the binary. in this case, we'll instead invoke
		// the fullpath to the binary
		size_t found = cmd.rfind(".app");
		if (found != std::string::npos)
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
		
		NSString *nsCmd = [NSString stringWithUTF8String:cmd.c_str()];
		NSMutableArray *arguments = [[NSMutableArray alloc] init];
		for (size_t i = 1; i < args->Size(); i++)
		{
			std::string arg = args->At(i)->ToString();
			[arguments addObject:[NSString stringWithUTF8String:arg.c_str()]];
		}
		
		[task setLaunchPath:nsCmd];
		[task setArguments:arguments];
		[task setCurrentDirectoryPath: [[NSFileManager defaultManager] currentDirectoryPath]];

		NSMutableDictionary *environment = [NSMutableDictionary dictionaryWithCapacity:100];
		SharedStringList properties = process->GetEnvironment()->GetPropertyNames();
		for (size_t i = 0; i < properties->size(); i++)
		{
			const char *key = properties->at(i)->c_str();
			[environment
				setValue:[NSString stringWithUTF8String:process->GetString(key).c_str()]
				forKey:[NSString stringWithUTF8String:key] ];
		}
		
		[task setEnvironment:environment];
		[task setStandardOutput: process->GetStdout()->GetPipe()];
		[task setStandardError: process->GetStderr()->GetPipe()];
		[task setStandardInput: process->GetStdin()->GetPipe()];

		[[NSNotificationCenter defaultCenter] addObserver:self 
			selector:@selector(terminated:) 
			name: NSTaskDidTerminateNotification 
			object: task];
	}
	return self;
}

-(void)dealloc
{
	if ([task isRunning])
	{
		[task terminate];
	}
	[task release];
	[super dealloc];
}

-(NSTask*)task
{
	return task;
}

-(void)terminated: (NSNotification *)aNotification
{
	Logger *logger = Logger::Get("OSXProcess");
	logger->Info("Process has terminated");

	[[NSNotificationCenter defaultCenter] removeObserver:self name:NSTaskDidTerminateNotification object: task];
	
	process->SetExitCode([task terminationStatus]);
	process->Exited();

	// close the streams after our onexit in case they want to read
	// one last time from the stream in the callback
	process->GetStdin()->Close();
	process->GetStdout()->Close();
	process->GetStderr()->Close();
}
@end

namespace ti
{
	OSXProcess::OSXProcess(SharedKList args, SharedKObject environment,
		AutoOutputPipe stdin, AutoInputPipe stdout, AutoInputPipe stderr) :
			Process(args, environment, stdin, stdout, stderr), currentProcessInfo(nil)
	{
		
		delegate = [[TiOSXProcess alloc] initWithProcess:this];
	}
	
	// Current process constructor
	OSXProcess::OSXProcess(NSProcessInfo *processInfo) : Process(), currentProcessInfo(processInfo)
	{
		environment = new StaticBoundObject();
		args = new StaticBoundList();

		for (id k in [processInfo environment])
		{
			NSString *key = (NSString*)k;
			NSString *val = (NSString*)[[processInfo environment] objectForKey:key];
			SetEnvironment([key UTF8String], [val UTF8String]);
		}
		
		for (id a in [processInfo arguments])
		{
			NSString *arg = (NSString*)a;
			args->Append(Value::NewString([arg UTF8String]));
		}
	}
	
	AutoPtr<OSXProcess> OSXProcess::currentProcess = new OSXProcess([NSProcessInfo processInfo]);
	
	/*static*/
	AutoProcess OSXProcess::GetCurrentProcess()
	{
		return currentProcess;
	}
	
	OSXProcess::~OSXProcess()
	{	
		Terminate();
		//[delegate release];
	}
	
	int OSXProcess::GetPID()
	{
		if (currentProcessInfo != nil)
		{
			return [currentProcessInfo processIdentifier];
		}
		return [[delegate task] processIdentifier];
	}
	
	void OSXProcess::Launch(bool async)
	{
		if (currentProcessInfo != nil)
		{
			return;
		}
		
		// start the process
		// TODO -- implement sync
		@try
		{
			std::ostringstream str;
			for (int i = 0; i < args->Size(); i++)
			{
				str << " \"" << args->At(i)->ToString() << "\" ";
			}
			Logger::Get("OSXProcess")->Debug("Launching: %s", str.str().c_str());
			
			[[delegate task] launch];
		}
		@catch(NSException *e)
		{
			throw ValueException::FromString([[e reason] UTF8String]);
		}
	}
	
	void OSXProcess::Terminate()
	{
		if (currentProcessInfo != nil)
		{
			return;
		}
		
		[[delegate task] terminate];
	}
	
	void OSXProcess::Kill()
	{
		SendSignal(SIGKILL);
	}
	
	void OSXProcess::SendSignal(int signal)
	{
		kill(GetPID(), signal);
	}
	
	void OSXProcess::Restart()
	{
		/*
		NSProcessInfo *p = [NSProcessInfo processInfo];
		NSString *path = [[NSBundle mainBundle] bundlePath];
		NSString *killArg1AndOpenArg2Script = [NSString stringWithFormat:@"kill -9 %d\n open \"%@\"",[p processIdentifier],path];
		NSArray *shArgs = [NSArray arrayWithObjects:@"-c", // -c tells sh to execute the next argument, passing it the remaining arguments.
			killArg1AndOpenArg2Script,nil];
		NSTask *restartTask = [NSTask launchedTaskWithLaunchPath:@"/bin/sh" arguments:shArgs];
		[restartTask waitUntilExit]; //wait for killArg1AndOpenArg2Script to finish*/
	}
	
	void OSXProcess::Restart(
		SharedKObject env, AutoOutputPipe stdin, AutoInputPipe stdout, AutoInputPipe stderr)
	{
		
	}
}
