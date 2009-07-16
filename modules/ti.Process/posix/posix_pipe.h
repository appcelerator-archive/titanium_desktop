/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _POSIX_PIPE_H_
#define _POSIX_PIPE_H_

#include <kroll/kroll.h>
#include "../monitored_pipe.h"

namespace ti
{
	class PosixPipe : public MonitoredPipe
	{
	public:
		PosixPipe();
		virtual ~PosixPipe();
		virtual void Close();
		virtual int Write(AutoPtr<Blob> data);
		virtual void Flush();

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
