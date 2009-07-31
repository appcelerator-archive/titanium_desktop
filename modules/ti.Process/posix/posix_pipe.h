/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _POSIX_PIPE_H_
#define _POSIX_PIPE_H_

#include <kroll/kroll.h>
#include "../native_pipe.h"

namespace ti
{
	class PosixPipe : public NativePipe
	{
	public:
		PosixPipe(bool isReader);
		virtual void CreateHandles();
		virtual void Close();
		virtual void CloseNativeRead();
		virtual void CloseNativeWrite();
		inline int GetReadHandle() { return readHandle; }
		inline int GetWriteHandle() { return writeHandle; }

	protected:
		int readHandle;
		int writeHandle;
		virtual int RawRead(char *buffer, int size);
		virtual int RawWrite(const char *buffer, int size);
	};
}

#endif
