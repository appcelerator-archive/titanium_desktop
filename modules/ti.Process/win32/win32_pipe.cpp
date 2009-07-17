/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#include "win32_pipe.h"

namespace ti
{
	Win32Pipe::Win32Pipe() : MonitoredPipe()
	{
		SECURITY_ATTRIBUTES attr;
		attr.nLength              = sizeof(attr);
		attr.lpSecurityDescriptor = NULL;
		attr.bInheritHandle       = TRUE;
		
		::CreatePipe(&readHandle, &writeHandle, &attr, 0);
	}
	
	void Win32Pipe::Close()
	{
		if (!closed) {
			if (writeHandle != INVALID_HANDLE_VALUE)
				CloseHandle(writeHandle);
			
			MonitoredPipe::Close();
		}
	}
	
	int Win32Pipe::RawRead(char *buffer, int size)
	{
		if (readHandle != INVALID_HANDLE_VALUE) {
			DWORD bytesRead;
			BOOL ok = ReadFile(readHandle, buffer, size, &bytesRead, NULL);
			int error = GetLastError();
			if (ok)
			{
				return bytesRead;
			}
			else if (error == ERROR_BROKEN_PIPE)
			{
				return -1;
			}
			else
			{
				throw ValueException::FromString("Error writing anonymous pipe");
			}
		}
		return -1;
	}
	
	void Win32Pipe::DuplicateRead(HANDLE process, LPHANDLE handle)
	{
		DuplicateHandle(process, readHandle, process, handle, 0, TRUE, DUPLICATE_SAME_ACCESS);
        CloseHandle(readHandle);
	}
	
	void Win32Pipe::DuplicateWrite(HANDLE process, LPHANDLE handle)
	{
		DuplicateHandle(process, writeHandle, process, handle, 0, TRUE, DUPLICATE_SAME_ACCESS);
        CloseHandle(writeHandle);
	}
	
	int Win32Pipe::Write(AutoPtr<Blob> blob)
	{
		Poco::Mutex::ScopedLock lock(mutex);
		
		if (writeHandle != INVALID_HANDLE_VALUE) {
			DWORD bytesWritten;
			BOOL ok = WriteFile(writeHandle, (LPCVOID)blob->Get(), blob->Length(), &bytesWritten, NULL);
			if (ok || GetLastError() == ERROR_BROKEN_PIPE) {
				return bytesWritten;
			}
			else {
				throw ValueException::FromString("Error writing anonymous pipe");
			}
		}
		return 0;
	}
	
	void Win32Pipe::Flush()
	{
		
	}
}
