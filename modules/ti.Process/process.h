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
#include "native_pipe.h"

namespace ti
{
	class Process;
	typedef AutoPtr<Process> AutoProcess;
	class Process : public KEventMethod
	{
	public:
		Process();
		virtual ~Process();
		static AutoProcess CreateProcess();
		virtual SharedKObject CloneEnvironment();
		virtual void LaunchAsync();
		virtual AutoBlob LaunchSync();
		virtual void ExitMonitorAsync();
		virtual void ExitMonitorSync();
		std::string ArgumentsToString();
		void SetOnRead(SharedKMethod method);
		void SetOnExit(SharedKMethod onExit);
		void Exited(bool async);
		void ExitCallback(const ValueList& args, SharedValue result);
		virtual SharedValue Call(const ValueList& args);
		static SharedKObject GetCurrentEnvironment();

		void SetStdin(AutoPipe stdinPipe);
		void SetStdout(AutoPipe stdoutPipe);
		void SetStderr(AutoPipe stderrPipe);
		inline bool IsRunning() { return running; }
		inline void SetPID(int pid) { this->pid = pid; }
		virtual inline void SetArguments(SharedKList args) { this->args = args; }
		inline void SetEnvironment(SharedKObject env) { this->environment = env; }
		inline int GetPID() { return this->pid; }
		inline AutoPipe GetStdin() { return this->stdinPipe; }
		inline AutoPipe GetStdout() { return this->stdoutPipe; }
		inline AutoPipe GetStderr() { return this->stderrPipe; }
		inline SharedKList GetArgs() { return this->args; };
		inline SharedKObject GetEnvironment() { return this->environment; }
		void SetEnvironment(const char *name, const char *value)
		{
			environment->SetString(name, value);
		}

		virtual void Terminate() = 0;
		virtual void Kill() = 0;
		virtual void SendSignal(int signal) = 0;
		virtual void ForkAndExec() = 0;
		virtual void MonitorAsync() = 0;
		virtual AutoBlob MonitorSync() = 0;
		virtual int Wait() = 0;
		virtual void RecreateNativePipes() = 0;
		virtual AutoPtr<NativePipe> GetNativeStdin() = 0;
		virtual AutoPtr<NativePipe> GetNativeStdout() = 0;
		virtual AutoPtr<NativePipe> GetNativeStderr() = 0;
		void AttachPipes(bool async);

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
		void _GetStdin(const ValueList& args, SharedValue result);
		void _GetStdout(const ValueList& args, SharedValue result);
		void _GetStderr(const ValueList& args, SharedValue result);
		void _IsRunning(const ValueList& args, SharedValue result);
		void _SetOnRead(const ValueList& args, SharedValue result);
		void _SetOnExit(const ValueList& args, SharedValue result);
		void _ToString(const ValueList& args, SharedValue result);
		void DetachPipes();

		AutoPipe stdoutPipe;
		AutoPipe stderrPipe;
		AutoPipe stdinPipe;
		SharedKObject environment;
		SharedKList args;
		int pid;
		SharedValue exitCode;
		SharedKMethod onRead;
		SharedKMethod onExit;
		Poco::RunnableAdapter<Process>* exitMonitorAdapter;
		Poco::Thread exitMonitorThread;
		SharedKMethod exitCallback;
		bool running;
	};
}

#endif
