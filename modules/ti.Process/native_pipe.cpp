/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
 
#include "native_pipe.h"

namespace ti
{
	
	NativePipe::~NativePipe ()
	{
		this->Close();
	}

	void NativePipe::Close()
	{
		if (!closed)
		{
			if (monitorThread.isRunning())
			{
				JoinMonitor();
				delete monitorAdapter;
			}
		}
	}
	
	void NativePipe::JoinMonitor()
	{
		if (monitorJoined) return;
		monitorJoined = true;
		
		try
		{
			this->monitorThread.join();
		}
		catch (Poco::Exception& e)
		{
			Logger::Get("Process.NativePipe")->Error(
				"Exception while try to join with Pipe thread: %s",
				e.displayText().c_str());
		}
	}
	
	void NativePipe::StartMonitor()
	{
		monitorAdapter = new Poco::RunnableAdapter<NativePipe>(*this, &NativePipe::MonitorThread);
		monitorThread.start(*monitorAdapter);
	}
	
	void NativePipe::MonitorThread()
	{
		char buffer[MAX_BUFFER_SIZE];
		int length = MAX_BUFFER_SIZE;
		int bytesRead = this->RawRead(buffer, length);
		while (bytesRead > 0) {
			delegate->Write(buffer, bytesRead);
			bytesRead = this->RawRead(buffer, length);
		}
	}
}
