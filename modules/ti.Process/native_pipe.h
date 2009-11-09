/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
 
#ifndef _NATIVE_PIPE_H_
#define _NATIVE_PIPE_H_

#include "pipe.h"
#include <kroll/kroll.h>
#include <Poco/Thread.h>
#include <Poco/RunnableAdapter.h>

namespace ti
{
	class NativePipe : public Pipe
	{
	public:
		NativePipe(bool isReader);
		~NativePipe();
		void StartMonitor();
		void StartMonitor(KMethodRef readCallback);
		virtual void StopMonitors();
		virtual int Write(BlobRef blob);
		void PollForWriteIteration();
		virtual void Close();
		virtual void CloseNative();
		virtual void CloseNativeRead() = 0;
		virtual void CloseNativeWrite() = 0;
		inline void SetReadCallback(KMethodRef cb) { this->readCallback = cb; }

	protected:
		bool closed;
		bool isReader;
		std::vector<KObjectRef> attachedObjects;
		Poco::RunnableAdapter<NativePipe>* writeThreadAdapter;
		Poco::RunnableAdapter<NativePipe>* readThreadAdapter;
		Poco::Thread writeThread;
		Poco::Thread readThread;
		KMethodRef readCallback;
		Logger* logger;
		Poco::Mutex buffersMutex;
		std::queue<BlobRef> buffers;

		void PollForReads();
		void PollForWrites();
		virtual void RawWrite(BlobRef blob);
		virtual int RawRead(char *buffer, int size) = 0;
		virtual int RawWrite(const char *buffer, int size) = 0;
	};
}

#endif
