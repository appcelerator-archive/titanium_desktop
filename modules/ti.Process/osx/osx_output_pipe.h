/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _OSX_OUTPUT_PIPE_H_
#define _OSX_OUTPUT_PIPE_H_

#include <kroll/kroll.h>
#include "../output_pipe.h"
#include <Cocoa/Cocoa.h>

namespace ti
{
	class OSXOutputPipe : public OutputPipe
	{
	public:
		OSXOutputPipe();
		virtual ~OSXOutputPipe();
		
		virtual void Close();
		virtual bool IsClosed();
		virtual int Write(SharedPtr<Blob> data, int size=-1);
		NSPipe* GetPipe() { return pipe; }
		
	protected:
		NSPipe* pipe;
		NSFileHandle* handle;
		bool closed;
	};
}

#endif