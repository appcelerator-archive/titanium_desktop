/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _WIN32_PIPE_H_
#define _WIN32_PIPE_H_

#include <kroll/kroll.h>
#include "../native_pipe.h"

namespace ti
{
	class Win32Pipe : public NativePipe
	{
	public:
		Win32Pipe(bool isReader);
		virtual void CreateHandles();
		virtual void Close();
		virtual void CloseNativeRead();
		virtual void CloseNativeWrite();
		void DuplicateWrite(HANDLE process, LPHANDLE handle);
		void DuplicateRead(HANDLE process, LPHANDLE handle);
		HANDLE GetReadHandle() { return readHandle; }
		HANDLE GetWriteHandle() { return writeHandle; }
		
	protected:
		virtual int RawRead(char *buffer, int size);
		virtual int RawWrite(const char *buffer, int size);
		
		Poco::Mutex mutex;
		HANDLE readHandle, writeHandle;
		Logger *logger;
	};
}

#endif
