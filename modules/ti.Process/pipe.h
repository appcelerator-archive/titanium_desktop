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
#include <Poco/ThreadTarget.h>

namespace ti
{
	class Pipe;
	typedef AutoPtr<Pipe> AutoPipe;

	class Pipe : public KEventObject
	{
		public:
		Pipe(const char *type = "Process.Pipe");
		virtual ~Pipe();
		virtual int Write(BytesRef data);
		virtual void CallWrite(KObjectRef target, BytesRef data);
		virtual void Close();
		virtual void CallClose(KObjectRef target);
		virtual void Flush();
		void Attach(KObjectRef object);
		void Detach(KObjectRef object);
		bool IsAttached();
		AutoPipe Clone();
		std::vector<BytesRef> readData;
		static void FireEventAsynchronously(AutoPtr<Event> event);

		protected:
		int FindFirstLineFeed(char *data, int length, int *charsToErase);
		void _Close(const ValueList& args, KValueRef result);
		void _SetOnClose(const ValueList& args, KValueRef result);
		void _Attach(const ValueList& args, KValueRef result);
		void _Detach(const ValueList& args, KValueRef result);
		void _IsAttached(const ValueList& args, KValueRef result);
		void _Write(const ValueList& args, KValueRef result);
		void _Flush(const ValueList& args, KValueRef result);
		Poco::Mutex attachedMutex;
		std::vector<KObjectRef> attachedObjects;
		Logger *logger;
	};
}

#endif
