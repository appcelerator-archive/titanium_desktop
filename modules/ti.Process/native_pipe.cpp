/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
 
#include "native_pipe.h"
#define MILLISECONDS_BETWEEN_READ_FLUSHES 2500
#define MAX_BUFFER_SIZE 512

namespace ti
{
	NativePipe::NativePipe(bool isReader) :
		Pipe("Process.NativePipe"),
		closed(false),
		isReader(isReader),
		writeThreadAdapter(new Poco::RunnableAdapter<NativePipe>(
			*this, &NativePipe::PollForWrites)),
		readThreadAdapter(new Poco::RunnableAdapter<NativePipe>(
			*this, &NativePipe::PollForReads)),
		readCallback(0),
		logger(Logger::Get("Process.NativePipe"))
	{
	}

	NativePipe::~NativePipe ()
	{
		// Don't need to StopMonitors here, because the destructor
		// should never be called until the monitors are shutdown
		delete readThreadAdapter;
		delete writeThreadAdapter;
	}

	void NativePipe::StopMonitors()
	{
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

	void NativePipe::Close()
	{
		if (!isReader)
		{
			closed = true;
		}
		Pipe::Close();
	}

	int NativePipe::Write(BytesRef bytes)
	{
		if (isReader)
		{
			// If this is a reader pipe (ie one reading from stdout and stderr
			// via polling), then we want to pass along the data to all attached
			// pipes

			// Someone (probably a process) wants to subscribe to this pipe's
			// reads synchronously. So we need to call the callback on this thread
			// right now.
			if (!readCallback.isNull())
			{
				readCallback->Call(Value::NewObject(bytes));
			}

			return Pipe::Write(bytes);
		}
		else
		{
			// If this is not a reader pipe (ie one that simply accepts write
			// requests via the Write(...) method, like stdin), then queue the
			// data to be written to the native pipe (blocking operation) by
			// our writer thread.:
			Poco::Mutex::ScopedLock lock(buffersMutex);
			buffers.push(bytes);
		}

		return bytes->Length();
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
		KObjectRef save(this, true);

		char buffer[MAX_BUFFER_SIZE];
		int length = MAX_BUFFER_SIZE;

		int bytesRead = this->RawRead(buffer, length);

		while (bytesRead > 0)
		{
			BytesRef bytes = new Bytes(buffer, bytesRead);
			this->Write(bytes);
			bytesRead = this->RawRead(buffer, length);
		}

		this->CloseNativeRead();
	}

	void NativePipe::PollForWrites()
	{
		KObjectRef save(this, true);

		BytesRef bytes = 0;
		while (!closed || buffers.size() > 0)
		{
			PollForWriteIteration();
			Poco::Thread::sleep(50);
		}

		this->CloseNativeWrite();
	}

	void NativePipe::PollForWriteIteration()
	{
		BytesRef bytes = 0;
		while (buffers.size() > 0)
		{
			{
				Poco::Mutex::ScopedLock lock(buffersMutex);
				bytes = buffers.front();
				buffers.pop();
			}
			if (!bytes.isNull())
			{
				this->RawWrite(bytes);
				bytes = 0;
			}
		}
	}

	void NativePipe::RawWrite(BytesRef bytes)
	{
		try
		{
			this->RawWrite((char*) bytes->Get(), bytes->Length());
		}
		catch (Poco::Exception& e)
		{
			logger->Error("Exception while try to write to pipe Pipe: %s",
				e.displayText().c_str());
		}
	}

	void NativePipe::CloseNative()
	{
		this->CloseNativeRead();
		this->CloseNativeWrite();
	}

}
