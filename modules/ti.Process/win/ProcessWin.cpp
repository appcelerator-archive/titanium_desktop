/*
 * Copyright (c) 2009-2010 Appcelerator, Inc. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ProcessWin.h"
 
#include <signal.h>

#include "PipeWin.h"

namespace Titanium {

ProcessWin::ProcessWin()
	: logger(Logger::Get("Process.ProcessWin"))
	, nativeIn(new PipeWin(false))
	, nativeOut(new PipeWin(true))
	, nativeErr(new PipeWin(true))
{
	stdinPipe->Attach(this->GetNativeStdin());
}

ProcessWin::~ProcessWin()
{
}

void ProcessWin::RecreateNativePipes()
{
	this->nativeIn = new PipeWin(false);
	this->nativeOut = new PipeWin(true);
	this->nativeErr = new PipeWin(true);
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
std::string ProcessWin::ArgListToString(KListRef argList)
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

std::string ProcessWin::ArgumentsToString()
{
	return ArgListToString(args);
}

void ProcessWin::ForkAndExec()
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
	
	std::string commandLine(ArgListToString(args));
	std::wstring commandLineW(::UTF8ToWide(commandLine));
	logger->Debug("Launching: %s", commandLine.c_str());

	PROCESS_INFORMATION processInfo;
	BOOL rc = CreateProcessW(NULL,
		(wchar_t*) commandLineW.c_str(),
		NULL,
		NULL,
		TRUE,
		CREATE_NO_WINDOW, // If this is a console application,
           NULL,             // don't open a console window.
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

void ProcessWin::MonitorAsync()
{
	nativeIn->StartMonitor();
	nativeOut->StartMonitor();
	nativeErr->StartMonitor();
}

BytesRef ProcessWin::MonitorSync()
{
	KMethodRef readCallback =
		StaticBoundMethod::FromMethod<ProcessWin>(
			this, &ProcessWin::ReadCallback);

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

	BytesRef output = 0;
	{
		Poco::Mutex::ScopedLock lock(processOutputMutex);
		output = Bytes::Concat(processOutput);
	}
	return output;
}

int ProcessWin::Wait()
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

int ProcessWin::GetPID()
{
	return pid;
}

void ProcessWin::ReadCallback(const ValueList& args, KValueRef result)
{
	if (args.at(0)->IsObject())
	{
		BytesRef bytes = args.GetObject(0).cast<Bytes>();
		if (!bytes.isNull() && bytes->Length() > 0)
		{
			Poco::Mutex::ScopedLock lock(processOutputMutex);
			processOutput.push_back(bytes);
		}
	}
}

void ProcessWin::Terminate()
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

void ProcessWin::Kill()
{
	Terminate();
}

void ProcessWin::SendSignal(int signal)
{
	logger->Warn("Signals are not supported in Windows");
}

} // namespace Titanium
