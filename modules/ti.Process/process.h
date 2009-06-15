/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _PROCESS_H_
#define _PROCESS_H_

#include <kroll/kroll.h>
#include <Poco/Process.h>
#include <Poco/Pipe.h>
#include <Poco/Thread.h>
#include <Poco/RunnableAdapter.h>
#include <Poco/Mutex.h>
#include <Poco/Condition.h>
#include <sstream>
#include "pipe.h"
#include "process_binding.h"

namespace ti
{
	class Process : public StaticBoundObject
	{
	public:
		Process(ProcessBinding* parent, std::string& command, std::vector<std::string>& args);

	protected:
		virtual ~Process();
		virtual void Set(const char *name, SharedValue value);

	private:
		ProcessBinding *parent;
		Poco::Thread exitMonitorThread;
		Poco::Thread stdOutThread;
		Poco::Thread stdErrorThread;
		Poco::RunnableAdapter<Process>* monitorAdapter;
		Poco::RunnableAdapter<Process>* stdOutAdapter;
		Poco::RunnableAdapter<Process>* stdErrorAdapter;
		bool running;
		bool complete;
		int pid;
		int exitCode;
		std::vector<std::string> arguments;
		std::string command;
		Pipe *in;
		Pipe *out;
		Pipe *err;
		Poco::Pipe *errp;
		Poco::Pipe *outp;
		Poco::Pipe *inp;
		SharedKObject *shared_input;
		SharedKObject *shared_output;
		SharedKObject *shared_error;
		std::ostringstream stdOutBuffer;
		std::ostringstream stdErrorBuffer;
		Logger* logger;
		Poco::Mutex outputBufferMutex;

		void Terminate(const ValueList& args, SharedValue result);
		void Terminate();
		void Monitor();
		void ReadStdOut();
		void ReadStdError();
		void StartReadThreads();
		void InvokeOnExitCallback();
		void InvokeOnReadCallback(bool isStdError);
	};
}

#endif
