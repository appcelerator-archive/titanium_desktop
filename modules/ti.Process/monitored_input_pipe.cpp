/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
 
#include "monitored_input_pipe.h"

namespace ti
{
	
	MonitoredInputPipe::~MonitoredInputPipe ()
	{
		Close();
		
		if (monitorAdapter)
		{
			delete monitorAdapter;
		}
	}

	void MonitoredInputPipe::Close()
	{
		if (!closed)
		{
			if (monitorThread.isRunning())
			{
				try
				{
					this->monitorThread.join();
				}
				catch (Poco::Exception& e)
				{
					Logger::Get("Process.MonitoredInputPipe")->Error(
						"Exception while try to join with InputPipe thread: %s",
						e.displayText().c_str());
				}
			}
			
			BufferedInputPipe::Close();
		}
	}
	
	void MonitoredInputPipe::StartMonitor()
	{
		monitorAdapter = new Poco::RunnableAdapter<MonitoredInputPipe>(*this, &MonitoredInputPipe::MonitorThread);
		monitorThread.start(*monitorAdapter);
	}
	
	void MonitoredInputPipe::MonitorThread()
	{
		char buffer[MAX_BUFFER_SIZE];
		int length = MAX_BUFFER_SIZE;
		int bytesRead = this->RawRead(buffer, length);
		while (bytesRead > 0) {
			bytesRead = this->RawRead(buffer, length);
			if (bytesRead > 0)
			{
				this->Append(buffer, bytesRead);
			}
		}
	}
}
