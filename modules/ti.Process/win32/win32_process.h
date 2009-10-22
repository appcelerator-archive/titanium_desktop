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

namespace ti
{
	class Win32Process : public Process
	{
	public:
		Win32Process();
		virtual ~Win32Process();

		inline virtual AutoPtr<NativePipe> GetNativeStdin() { return nativeIn; }
		inline virtual AutoPtr<NativePipe> GetNativeStdout() { return nativeOut; }
		inline virtual AutoPtr<NativePipe> GetNativeStderr() { return nativeErr; }

		virtual int GetPID();
		virtual void Terminate();
		virtual void Kill();
		virtual void SendSignal(int signal);
		virtual void ForkAndExec();
		virtual void MonitorAsync();
		virtual BlobRef MonitorSync();
		virtual int Wait();
		virtual std::string ArgumentsToString();
		void ReadCallback(const ValueList& args, KValueRef result);
		virtual void RecreateNativePipes();
		
	protected:
		std::string ArgListToString(KListRef argList);
		
		Poco::Thread exitMonitorThread;
		Poco::RunnableAdapter<Win32Process>* exitMonitorAdapter;
		AutoPtr<Win32Pipe> nativeIn, nativeOut, nativeErr;
		Poco::Mutex mutex;
		
		Poco::Mutex processOutputMutex;
		std::vector<BlobRef> processOutput;
		
		int pid;
		HANDLE process;
		
		Logger* logger;
	};
}

#endif
