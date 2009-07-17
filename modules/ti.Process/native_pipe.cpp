/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
 
#include "native_pipe.h"

namespace ti
{
	NativePipe::NativePipe(bool isReader) :
		closed(false),
		isReader(isReader),
		writeThreadAdapter(new Poco::RunnableAdapter<NativePipe>(
			*this, &NativePipe::PollForWrites)),
		readThreadAdapter(new Poco::RunnableAdapter<NativePipe>(
			*this, &NativePipe::PollForReads)),
		logger(Logger::Get("Process.NativePipe"))
	{
	};

	NativePipe::~NativePipe ()
	{
		delete readThreadAdapter;
		delete writeThreadAdapter;
	}

	void NativePipe::Close()
	{
		// Do we need to send EOF here for writer threads? 

		closed = true;
		try
		{
			if (readThread.isRunning())
					this->readThread.join();
			if (writeThread.isRunning())
					this->writeThread.join();
		}
		catch (Poco::Exception& e)
		{
			logger->Error("Exception while try to join with Pipe thread: %s",
				e.displayText().c_str());
		}
	}

	int NativePipe::Write(AutoBlob blob)
	{
		if (isReader)
		{
			// If this is a reader pipe (ie one reading from stdout and stderr
			// via polling), then we want to pass along the data to all attached
			// pipes
			return Pipe::Write(blob);
		}
		else
		{
			// If this is not a reader pipe (ie one that simply accepts write
			// requests via the Write(...) method, like stdin), then queue the
			// data to be written to the native pipe (blocking operation) by
			// our writer thread.
			Poco::Mutex::ScopedLock lock(buffersMutex);
			buffers.push(blob);
		}

		return blob->Length();
	}

	void NativePipe::StartMonitor()
	{
		if (isReader)
		{
			readThread.start(*readThreadAdapter);
		}
		else
		{
			writeThread.start(*writeThreadAdapter);
		}
	}

	void NativePipe::PollForReads()
	{
		char buffer[MAX_BUFFER_SIZE];
		int length = MAX_BUFFER_SIZE;
		int bytesRead = this->RawRead(buffer, length);
		while (bytesRead > 0)
		{
			bytesRead = this->RawRead(buffer, length);
			AutoBlob blob = new Blob(buffer, bytesRead);
			this->Write(blob);
		}
	}

	void NativePipe::PollForWrites()
	{
		AutoBlob blob = 0;
		while (!closed || buffers.size() > 0)
		{
			if (buffers.size() > 0)
			{
				Poco::Mutex::ScopedLock lock(buffersMutex);
				blob = buffers.front();
				buffers.pop();
			}

			if (!blob.isNull())
			{
				this->RawWrite(blob);
			}
		}
	}

	void NativePipe::RawWrite(AutoBlob blob)
	{
		try
		{
			this->RawWrite((char*) blob->Get(), blob->Length());
		}
		catch (Poco::Exception& e)
		{
			logger->Error("Exception while try to write to pipe Pipe: %s",
				e.displayText().c_str());
		}
	}
}
