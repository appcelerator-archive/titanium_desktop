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
		virtual BytesRef MonitorSync();
		virtual int Wait();
		virtual void RecreateNativePipes();
		virtual void SetArguments(KListRef args);
		void ReadCallback(const ValueList& args, KValueRef result);
		inline virtual AutoPtr<NativePipe> GetNativeStdin() { return nativeIn; }
		inline virtual AutoPtr<NativePipe> GetNativeStdout() { return nativeOut; }
		inline virtual AutoPtr<NativePipe> GetNativeStderr() { return nativeErr; }

	protected:
		Logger* logger;
		int pid;
		AutoPtr<PosixPipe> nativeIn;
		AutoPtr<PosixPipe> nativeOut;
		AutoPtr<PosixPipe> nativeErr;

		// For synchronous process execution store
		// process output as a vector of Bytes for speed.
		Poco::Mutex processOutputMutex;
		std::vector<BytesRef> processOutput;
		void StartProcess();
	};
}

#endif
