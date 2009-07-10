/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
 
#ifndef _WIN32_PROCESS_H_
#define _WIN32_PROCESS_H_

#include <sstream>
#include "win32_output_pipe.h"
#include "win32_input_pipe.h"
#include "../process.h"

#undef stdin
#undef stdout
#undef stderr

namespace ti
{
	class Win32Process : public Process
	{
	public:
		Win32Process(SharedKList args, SharedKObject environment, SharedOutputPipe stdin, SharedInputPipe stdout, SharedInputPipe stderr);
		virtual ~Win32Process();

		static SharedPtr<Win32Process> GetCurrentProcess();
		
		SharedPtr<Win32OutputPipe> GetStdin() { return stdin.cast<Win32OutputPipe>(); }
		SharedPtr<Win32InputPipe> GetStdout() { return stdout.cast<Win32InputPipe>(); }
		SharedPtr<Win32InputPipe> GetStderr() { return stderr.cast<Win32InputPipe>(); }
		
		virtual int GetPID();
		virtual void Launch(bool async=true);
		virtual void Terminate();
		virtual void Kill();
		virtual void SendSignal(int signal);
		virtual void Restart();
		virtual void Restart(SharedKObject env, SharedOutputPipe stdin, SharedInputPipe stdout, SharedInputPipe stderr);
		
	protected:
		// for current process
		Win32Process();
		std::string ArgListToString(SharedKList argList);
		
		void StartProcess();
		void ExitMonitor();
		
		Poco::Thread exitMonitorThread;
		Poco::RunnableAdapter<Win32Process>* exitMonitorAdapter;
		bool running, complete, current;
		int pid;
		HANDLE process;
		int exitCode;
		
		std::ostringstream outBuffer, errBuffer;
		SharedKObject *shared_input, *shared_output, *shared_error;
		Logger* logger;
		static SharedPtr<Win32Process> currentProcess;
	};
}

#endif