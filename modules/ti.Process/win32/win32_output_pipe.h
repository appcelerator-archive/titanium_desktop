/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _WIN32_OUTPUT_PIPE_H_
#define _WIN32_OUTPUT_PIPE_H_

#include <kroll/base.h>
#include <windows.h>
#include <Poco/RunnableAdapter.h>
#include <kroll/kroll.h>
#include "../output_pipe.h"
#include <vector>

namespace ti
{
	class Win32OutputPipe : public OutputPipe
	{
	public:
		Win32OutputPipe();
		virtual ~Win32OutputPipe();
		
		virtual void Close();
		virtual bool IsClosed();
		virtual int Write(AutoPtr<Blob> data);
		
		void DuplicateRead(HANDLE process, LPHANDLE handle);
		HANDLE GetReadHandle() { return readHandle; }
		HANDLE GetWriteHandle() { return writeHandle; }
		
	protected:
		Poco::Mutex mutex;
		HANDLE readHandle, writeHandle;
		bool closed;
	};
}

#endif