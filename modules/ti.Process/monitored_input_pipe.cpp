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
		this->Close();
	}

	void MonitoredInputPipe::Close()
	{
		if (!closed)
		{
			if (monitorThread.isRunning())
			{
				JoinMonitor();
				delete monitorAdapter;
			}
			
			BufferedInputPipe::Close();
		}
	}
	
	void MonitoredInputPipe::JoinMonitor()
	{
		if (monitorJoined) return;
		monitorJoined = true;
		
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
			Logger::Get("Process.MonitoredInputPipe")->Debug("monitored data ready: %d bytes, %s", bytesRead, buffer);
			this->Append(buffer, bytesRead);
			bytesRead = this->RawRead(buffer, length);
		}
	}
}
