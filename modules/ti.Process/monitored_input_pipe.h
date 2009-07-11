/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
 
#ifndef _MONITORED_INPUT_PIPE_H_
#define _MONITORED_INPUT_PIPE_H_

#define MAX_BUFFER_SIZE 4096

#include "buffered_input_pipe.h"
#include <Poco/Thread.h>
#include <Poco/RunnableAdapter.h>

namespace ti
{
	class MonitoredInputPipe : public BufferedInputPipe
	{
	public:
		MonitoredInputPipe() : BufferedInputPipe() {};
		~MonitoredInputPipe();
		virtual void Close();
		
		void StartMonitor();
		
	protected:
		void MonitorThread();
	
		virtual int RawRead(char *buffer, int size) = 0;
		Poco::Thread monitorThread;
		Poco::RunnableAdapter<MonitoredInputPipe>* monitorAdapter;
		
	};
}

#endif
