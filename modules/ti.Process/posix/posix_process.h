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
	class PosixProcess;
	class NativePosixProcess;

	class NativePosixProcess
	{
		NativePosixProcess(PosixProcess* process);
		PosixProcess* process;
		int exitStatus;
		int pid;
		AutoPtr<PosixPipe> stdinPipe;
		AutoPtr<PosixPipe> stdoutPipe;
		AutoPtr<PosixPipe> stderrPipe;

		// For synchronous process execution store
		// process output as a vector of blobs for speed.
		std::vector<AutoBlob> processOutput;
		Poco::Thread exitMonitorThread;
		SharedKMethod exitCallback;

		void MonitorAsynchronously();
		void MonitorSynchronously();
		void ExitMonitor();
		void ExitCallback(const ValueList& args, SharedValue result);
		void ReadCallback(const ValueList& args, SharedValue result);
		static NativePosixProcess* Create(PosixProcess* process);
	};

	class PosixProcess : public Process
	{
	public:
		PosixProcess(SharedKList args, SharedKObject environment, 
			AutoPipe stdinPipe, AutoPipe stdoutPipe, AutoPipe stderrPipe);
		virtual ~PosixProcess();
		virtual int GetPID();
		virtual void Launch(bool async=true);
		virtual void Terminate();
		virtual void Kill();
		virtual void SendSignal(int signal);
		virtual bool IsRunning();
		static AutoPtr<PosixProcess> GetCurrentProcess();

	protected:
		Poco::Mutex nativeProcessesMutex;
		std::vector<NativePosixProcess*> nativeProcesses;
		Poco::RunnableAdapter<PosixProcess>* exitMonitorAdapter;
		Logger* logger;
		static AutoPtr<PosixProcess> currentProcess;
		int pid;

		PosixProcess();
		void StartProcess();
		void ExitMonitor();
	};
}

#endif
