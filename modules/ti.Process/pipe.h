/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _PIPE_H_
#define _PIPE_H_

#include <kroll/kroll.h>

namespace ti
{
	class Pipe;
	typedef AutoPtr<Pipe> AutoPipe;

	class Pipe : public StaticBoundObject
	{
	public:
		static AutoPipe CreatePipe();
		
		Pipe(const char *type = "Process.Pipe");
		virtual ~Pipe() {};

		virtual AutoPtr<Blob> Read(int bufsize=-1) = 0;
		virtual AutoPtr<Blob> ReadLine() = 0;
		virtual void Close() = 0;
		virtual bool IsClosed() = 0;
		virtual int Write(AutoPtr<Blob> data) = 0;
		virtual void Flush() = 0;
		
		void DataReady(AutoPipe pipe = NULL);
		void Attach(SharedKObject other);
		void Detach();
		bool IsAttached();
		
		void SetAsyncOnRead(bool asyncOnRead) { this->asyncOnRead = asyncOnRead; }
		void SetOnRead(SharedKMethod onRead);
		AutoPipe Clone();

		virtual void Closed();
		void SetOnClose(SharedKMethod onClose);
		
	protected:	
		int FindFirstLineFeed(char *data, int length, int *charsToErase);
		
		void _Close(const ValueList& args, SharedValue result);
		void _IsClosed(const ValueList& args, SharedValue result);
		void _SetOnClose(const ValueList& args, SharedValue result);
		void _Read(const ValueList& args, SharedValue result);
		void _ReadLine(const ValueList& args, SharedValue result);
		void _IsJoined(const ValueList& args, SharedValue result);
		void _Attach(const ValueList& args, SharedValue result);
		void _Detach(const ValueList& args, SharedValue result);
		void _IsAttached(const ValueList& args, SharedValue result);
		void _SetOnRead(const ValueList& args, SharedValue result);
		void _Write(const ValueList& args, SharedValue result);
		void _Flush(const ValueList& args, SharedValue result);
			
		SharedKMethod *onRead, *onClose;
		SharedKObject* attachedOutput;
		Logger *logger;
		bool asyncOnRead;
	};
}

#endif
