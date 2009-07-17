/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _PIPE_H_
#define _PIPE_H_

#include <kroll/kroll.h>
#include <queue>
#include <Poco/Thread.h>
#include <Poco/RunnableAdapter.h>

namespace ti
{
	class Pipe;
	typedef AutoPtr<Pipe> AutoPipe;

	class Pipe : public KEventObject
	{
	public:
		Pipe(const char *type = "Process.Pipe");
		virtual ~Pipe();

		virtual void Close();
		virtual bool IsClosed();
		virtual int Write(AutoBlob data);
		virtual void CallWrite(SharedKObject target, AutoBlob data);
		virtual void Flush();

		int GetSize();
		const char* GetBuffer();

		void Attach(SharedKObject object);
		void Detach(SharedKObject object);
		bool IsAttached();
		AutoPipe Clone();

		virtual void Closed();
		void SetOnClose(SharedKMethod onClose);

	protected:
		int FindFirstLineFeed(char *data, int length, int *charsToErase);
		void FireEvents();
		
		void _Close(const ValueList& args, SharedValue result);
		void _IsClosed(const ValueList& args, SharedValue result);
		void _SetOnClose(const ValueList& args, SharedValue result);
		void _Attach(const ValueList& args, SharedValue result);
		void _Detach(const ValueList& args, SharedValue result);
		void _IsAttached(const ValueList& args, SharedValue result);
		void _Write(const ValueList& args, SharedValue result);
		void _Flush(const ValueList& args, SharedValue result);

		SharedKMethod *onClose;
		
		Poco::Mutex attachedMutex;
		std::vector<SharedKObject> attachedObjects;
		
		Logger *logger;
		bool closed;

		Poco::Mutex buffersMutex;
		std::queue<AutoBlob> buffers;
		Poco::Thread* eventsThread;
		Poco::RunnableAdapter<Pipe>* eventsThreadAdapter;
	};
}

#endif
