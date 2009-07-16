/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _POSIX_OUTPUT_PIPE_H_
#define _POSIX_OUTPUT_PIPE_H_

#include <kroll/kroll.h>
#include "../output_pipe.h"

namespace ti
{
	class PosixOutputPipe : public OutputPipe
	{
	public:
		PosixOutputPipe();
		virtual ~PosixOutputPipe();
		virtual void Close();
		virtual bool IsClosed();
		virtual int Write(AutoPtr<Blob> data);
		virtual void Flush();
		
		int GetReadHandle() { return readHandle; }
		int GetWriteHandle() { return writeHandle; }
		
	protected:
		int readHandle;
		int writeHandle;
		bool closed;
	};
}


#endif
