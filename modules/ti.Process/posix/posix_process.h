/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
 
#ifndef _POSIX_PROCESS_H_
#define _POSIX_PROCESS_H_

#include <sstream>
#include "posix_pipe.h"
#include "../process.h"

namespace ti
{
	class PosixProcess : public Process
	{
	public:
		PosixProcess(SharedKList args, SharedKObject environment, AutoPipe stdinPipe, AutoPipe stdoutPipe, AutoPipe stderrPipe);
		virtual ~PosixProcess();

		static AutoPtr<PosixProcess> GetCurrentProcess();
		
		AutoPtr<PosixPipe> GetStdin() { return stdinPipe.cast<PosixPipe>(); }
		AutoPtr<PosixPipe> GetStdout() { return stdoutPipe.cast<PosixPipe>(); }
		AutoPtr<PosixPipe> GetStderr() { return stderrPipe.cast<PosixPipe>(); }
		
		virtual int GetPID();
		virtual void Launch(bool async=true);
		virtual void Terminate();
		virtual void Kill();
		virtual void SendSignal(int signal);
		virtual bool IsRunning();
		
	protected:
		// for current process
		PosixProcess();
		
		void StartProcess();
		void ExitMonitor();
		
		Poco::Thread exitMonitorThread;
		Poco::RunnableAdapter<PosixProcess>* exitMonitorAdapter;
		bool running, complete, current, runningSync;
		int pid;
		
		Logger* logger;
		static AutoPtr<PosixProcess> currentProcess;
	};
}

#endif
