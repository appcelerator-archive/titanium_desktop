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
		PosixProcess();
		virtual ~PosixProcess();
		virtual void Terminate();
		virtual void Kill();
		virtual void SendSignal(int signal);
		static AutoPtr<PosixProcess> GetCurrentProcess();

		virtual void ForkAndExec();
		virtual void MonitorAsync();
		virtual std::string MonitorSync();
		virtual int Wait();
		virtual void SetArguments(SharedKList args);
		void ReadCallback(const ValueList& args, SharedValue result);

	protected:
		Logger* logger;
		int pid;
		AutoPtr<PosixPipe> nativeIn, nativeOut, nativeErr;

		// For synchronous process execution store
		// process output as a vector of blobs for speed.
		std::vector<AutoBlob> processOutput;
		void StartProcess();
	};
}

#endif
