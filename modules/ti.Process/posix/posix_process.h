/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
 
#ifndef _POSIX_PROCESS_H_
#define _POSIX_PROCESS_H_

#include <sstream>
#include <Poco/Thread.h>
#include "posix_pipe.h"
#include "../process.h"

namespace ti
{
	class PosixProcess;
	class PosixProcess : public Process
	{
	public:
		PosixProcess(SharedKList args, SharedKObject environment, 
			AutoPipe stdinPipe, AutoPipe stdoutPipe, AutoPipe stderrPipe);
		virtual ~PosixProcess();
		virtual int GetPID();
		virtual void LaunchAsync();
		virtual std::string LaunchSync();
		virtual void Terminate();
		virtual void Kill();
		virtual void SendSignal(int signal);
		virtual bool IsRunning();
		static AutoPtr<PosixProcess> GetCurrentProcess();

		void ForkAndExec();
		void MonitorAsync();
		std::string MonitorSync();
		void ExitMonitor();
		void ExitCallback(const ValueList& args, SharedValue result);
		void ReadCallback(const ValueList& args, SharedValue result);
	protected:
		Logger* logger;
		static AutoPtr<PosixProcess> currentProcess;
		int pid, exitCode;
		AutoPtr<PosixPipe> nativeIn, nativeOut, nativeErr;

		// For synchronous process execution store
		// process output as a vector of blobs for speed.
		std::vector<AutoBlob> processOutput;
		Poco::RunnableAdapter<PosixProcess>* exitMonitorAdapter;
		Poco::Thread exitMonitorThread;
		SharedKMethod exitCallback;
		
		PosixProcess();
		void StartProcess();
	};
}

#endif
