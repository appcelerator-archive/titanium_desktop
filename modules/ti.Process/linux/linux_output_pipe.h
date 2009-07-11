/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _LINUX_OUTPUT_PIPE_H_
#define _LINUX_OUTPUT_PIPE_H_

#include <kroll/kroll.h>
#include "../output_pipe.h"

namespace ti
{
	class LinuxOutputPipe : public OutputPipe
	{
	public:
		LinuxOutputPipe();
		virtual ~LinuxOutputPipe();
		virtual void Close();
		virtual bool IsClosed();
		
		int GetReadHandle() { return readHandle; }
		int GetWriteHandle() { return writeHandle; }
		
	protected:
		virtual int Write(AutoPtr<Blob> blob);
		
		int readHandle, writeHandle;
		bool closed;
	};
}


#endif
