/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
 
#ifndef _WIN32_PROCESS_H_
#define _WIN32_PROCESS_H_

#include <sstream>
#include "win32_pipe.h"
#include "../process.h"

#undef stdin
#undef stdout
#undef stderr

namespace ti
{
	class Win32Process : public Process
	{
	public:
		Win32Process(SharedKList args, SharedKObject environment, AutoPipe stdinPipe, AutoPipe stdoutPipe, AutoPipe stderrPipe);
		virtual ~Win32Process();

		static AutoPtr<Win32Process> GetCurrentProcess();
		
		AutoPtr<Win32Pipe> GetStdin() { return nativeIn; }
		AutoPtr<Win32Pipe> GetStdout() { return nativeOut; }
		AutoPtr<Win32Pipe> GetStderr() { return nativeErr; }
		
		virtual int GetPID();
		virtual void Launch(bool async=true);
		virtual void Terminate();
		virtual void Kill();
		virtual void SendSignal(int signal);
		virtual bool IsRunning();
		
	protected:
		// for current process
		Win32Process();
		std::string ArgListToString(SharedKList argList);
		
		void StartProcess();
		void ExitMonitor();
		
		Poco::Thread exitMonitorThread;
		Poco::RunnableAdapter<Win32Process>* exitMonitorAdapter;
		AutoPtr<Win32Pipe> nativeIn, nativeOut, nativeErr;
		
		bool running, complete, current;
		int pid;
		HANDLE process;
		
		Logger* logger;
		static AutoPtr<Win32Process> currentProcess;
	};
}

#endif
