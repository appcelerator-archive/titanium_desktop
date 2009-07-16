/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _POSIX_INPUT_PIPE_H_
#define _POSIX_INPUT_PIPE_H_

#include <kroll/kroll.h>
#include "../monitored_input_pipe.h"

namespace ti
{
	class PosixInputPipe : public MonitoredInputPipe
	{
	public:
		PosixInputPipe();
		virtual ~PosixInputPipe();
		virtual void Close();

		int GetReadHandle() { return readHandle; }
		int GetWriteHandle() { return writeHandle; }
		
	protected:
		friend class PosixProcess;
		virtual int RawRead(char *buffer, int size);
		int readHandle;
		int writeHandle;
	};
}

#endif
