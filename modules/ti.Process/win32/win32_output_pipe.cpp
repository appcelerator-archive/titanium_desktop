/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#include "win32_output_pipe.h"

namespace ti
{
	Win32OutputPipe::Win32OutputPipe() : closed(false)
	{
		SECURITY_ATTRIBUTES attr;
		attr.nLength              = sizeof(attr);
		attr.lpSecurityDescriptor = NULL;
		attr.bInheritHandle       = TRUE;
		
		CreatePipe(&readHandle, &writeHandle, &attr, 0);
	}
	
	Win32OutputPipe::~Win32OutputPipe()
	{
		Close();
	}
	
	void Win32OutputPipe::Close()
	{
		if (!closed) {
			closed = true;
			
			if (writeHandle != INVALID_HANDLE_VALUE)
				CloseHandle(writeHandle);
			
			Pipe::Closed();
		}
	}
	
	bool Win32OutputPipe::IsClosed()
	{
		return closed;
	}
	
	int Win32OutputPipe::Write(AutoPtr<Blob> blob)
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
	
	void Win32OutputPipe::DuplicateRead(HANDLE process, LPHANDLE handle)
	{
		DuplicateHandle(process, readHandle, process, handle, 0, TRUE, DUPLICATE_SAME_ACCESS);
        CloseHandle(readHandle);
	}
}