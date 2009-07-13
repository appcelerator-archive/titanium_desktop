/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _OUTPUT_PIPE_H_
#define _OUTPUT_PIPE_H_

#include <kroll/kroll.h>
#include <utility>
#include <vector>
#include "pipe.h"

namespace ti
{
	class OutputPipe;
	typedef AutoPtr<OutputPipe> AutoOutputPipe;
	
	class OutputPipe : public Pipe
	{
	public:
		OutputPipe();
		virtual int Write(AutoPtr<Blob> data) = 0;
		virtual void Flush() = 0;
		static AutoOutputPipe CreateOutputPipe();

	protected:
		void _Write(const ValueList& args, SharedValue result);
		void _Flush(const ValueList& args, SharedValue result);
	};
}

#endif
