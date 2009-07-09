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
	typedef SharedPtr<OutputPipe> SharedOutputPipe;
	
	class OutputPipe : public Pipe
	{
	public:
		OutputPipe();
		virtual int Write(SharedPtr<Blob> data) = 0;
		static SharedOutputPipe CreateOutputPipe();
		
	protected:
		void _Write(const ValueList& args, SharedValue result);
		
	};	
}

#endif