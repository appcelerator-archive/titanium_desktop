/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#include "win32_input_pipe.h"

namespace ti
{
	Win32InputPipe::Win32InputPipe() : BufferedInputPipe()
	{
		SECURITY_ATTRIBUTES attr;
		attr.nLength              = sizeof(attr);
		attr.lpSecurityDescriptor = NULL;
		attr.bInheritHandle       = FALSE;
		
		CreatePipe(&readHandle, &writeHandle, &attr, 0);
		
		monitorAdapter = new Poco::RunnableAdapter<Win32InputPipe>(*this, &Win32InputPipe::MonitorThread);
		monitorThread.start(*monitorAdapter);
	}
	
	Win32InputPipe::~Win32InputPipe()
	{
		Close();
	}
	
	void Win32InputPipe::Close()
	{
		if (!closed) {
			if (writeHandle != INVALID_HANDLE_VALUE)
				CloseHandle(writeHandle);
				
			if (monitorThread.isRunning())
			{
				try
				{
					this->monitorThread.join();
				}
				catch (Poco::Exception& e)
				{
					Logger::Get("Process.Win32InputPipe")->Error(
						"Exception while try to join with InputPipe thread: %s",
						e.displayText().c_str());
				}
			}
			
			delete monitorAdapter;
			
			BufferedInputPipe::Close();
		}
	}
	
	int Win32InputPipe::RawRead(char *buffer, int size)
	{
		if (readHandle != INVALID_HANDLE_VALUE) {
			DWORD bytesRead;
			BOOL ok = ReadFile(readHandle, buffer, size, &bytesRead, NULL);
			if (ok || GetLastError() == ERROR_BROKEN_PIPE) {
				return bytesRead;
			}
			else {
				throw ValueException::FromString("Error writing anonymous pipe");
			}
		}
		return -1;
	}
	
	void Win32InputPipe::MonitorThread()
	{
		char buffer[1024];
		int length = 1024;
		int bytesRead = this->RawRead(buffer, length);
		while (bytesRead > 0) {
			bytesRead = this->RawRead(buffer, length);
			if (bytesRead > 0)
			{
				this->Append(buffer, bytesRead);
			}
		}
	}
	
	void Win32InputPipe::DuplicateWrite(HANDLE process, LPHANDLE handle)
	{
		DuplicateHandle(process, writeHandle, process, handle, 0, TRUE, DUPLICATE_SAME_ACCESS);
        CloseHandle(writeHandle);
	}
}