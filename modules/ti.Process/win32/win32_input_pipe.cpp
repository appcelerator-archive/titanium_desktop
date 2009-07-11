/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#include "win32_input_pipe.h"

namespace ti
{
	Win32InputPipe::Win32InputPipe() : MonitoredInputPipe()
	{
		SECURITY_ATTRIBUTES attr;
		attr.nLength              = sizeof(attr);
		attr.lpSecurityDescriptor = NULL;
		attr.bInheritHandle       = TRUE;
		
		CreatePipe(&readHandle, &writeHandle, &attr, 0);
	}
	
	void Win32InputPipe::Close()
	{
		if (!closed) {
			if (writeHandle != INVALID_HANDLE_VALUE)
				CloseHandle(writeHandle);
			
			MonitoredInputPipe::Close();
		}
	}
	
	int Win32InputPipe::RawRead(char *buffer, int size)
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
	
	void Win32InputPipe::DuplicateWrite(HANDLE process, LPHANDLE handle)
	{
		DuplicateHandle(process, writeHandle, process, handle, 0, TRUE, DUPLICATE_SAME_ACCESS);
        CloseHandle(writeHandle);
	}
}
