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
		virtual KObjectRef CloneEnvironment();
		virtual void LaunchAsync();
		virtual BlobRef LaunchSync();
		virtual void ExitMonitorAsync();
		virtual void ExitMonitorSync();
		std::string ArgumentsToString();
		void SetOnRead(KMethodRef method);
		void SetOnExit(KMethodRef onExit);
		void Exited(bool async);
		void ExitCallback(const ValueList& args, KValueRef result);
		virtual KValueRef Call(const ValueList& args);
		static KObjectRef GetCurrentEnvironment();

		void SetStdin(AutoPipe stdinPipe);
		void SetStdout(AutoPipe stdoutPipe);
		void SetStderr(AutoPipe stderrPipe);
		inline bool IsRunning() { return running; }
		inline void SetPID(int pid) { this->pid = pid; }
		virtual inline void SetArguments(KListRef args) { this->args = args; }
		inline void SetEnvironment(KObjectRef env) { this->environment = env; }
		inline int GetPID() { return this->pid; }
		inline AutoPipe GetStdin() { return this->stdinPipe; }
		inline AutoPipe GetStdout() { return this->stdoutPipe; }
		inline AutoPipe GetStderr() { return this->stderrPipe; }
		inline KListRef GetArgs() { return this->args; };
		inline KObjectRef GetEnvironment() { return this->environment; }
		void SetEnvironment(const char *name, const char *value)
		{
			environment->SetString(name, value);
		}

		virtual void Terminate() = 0;
		virtual void Kill() = 0;
		virtual void SendSignal(int signal) = 0;
		virtual void ForkAndExec() = 0;
		virtual void MonitorAsync() = 0;
		virtual BlobRef MonitorSync() = 0;
		virtual int Wait() = 0;
		virtual void RecreateNativePipes() = 0;
		virtual AutoPtr<NativePipe> GetNativeStdin() = 0;
		virtual AutoPtr<NativePipe> GetNativeStdout() = 0;
		virtual AutoPtr<NativePipe> GetNativeStderr() = 0;
		void AttachPipes(bool async);

	protected:
		void _GetPID(const ValueList& args, KValueRef result);
		void _GetExitCode(const ValueList& args, KValueRef result);
		void _GetArguments(const ValueList& args, KValueRef result);
		void _GetEnvironment(const ValueList& args, KValueRef result);
		void _SetEnvironment(const ValueList& args, KValueRef result);
		void _CloneEnvironment(const ValueList& args, KValueRef result);
		void _Launch(const ValueList& args, KValueRef result);
		void _Terminate(const ValueList& args, KValueRef result);
		void _Kill(const ValueList& args, KValueRef result);
		void _SendSignal(const ValueList& args, KValueRef result);
		void _GetStdin(const ValueList& args, KValueRef result);
		void _GetStdout(const ValueList& args, KValueRef result);
		void _GetStderr(const ValueList& args, KValueRef result);
		void _IsRunning(const ValueList& args, KValueRef result);
		void _SetOnRead(const ValueList& args, KValueRef result);
		void _SetOnExit(const ValueList& args, KValueRef result);
		void _ToString(const ValueList& args, KValueRef result);
		void DetachPipes();

		AutoPipe stdoutPipe;
		AutoPipe stderrPipe;
		AutoPipe stdinPipe;
		KObjectRef environment;
		KListRef args;
		int pid;
		KValueRef exitCode;
		KMethodRef onRead;
		KMethodRef onExit;
		Poco::RunnableAdapter<Process>* exitMonitorAdapter;
		Poco::Thread exitMonitorThread;
		KMethodRef exitCallback;
		bool running;
	};
}

#endif
