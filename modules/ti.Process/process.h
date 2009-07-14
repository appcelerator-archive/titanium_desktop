/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _PROCESS_H_
#define _PROCESS_H_

#include <kroll/kroll.h>
#include <sstream>
#include "input_pipe.h"
#include "output_pipe.h"
#include "buffered_input_pipe.h"

namespace ti
{
	class Process;
	typedef AutoPtr<Process> AutoProcess;
	
	class Process : public AccessorBoundMethod
	{
	public:
		Process(SharedKList args, SharedKObject environment,
			 AutoOutputPipe stdinPipe, AutoInputPipe stdoutPipe, AutoInputPipe stderrPipe);
		virtual ~Process();
		static AutoProcess GetCurrentProcess();
		static AutoProcess CreateProcess(SharedKList args, SharedKObject environment, 
			AutoOutputPipe stdinPipe, AutoInputPipe stdoutPipe, AutoInputPipe stderrPipe);
		
		virtual int GetPID() = 0;
		void SetExitCode(int exitCode) { this->exitCode = exitCode; }
		virtual int GetExitCode() { return exitCode; }
		virtual SharedKList GetArgs() { return args; }
		virtual SharedKObject GetEnvironment() { return environment; }
		virtual void SetEnvironment(const char *name, const char *value) { environment->SetString(name, value); }
		virtual SharedKObject CloneEnvironment();
		virtual void Launch(bool async=true) = 0;
		virtual void Terminate() = 0;
		virtual void Kill() = 0;
		virtual void SendSignal(int signal) = 0;
		virtual void Restart() = 0;
		virtual void Restart(SharedKObject env,
			 AutoOutputPipe stdinPipe, AutoInputPipe stdoutPipe, AutoInputPipe stderrPipe) = 0;
		virtual bool IsRunning() = 0;
		
		std::string ArgumentsToString();
		void SetOnRead(SharedKMethod method);
		void Exited();
		
	protected:
		// empty constructor for creating the current process in platform implementations
		Process();
		void InitBindings();
		
		void _GetPID(const ValueList& args, SharedValue result);
		void _GetExitCode(const ValueList& args, SharedValue result);
		void _GetArguments(const ValueList& args, SharedValue result);
		void _GetEnvironment(const ValueList& args, SharedValue result);
		void _SetEnvironment(const ValueList& args, SharedValue result);
		void _CloneEnvironment(const ValueList& args, SharedValue result);
		void _Launch(const ValueList& args, SharedValue result);
		void _Terminate(const ValueList& args, SharedValue result);
		void _Kill(const ValueList& args, SharedValue result);
		void _SendSignal(const ValueList& args, SharedValue result);
		void _Restart(const ValueList& args, SharedValue result);
		void _GetStdin(const ValueList& args, SharedValue result);
		void _GetStdout(const ValueList& args, SharedValue result);
		void _GetStderr(const ValueList& args, SharedValue result);
		void _IsRunning(const ValueList& args, SharedValue result);
		
		void _SetOnRead(const ValueList& args, SharedValue result);
		void _SetOnExit(const ValueList& args, SharedValue result);
		void _ToString(const ValueList& args, SharedValue result);
		
		// non-exposed binding methods
		BufferedInputPipe buffer;
		SharedKMethod bufferedRead;
		void BufferedRead(const ValueList& args, SharedValue result);
		void Call(const ValueList& args, SharedValue result);
		
		AutoInputPipe stdoutPipe, stderrPipe;
		AutoOutputPipe stdinPipe;
		SharedKObject environment;
		SharedKList args;
		int exitCode;
		SharedKMethod* onExit;
	};
}

#endif
