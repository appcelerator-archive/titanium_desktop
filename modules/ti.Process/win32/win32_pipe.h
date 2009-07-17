/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _WIN32_PIPE_H_
#define _WIN32_PIPE_H_

#include <kroll/base.h>
#include <windows.h>
#include <kroll/kroll.h>
#include "../monitored_pipe.h"

namespace ti
{
	class Win32Pipe : public MonitoredPipe
	{
	public:
		Win32Pipe();	
		virtual void Close();
		virtual int Write(AutoPtr<Blob> data);
		virtual void Flush();
		
		void DuplicateWrite(HANDLE process, LPHANDLE handle);
		void DuplicateRead(HANDLE process, LPHANDLE handle);
		HANDLE GetReadHandle() { return readHandle; }
		HANDLE GetWriteHandle() { return writeHandle; }
		
	protected:
		virtual int RawRead(char *buffer, int size);
		
		HANDLE readHandle, writeHandle;
	};
}

#endif
