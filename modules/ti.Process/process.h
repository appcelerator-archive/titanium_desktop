/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _PROCESS_H_
#define _PROCESS_H_

#include <kroll/kroll.h>
#include <sstream>
#include "pipe.h"

namespace ti
{
	class Process;
	typedef AutoPtr<Process> AutoProcess;
	
	class Process : public AccessorBoundMethod
	{
	public:
		Process();
		virtual ~Process();
		static AutoProcess GetCurrentProcess();
		static AutoProcess CreateProcess();

		virtual int GetPID() = 0;
		virtual SharedKObject CloneEnvironment();
		virtual void LaunchAsync() = 0;
		virtual std::string LaunchSync() = 0;
		virtual void Terminate() = 0;
		virtual void Kill() = 0;
		virtual void SendSignal(int signal) = 0;
		virtual void Restart();
		virtual void Restart(SharedKObject env, AutoPipe stdinPipe, AutoPipe stdoutPipe, AutoPipe stderrPipe);
		virtual bool IsRunning() = 0;
		std::string ArgumentsToString();
		void SetOnRead(SharedKMethod method);
		void Exited();

		inline void SetStdin(AutoPipe stdinPipe) { this->stdinPipe = stdinPipe; }
		inline void SetStdout(AutoPipe stdoutPipe) { this->stdoutPipe = stdoutPipe; }
		inline void SetStderr(AutoPipe stderrPipe) { this->stderrPipe = stderrPipe; }
		inline void SetArguments(SharedKList args) { this->args = args; }
		inline void SetEnvironment(SharedKObject env) { this->environment = env; }
		inline void SetExitCode(int exitCode) { this->exitCode = exitCode; }
		inline AutoPipe GetStdin() { return this->stdinPipe; }
		inline AutoPipe GetStdout() { return this->stdoutPipe; }
		inline AutoPipe GetStderr() { return this->stderrPipe; }
		inline SharedKList GetArgs() { return this->args; };
		inline SharedKObject GetEnvironment() { return this->environment; }
		inline int GetExitCode() { return exitCode; }

		void SetEnvironment(const char *name, const char *value)
		{
			environment->SetString(name, value);
		}


	protected:
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
		void Call(const ValueList& args, SharedValue result);
		
		AutoPipe stdoutPipe, stderrPipe;
		AutoPipe stdinPipe;
		SharedKObject environment;
		SharedKList args;
		int exitCode;
		SharedKMethod* onExit;
	};
}

#endif
