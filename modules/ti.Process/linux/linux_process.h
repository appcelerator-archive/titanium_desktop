/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
 
#ifndef _LINUX_PROCESS_H_
#define _LINUX_PROCESS_H_

#include <sstream>
#include "linux_output_pipe.h"
#include "linux_input_pipe.h"
#include "../process.h"

namespace ti
{
	class LinuxProcess : public Process
	{
	public:
		LinuxProcess(SharedKList args, SharedKObject environment, AutoOutputPipe stdinPipe, AutoInputPipe stdoutPipe, AutoInputPipe stderrPipe);
		virtual ~LinuxProcess();

		static AutoPtr<LinuxProcess> GetCurrentProcess();
		
		AutoPtr<LinuxOutputPipe> GetStdin() { return stdinPipe.cast<LinuxOutputPipe>(); }
		AutoPtr<LinuxInputPipe> GetStdout() { return stdoutPipe.cast<LinuxInputPipe>(); }
		AutoPtr<LinuxInputPipe> GetStderr() { return stderrPipe.cast<LinuxInputPipe>(); }
		
		virtual int GetPID();
		virtual void Launch(bool async=true);
		virtual void Terminate();
		virtual void Kill();
		virtual void SendSignal(int signal);
		virtual void Restart();
		virtual void Restart(SharedKObject env, AutoOutputPipe stdinPipe, AutoInputPipe stdoutPipe, AutoInputPipe stderrPipe);
		virtual bool IsRunning();
		
	protected:
		// for current process
		LinuxProcess();
		
		void StartProcess();
		void ExitMonitor();
		
		Poco::Thread exitMonitorThread;
		Poco::RunnableAdapter<LinuxProcess>* exitMonitorAdapter;
		bool running, complete, current;
		int pid;
		int exitCode;
		
		Logger* logger;
		static AutoPtr<LinuxProcess> currentProcess;
	};
}

#endif
