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
	typedef SharedPtr<InputPipe> SharedInputPipe;
	
	class BlobInputPipe;
	class InputPipe : public Pipe
	{
	public:
		InputPipe();
		virtual ~InputPipe();
		static SharedInputPipe CreateInputPipe();
		
		virtual SharedPtr<Blob> Read(int bufsize=-1) = 0;
		
		void DataReady(SharedInputPipe pipe = NULL);
		void Closed();
		
		void Join(SharedInputPipe other);
		void Unjoin(SharedInputPipe other);
		bool IsJoined();
		
		std::pair<SharedPtr<BlobInputPipe>, SharedPtr<BlobInputPipe> >& Split();
		void Unsplit();
		bool IsSplit();
		
		void Attach(SharedKObject other);
		void Detach();
		bool IsAttached();
		bool IsPiped() { return IsJoined() || IsSplit() || IsAttached(); }
		
		void SetOnRead(SharedKMethod onRead);
	protected:
		
		void _Read(const ValueList& args, SharedValue result);
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
		SharedInputPipe joinedTo;
		std::vector<SharedInputPipe> joinedPipes;
		SharedKMethod joinedRead;
		SharedKObject* attachedOutput;
		std::pair<SharedPtr<BlobInputPipe>, SharedPtr<BlobInputPipe> >* splitPipes;
		Logger *logger;
	};
}

#endif
