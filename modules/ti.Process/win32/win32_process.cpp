/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
 
#include "win32_process.h"
#include <kroll/base.h>
#include <windows.h>
#include "../process_binding.h"

namespace ti
{
	Win32Process::Win32Process(ProcessBinding* parent, std::string& command, std::vector<std::string>& args) :
		StaticBoundObject("Process"),
		running(false),
		complete(false),
		pid(-1),
		exitCode(-1),
		logger(Logger::Get("Process.Win32Process")),
		parent(parent)
	{
		try
		{
			this->arguments = args;
			this->command = command;
		}
		catch (std::exception &e)	
		{
			throw ValueException::FromString(e.what());
		}

		this->SetString("command", command);
		this->Set("pid",Value::NewInt(-1));
		this->SetBool("running", false);

		this->err = new Win32Pipe();
		this->shared_error = new SharedKObject(this->err);
		this->SetObject("err", *shared_error);

		this->out = new Win32Pipe();
		this->shared_output = new SharedKObject(this->out);
		this->SetObject("out", *shared_output);

		this->in = new Win32Pipe();
		this->shared_input = new SharedKObject(this->in);
		this->SetObject("in", *shared_input);

		this->SetMethod("terminate", &Win32Process::Terminate);
		this->SetNull("exitCode");
		this->SetNull("onread");
		this->SetNull("onexit");

		// setup threads which can read output and also monitor the exit
		this->monitorAdapter = new Poco::RunnableAdapter<Win32Process>(*this, &Win32Process::Monitor);
		this->exitMonitorThread.start(*monitorAdapter);
	}
	
	void Win32Process::StartThreads()
	{
		logger->Debug("Starting output handler threads...");

		if (!stdOutThread.isRunning())
		{
			this->stdOutAdapter = new Poco::RunnableAdapter<Win32Process>(*this, &Win32Process::ReadStdOut);
			stdOutThread.start(*stdOutAdapter);
		}

		if (!stdErrorThread.isRunning())
		{
			this->stdErrorAdapter = new Poco::RunnableAdapter<Win32Process>(*this, &Win32Process::ReadStdErr);
			stdErrorThread.start(*stdErrorAdapter);
		}
	}
	
	void Win32Process::Monitor()
	{
		STARTUPINFO startupInfo;
		startupInfo.cb          = sizeof(STARTUPINFO);
		startupInfo.lpReserved  = NULL;
		startupInfo.lpDesktop   = NULL;
		startupInfo.lpTitle     = NULL;
		startupInfo.dwFlags     = STARTF_FORCEOFFFEEDBACK | STARTF_USESTDHANDLES;
		startupInfo.cbReserved2 = 0;
		startupInfo.lpReserved2 = NULL;
		
		HANDLE hProc = GetCurrentProcess();
		DuplicateHandle(hProc, in->GetReadHandle(), hProc, &startupInfo.hStdInput, 0, TRUE, DUPLICATE_SAME_ACCESS);
		CloseHandle(in->GetReadHandle());
		DuplicateHandle(hProc, out->GetWriteHandle(), hProc, &startupInfo.hStdOutput, 0, TRUE, DUPLICATE_SAME_ACCESS);
		CloseHandle(out->GetWriteHandle());
		DuplicateHandle(hProc, err->GetWriteHandle(), hProc, &startupInfo.hStdError, 0, TRUE, DUPLICATE_SAME_ACCESS);
		CloseHandle(err->GetWriteHandle());
		
		std::string commandLine = command;
		for (std::vector<std::string>::const_iterator it = arguments.begin(); it != arguments.end(); ++it)
		{
			commandLine.append(" ");
			commandLine.append(*it);
		}
		
		logger->Debug("Launching: %s", commandLine.c_str());
	
		PROCESS_INFORMATION processInfo;
		BOOL rc = CreateProcessA(NULL,
			(char*)commandLine.c_str(),
			NULL,
			NULL,
			TRUE,
			0,
			NULL,
			NULL,
			&startupInfo,
			&processInfo);
		
		CloseHandle(startupInfo.hStdInput);
		CloseHandle(startupInfo.hStdOutput);
		CloseHandle(startupInfo.hStdError);
		
		if (!rc) {
			std::string message = "Error launching: " + commandLine;
			logger->Error(message);
			throw ValueException::FromString(message);
		}
		else {
			StartThreads();
			CloseHandle(processInfo.hThread);
			this->pid = processInfo.dwProcessId;
			this->process = processInfo.hProcess;
			this->Set("pid", Value::NewInt(this->pid));
			this->running = true;
			this->Set("running", Value::NewBool(true));
		}
		
		while (true) {
			DWORD rc = WaitForSingleObject(this->process, 250);
			if (rc == WAIT_OBJECT_0) {
				break;
			}
			if (rc == WAIT_ABANDONED) {
				break;
			}
			else continue;
		}
		
		logger->Debug("finally exited it looks like");

		DWORD exitCode;
		if (GetExitCodeProcess(this->process, &exitCode) == 0) {
			throw ValueException::FromString("Cannot get exit code for process");
		}
		this->exitCode = exitCode;
		this->Set("exitCode", Value::NewInt(this->exitCode));
		
		this->parent->Terminated(this);
		this->Set("running", Value::NewBool(false));
		
		logger->Debug("Invoking onexit");
		InvokeOnExit();
	}
	
	void Win32Process::ReadStdOut()
	{
		char buffer[1024];
		int length = 1023;
		int bytesRead = out->Read(buffer, length);
		while (bytesRead > 0) {
			buffer[bytesRead] = '\0';
			InvokeOnRead(buffer, false);
			bytesRead = out->Read(buffer, length);
		}
	}
	
	void Win32Process::ReadStdErr()
	{
		char buffer[1024];
		int length = 1023;
		int bytesRead = err->Read(buffer, length);
		while (bytesRead > 0) {
			buffer[bytesRead] = '\0';
			InvokeOnRead(buffer, false);
			bytesRead = err->Read(buffer, length);
		}
	}

	Win32Process::~Win32Process()
	{
		try {
			Terminate();
		} catch (ValueException &ve) {
			logger->Error(ve.what());
		}
		
		if (this->exitMonitorThread.isRunning())
		{
			try
			{
				this->exitMonitorThread.join();
			}
			catch (Poco::Exception& e)
			{
				logger->Error(
					"Exception while try to join with exit monitor thread: %s",
					e.displayText().c_str());
			}
		}

		if (this->stdOutThread.isRunning())
		{
			try
			{
				this->stdOutThread.join();
			}
			catch (Poco::Exception& e)
			{
				logger->Error(
					"Exception while try to join with stdout thread: %s",
					e.displayText().c_str());
			}
		}

		if (this->stdErrorThread.isRunning())
		{
			try
			{
				this->stdErrorThread.join();
			}
			catch (Poco::Exception& e)
			{
				logger->Error(
					"Exception while try to join with stderr thread: %s",
					e.displayText().c_str());
			}
		}

		delete monitorAdapter;
		delete stdOutAdapter;
		delete stdErrorAdapter;
		delete shared_output;
		delete shared_input;
		delete shared_error;
	}
	
	void Win32Process::Set(const char *name, SharedValue value)
	{
		// We need to check the previous value of certain incomming values
		// *before* we actually do the Set(...) on this object.
		//Poco::ScopedLock<Poco::Mutex> lock(outputBufferMutex);
		bool flushOnRead = 
			(!strcmp("onread", name)) && (!this->Get("onread")->IsMethod());
		bool flushOnExit = 
			(!strcmp("onexit", name)) && (!this->Get("onexit")->IsMethod());


		StaticBoundObject::Set(name, value);

		if (flushOnRead)
		{
			// If we had no previous onread callback flush our output
			// buffers, so that onread will be called even when it is
			// attached after a process finishes executing.
			this->InvokeOnRead(outBuffer, false);
			this->InvokeOnRead(errBuffer, true);
		}

		// this->complete is the signal that monitor thread has already
		// attempted to call the onexit callback. If it's false, the monitor
		// thread will take care of calling it.
		if (flushOnExit && this->complete)
		{
			this->InvokeOnExit();
		}
	}

	void Win32Process::Terminate(const ValueList& args, SharedValue result)
	{
		Terminate();
	}
	
	void Win32Process::Terminate()
	{
		if (running) {
			running = false;
			HANDLE hProc = OpenProcess(PROCESS_TERMINATE, FALSE, this->pid);
		if (hProc)
		{
			if (TerminateProcess(hProc, 0) == 0)
			{
				CloseHandle(hProc);
				// not sure if this is the right thing to do
				throw ValueException::FromString("cannot kill process");
			}
			CloseHandle(hProc);
		}
		else
		{
			switch (GetLastError())
			{
			case ERROR_ACCESS_DENIED:
				throw ValueException::FromString("cannot kill process");
			case ERROR_NOT_FOUND:
				throw ValueException::FromString("cannot kill process");
			default:
				throw ValueException::FromString("cannot kill process");
			}
		}
		
			this->Set("running", Value::NewBool(false));
			this->parent->Terminated(this);
		}
	}
	
	void Win32Process::InvokeOnExit()
	{
		SharedValue sv = this->Get("onexit");
		if (!sv->IsMethod())
		{
			return;
		}
		
		ValueList args(Value::NewInt(this->exitCode));
		SharedKMethod callback = sv->ToMethod();
		this->parent->GetHost()->InvokeMethodOnMainThread(
			callback, args, false);
	}
	
	void Win32Process::InvokeOnRead(char *buffer, bool isErr)
	{
		if (isErr) {
			errBuffer << buffer;
		}
		else {
			outBuffer << buffer;
		}
		InvokeOnRead(isErr ? errBuffer : outBuffer, isErr);
	}
	
	void Win32Process::InvokeOnRead(std::ostringstream& buffer, bool isErr)
	{
		SharedValue sv = this->Get("onread");
		if (!sv->IsMethod())
		{
			return;
		}
		
		if (!buffer.str().empty())
		{
			ValueList args(
				Value::NewString(buffer.str()),
				Value::NewBool(isErr));
			SharedKMethod callback = sv->ToMethod();
			this->parent->GetHost()->InvokeMethodOnMainThread(
				callback, args, false);
				
			buffer.str("");
		}
	}
}
