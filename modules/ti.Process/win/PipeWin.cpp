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

#include "PipeWin.h"

namespace Titanium {

PipeWin::PipeWin(bool isReader)
	: NativePipe(isReader)
	, readHandle(INVALID_HANDLE_VALUE)
	, writeHandle(INVALID_HANDLE_VALUE)
	, logger(Logger::Get("Process.PipeWin"))
{
}

void PipeWin::CreateHandles()
{
	SECURITY_ATTRIBUTES attr;
	attr.nLength              = sizeof(attr);
	attr.lpSecurityDescriptor = NULL;
	attr.bInheritHandle       = FALSE;
	
	::CreatePipe(&readHandle, &writeHandle, &attr, 0);
}

void PipeWin::Close()
{
	NativePipe::Close();
}

int PipeWin::RawRead(char *buffer, int size)
{
	if (readHandle != INVALID_HANDLE_VALUE)
	{
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

int PipeWin::RawWrite(const char *data, int size)
{
	if (writeHandle != INVALID_HANDLE_VALUE)
	{
		DWORD bytesWritten;
		BOOL ok = WriteFile(writeHandle, (LPCVOID)data, size, &bytesWritten, NULL);
		if (ok || GetLastError() == ERROR_BROKEN_PIPE) {
			return bytesWritten;
		}
		else {
			throw ValueException::FromString("Error writing anonymous pipe");
		}
	}
	return 0;
}

void PipeWin::DuplicateRead(HANDLE process, LPHANDLE handle)
{
	DuplicateHandle(process, readHandle, process, handle, 0, TRUE, DUPLICATE_SAME_ACCESS);
	this->CloseNativeRead();
}

void PipeWin::DuplicateWrite(HANDLE process, LPHANDLE handle)
{
	DuplicateHandle(process, writeHandle, process, handle, 0, TRUE, DUPLICATE_SAME_ACCESS);
	this->CloseNativeWrite();
}

void PipeWin::CloseNativeRead()
{
	if (readHandle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(readHandle);
		readHandle = INVALID_HANDLE_VALUE;
	}
}

void PipeWin::CloseNativeWrite()
{
	if (writeHandle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(writeHandle);
		writeHandle = INVALID_HANDLE_VALUE;
	}
}

} // namespace Titanium
