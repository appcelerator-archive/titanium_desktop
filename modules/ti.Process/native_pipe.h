/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
 
#ifndef _NATIVE_PIPE_H_
#define _NATIVE_PIPE_H_

#define MAX_BUFFER_SIZE 4096

#include "pipe.h"
#include <kroll/kroll.h>
#include <Poco/Thread.h>
#include <Poco/RunnableAdapter.h>

namespace ti
{
	class NativePipe : public ReferenceCounted
	{
	public:
		NativePipe(AutoPipe delegate) : monitorJoined(false), delegate(delegate) {};
		~NativePipe();
		virtual void Close();
		void StartMonitor();
		void JoinMonitor();

		AutoPipe GetDelegate() { return delegate; }
		virtual void Write(AutoBlob blob) = 0;
		virtual void EndOfFile() = 0;
		
	protected:
		void MonitorThread();
	
		virtual int RawRead(char *buffer, int size) = 0;
		Poco::Thread monitorThread;
		Poco::RunnableAdapter<NativePipe>* monitorAdapter;
		bool monitorJoined;
		bool closed;
		
		AutoPipe delegate;
	};
}

#endif
