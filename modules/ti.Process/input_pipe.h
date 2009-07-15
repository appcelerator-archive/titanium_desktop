/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _INPUT_PIPE_H_
#define _INPUT_PIPE_H_

#include <kroll/kroll.h>
#include <utility>
#include <vector>
#include "pipe.h"

namespace ti
{
	class InputPipe;
	typedef AutoPtr<InputPipe> AutoInputPipe;

	class BufferedInputPipe;
	class InputPipe : public Pipe
	{
	public:
		InputPipe();
		virtual ~InputPipe();
		static AutoInputPipe CreateInputPipe();
		
		virtual AutoPtr<Blob> Read(int bufsize=-1) = 0;
		virtual AutoPtr<Blob> ReadLine() = 0;
		void DataReady(AutoInputPipe pipe = NULL);
		void Closed();
		virtual bool IsClosed() = 0;
		
		void Join(AutoInputPipe other);
		void Unjoin(AutoInputPipe other);
		bool IsJoined();
		
		void Split();
		void Unsplit();
		bool IsSplit();
		
		void Attach(SharedKObject other);
		void Detach();
		bool IsAttached();
		bool IsPiped() { return IsJoined() || IsSplit() || IsAttached(); }
		
		void SetAsyncOnRead(bool asyncOnRead) { this->asyncOnRead = asyncOnRead; }
		void SetOnRead(SharedKMethod onRead);
		AutoInputPipe Clone();
		
	protected:
		int FindFirstLineFeed(char *data, int length, int *charsToErase);
		
		void _Read(const ValueList& args, SharedValue result);
		void _ReadLine(const ValueList& args, SharedValue result);
		void _Join(const ValueList& args, SharedValue result);
		void _Unjoin(const ValueList& args, SharedValue result);
		void _IsJoined(const ValueList& args, SharedValue result);
		void _Split(const ValueList& args, SharedValue result);
		void _Unsplit(const ValueList& args, SharedValue result);
		void _IsSplit(const ValueList& args, SharedValue result);
		void _Attach(const ValueList& args, SharedValue result);
		void _Detach(const ValueList& args, SharedValue result);
		void _IsAttached(const ValueList& args, SharedValue result);
		void _IsPiped(const ValueList& args, SharedValue result);
		void _SetOnRead(const ValueList& args, SharedValue result);
		void _SetOnClose(const ValueList& args, SharedValue result);
		
		// these aren't exposed to JS
		void _Splitter(const ValueList& args, SharedValue result);
		void _SplitterClose(const ValueList& args, SharedValue result);
		void JoinedRead(const ValueList& args, SharedValue result);
		void Splitter(const ValueList& args, SharedValue result);
		void SplitterClose(const ValueList& args, SharedValue result);
		
		SharedKMethod *onRead, *onClose;
		bool joined;
		AutoInputPipe joinedTo;
		std::vector<AutoInputPipe> joinedPipes;
		SharedKMethod joinedRead;
		SharedKObject* attachedOutput;
		Poco::Mutex splitMutex;
		AutoPtr<BufferedInputPipe> splitPipe1, splitPipe2;
		Logger *logger;
		bool asyncOnRead;
	};
}

#endif
