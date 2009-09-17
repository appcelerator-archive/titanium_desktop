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
		virtual int Write(AutoBlob data);
		virtual void CallWrite(SharedKObject target, AutoBlob data);
		virtual void Close();
		virtual void CallClose(SharedKObject target);
		virtual void Flush();
		void Attach(SharedKObject object);
		void Detach(SharedKObject object);
		bool IsAttached();
		AutoPipe Clone();
		std::vector<AutoBlob> readData;
		static void FireEventAsynchronously(AutoPtr<Event> event);

		protected:
		int FindFirstLineFeed(char *data, int length, int *charsToErase);
		void _Close(const ValueList& args, SharedValue result);
		void _SetOnClose(const ValueList& args, SharedValue result);
		void _Attach(const ValueList& args, SharedValue result);
		void _Detach(const ValueList& args, SharedValue result);
		void _IsAttached(const ValueList& args, SharedValue result);
		void _Write(const ValueList& args, SharedValue result);
		void _Flush(const ValueList& args, SharedValue result);
		Poco::Mutex attachedMutex;
		std::vector<SharedKObject> attachedObjects;
		Logger *logger;
	};
}

#endif
