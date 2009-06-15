/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
 
#ifndef _WIN32_PROCESS_H_
#define _WIN32_PROCESS_H_

#include "win32_pipe.h"
#include <sstream>

namespace ti
{
	class ProcessBinding;
	
	class Win32Process : public StaticBoundObject
	{
	public:
		Win32Process(ProcessBinding* parent, std::string& command, std::vector<std::string>& args);

	protected:
		virtual ~Win32Process();
		virtual void Set(const char *name, SharedValue value);

	private:
		ProcessBinding *parent;
		Poco::Thread exitMonitorThread;
		Poco::Thread stdOutThread;
		Poco::Thread stdErrorThread;
		Poco::RunnableAdapter<Win32Process>* monitorAdapter;
		Poco::RunnableAdapter<Win32Process>* stdOutAdapter;
		Poco::RunnableAdapter<Win32Process>* stdErrorAdapter;
		bool running;
		bool complete;
		int pid;
		HANDLE process;
		int exitCode;
		std::vector<std::string> arguments;
		std::string command;
		Win32Pipe *in, *out, *err;
		std::ostringstream outBuffer, errBuffer;
		SharedKObject *shared_input, *shared_output, *shared_error;
		Logger* logger;

		void Terminate(const ValueList& args, SharedValue result);
		void Terminate();
		void StartProcess();
		void StartThreads();
		
		void InvokeOnExit();
		void InvokeOnRead(char *buffer, bool isStdError);
		void InvokeOnRead(std::ostringstream& buffer, bool isErr);
		
		void Monitor();
		void ReadStdOut();
		void ReadStdErr();
	};
}

#endif