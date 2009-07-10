/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _WIN32_INPUT_PIPE_H_
#define _WIN32_INPUT_PIPE_H_

#include <kroll/base.h>
#include <windows.h>
#include <Poco/RunnableAdapter.h>
#include <kroll/kroll.h>
#include "../buffered_input_pipe.h"
#include <vector>

namespace ti
{
	class Win32InputPipe : public BufferedInputPipe
	{
	public:
		Win32InputPipe();
		virtual ~Win32InputPipe();
		virtual void Close();
		
		void DuplicateWrite(HANDLE process, LPHANDLE handle);
		HANDLE GetReadHandle() { return readHandle; }
		HANDLE GetWriteHandle() { return writeHandle; }
		void StartMonitor();
		
	protected:
		int RawRead(char *buffer, int size);
		void MonitorThread();
		
		Poco::Thread monitorThread;
		Poco::RunnableAdapter<Win32InputPipe>* monitorAdapter;
		HANDLE readHandle, writeHandle;
		bool closed;
	};
}

#endif