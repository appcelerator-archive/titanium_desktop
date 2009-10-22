/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
 
#include "win32_process.h"
#include "win32_pipe.h"
#include <signal.h>

namespace ti
{
	Win32Process::Win32Process() :
		logger(Logger::Get("Process.Win32Process")),
		nativeIn(new Win32Pipe(false)),
		nativeOut(new Win32Pipe(true)),
		nativeErr(new Win32Pipe(true))
	{
		stdinPipe->Attach(this->GetNativeStdin());
	}
	
	Win32Process::~Win32Process()
	{
	}

	void Win32Process::RecreateNativePipes()
	{
		this->nativeIn = new Win32Pipe(false);
		this->nativeOut = new Win32Pipe(true);
		this->nativeErr = new Win32Pipe(true);
		stdinPipe->Attach(this->GetNativeStdin());
	}
	
	/*
		Inspired by python's os.list2cmdline, ported to C++ by Marshall Culpepper
		
		Translate a sequence of arguments into a command line
		string, using the same rules as the MS C runtime:

		1) Arguments are delimited by white space, which is either a
		   space or a tab.

		2) A string surrounded by double quotation marks is
		   interpreted as a single argument, regardless of white space
		   contained within.  A quoted string can be embedded in an
		   argument.

		3) A double quotation mark preceded by a backslash is
		   interpreted as a literal double quotation mark.

		4) Backslashes are interpreted literally, unless they
		   immediately precede a double quotation mark.

		5) If backslashes immediately precede a double quotation mark,
		   every pair of backslashes is interpreted as a literal
		   backslash.  If the number of backslashes is odd, the last
		   backslash escapes the next double quotation mark as
		   described in rule 3.
		See
		http://msdn.microsoft.com/library/en-us/vccelng/htm/progs_12.asp
	*/
	std::string Win32Process::ArgListToString(KListRef argList)
	{
		
		std::string result = "";
		bool needQuote = false;
		for (int i = 0; i < argList->Size(); i++)
		{
			std::string arg = argList->At(i)->ToString();
			std::string backspaceBuf = "";
			
			// Add a space to separate this argument from the others
			if (result.size() > 0) {
				result += ' ';
			}

			needQuote = (arg.find_first_of(" \t") != std::string::npos) || arg == "";
			if (needQuote) {
				result += '"';
			}

			for (int j = 0; j < arg.size(); j++)
			{
				char c = arg[j];
				if (c == '\\') {
					// Don't know if we need to double yet.
					backspaceBuf += c;
				}
				else if (c == '"') {
					// Double backspaces.
					result.append(backspaceBuf.size()*2, '\\');
					backspaceBuf = "";
					result.append("\\\"");
				}
				else {
					// Normal char
					if (backspaceBuf.size() > 0) {
						result.append(backspaceBuf);
						backspaceBuf = "";
					}
					result += c;
				}
			}
			// Add remaining backspaces, if any.
			if (backspaceBuf.size() > 0) {
				result.append(backspaceBuf);
			}

			if (needQuote) {
				result.append(backspaceBuf);
				result += '"';
			}
		}
		return result;
	}
	
	std::string Win32Process::ArgumentsToString()
	{
		return ArgListToString(args);
	}
	
	void Win32Process::ForkAndExec()
	{
		nativeIn->CreateHandles();
		nativeOut->CreateHandles();
		nativeErr->CreateHandles();

		STARTUPINFO startupInfo;
		startupInfo.cb          = sizeof(STARTUPINFO);
		startupInfo.lpReserved  = NULL;
		startupInfo.lpDesktop   = NULL;
		startupInfo.lpTitle     = NULL;
		startupInfo.dwFlags     = STARTF_FORCEOFFFEEDBACK | STARTF_USESTDHANDLES;
		startupInfo.cbReserved2 = 0;
		startupInfo.lpReserved2 = NULL;
		//startupInfo.hStdInput = nativeIn->GetReadHandle();
		//startupInfo.hStdOutput = nativeOut->GetWriteHandle();
		//startupInfo.hStdError = nativeErr->GetWriteHandle();
		
		HANDLE hProc = GetCurrentProcess();
		nativeIn->DuplicateRead(hProc, &startupInfo.hStdInput);
		nativeOut->DuplicateWrite(hProc, &startupInfo.hStdOutput);
		nativeErr->DuplicateWrite(hProc, &startupInfo.hStdError);
		
		std::string commandLine = ArgListToString(args);
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
		
		//CloseHandle(nativeIn->GetReadHandle());
		
		CloseHandle(startupInfo.hStdInput);
		CloseHandle(startupInfo.hStdOutput);
		CloseHandle(startupInfo.hStdError);
		
		if (!rc) {
			std::string message = "Error launching: " + commandLine;
			logger->Error(message);
			throw ValueException::FromString(message);
		}
		else {
			CloseHandle(processInfo.hThread);
			this->pid = processInfo.dwProcessId;
			this->process = processInfo.hProcess;
			this->running = true;
		}
	}
	
	void Win32Process::MonitorAsync()
	{
		nativeIn->StartMonitor();
		nativeOut->StartMonitor();
		nativeErr->StartMonitor();
	}

	BlobRef Win32Process::MonitorSync()
	{
		KMethodRef readCallback =
			StaticBoundMethod::FromMethod<Win32Process>(
				this, &Win32Process::ReadCallback);

		// Set up the synchronous callbacks
		nativeOut->SetReadCallback(readCallback);
		nativeErr->SetReadCallback(readCallback);

		nativeIn->StartMonitor();
		nativeOut->StartMonitor();
		nativeErr->StartMonitor();

		this->ExitMonitorSync();

		// Unset the callbacks just in case these pipes are used again
		nativeOut->SetReadCallback(0);
		nativeErr->SetReadCallback(0);

		BlobRef output = 0;
		{
			Poco::Mutex::ScopedLock lock(processOutputMutex);
			output = Blob::GlobBlobs(processOutput);
		}
		return output;
	}

	int Win32Process::Wait()
	{
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
		
		DWORD exitCode;
		if (GetExitCodeProcess(this->process, &exitCode) == 0) {
			throw ValueException::FromString("Cannot get exit code for process");
		}
		// close the process before exit.
		if (this->process != INVALID_HANDLE_VALUE)
		{
			CloseHandle(this->process);
			this->process = INVALID_HANDLE_VALUE;
		}
		return exitCode;
	}

	int Win32Process::GetPID()
	{
		return pid;
	}
	
	void Win32Process::ReadCallback(const ValueList& args, KValueRef result)
	{
		if (args.at(0)->IsObject())
		{
			BlobRef blob = args.GetObject(0).cast<Blob>();
			if (!blob.isNull() && blob->Length() > 0)
			{
				Poco::Mutex::ScopedLock lock(processOutputMutex);
				processOutput.push_back(blob);
			}
		}
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
		}
	}
	
	void Win32Process::Kill()
	{
		Terminate();
	}
	
	void Win32Process::SendSignal(int signal)
	{
		logger->Warn("Signals are not supported in Windows");
	}
}
