/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _OSX_PROCESS_H_
#define _OSX_PROCESS_H_
#include <kroll/kroll.h>
#include <Cocoa/Cocoa.h>
#include "../process_binding.h"
#include "../process.h"
#include "osx_input_pipe.h"
#include "osx_output_pipe.h"
#include <signal.h>

namespace ti { class OSXProcess; }

@interface TiOSXProcess : NSObject {
	NSTask *task;
	ti::OSXProcess *process;
}

-(id)initWithProcess:(ti::OSXProcess*)p;
-(NSTask*)task;
-(void)terminated:(NSNotification *)aNotification;
-(void)processExited;
@end

namespace ti
{
	class OSXProcess : public Process
	{
	public:
		static AutoProcess GetCurrentProcess();
		OSXProcess(SharedKList args, SharedKObject environment, AutoOutputPipe stdinPipe, AutoInputPipe stdoutPipe, AutoInputPipe stderrPipe);
		virtual ~OSXProcess();
		
		virtual int GetPID();
		virtual void Launch(bool async=true);
		virtual void Terminate();
		virtual void Kill();
		virtual void SendSignal(int signal);
		virtual bool IsRunning();
		virtual void Exited();
		
		AutoPtr<OSXOutputPipe> GetStdin() { return stdinPipe.cast<OSXOutputPipe>(); }
		AutoPtr<OSXInputPipe> GetStdout() { return stdoutPipe.cast<OSXInputPipe>(); }
		AutoPtr<OSXInputPipe> GetStderr() { return stderrPipe.cast<OSXInputPipe>(); }
	protected:
		TiOSXProcess *delegate;
		NSProcessInfo *currentProcessInfo;
		OSXProcess::OSXProcess(NSProcessInfo *processInfo);
		Logger *logger;
		
		static AutoPtr<OSXProcess> currentProcess;
	};
}

#endif
