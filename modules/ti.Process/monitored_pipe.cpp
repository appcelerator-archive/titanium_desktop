/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
 
#include "monitored_pipe.h"

namespace ti
{
	
	MonitoredPipe::~MonitoredPipe ()
	{
		this->Close();
	}

	void MonitoredPipe::Close()
	{
		if (!closed)
		{
			if (monitorThread.isRunning())
			{
				JoinMonitor();
				delete monitorAdapter;
			}
			
			Pipe::Close();
		}
	}
	
	void MonitoredPipe::JoinMonitor()
	{
		if (monitorJoined) return;
		monitorJoined = true;
		
		try
		{
			this->monitorThread.join();
		}
		catch (Poco::Exception& e)
		{
			Logger::Get("Process.MonitoredPipe")->Error(
				"Exception while try to join with Pipe thread: %s",
				e.displayText().c_str());
		}
	}
	
	void MonitoredPipe::StartMonitor()
	{
		monitorAdapter = new Poco::RunnableAdapter<MonitoredPipe>(*this, &MonitoredPipe::MonitorThread);
		monitorThread.start(*monitorAdapter);
	}
	
	void MonitoredPipe::MonitorThread()
	{
		char buffer[MAX_BUFFER_SIZE];
		int length = MAX_BUFFER_SIZE;
		int bytesRead = this->RawRead(buffer, length);
		while (bytesRead > 0) {
			Logger::Get("Process.MonitoredPipe")->Debug("monitored data ready: %d bytes, %s", bytesRead, buffer);
			this->Write(buffer, bytesRead);
			bytesRead = this->RawRead(buffer, length);
		}
	}
}
