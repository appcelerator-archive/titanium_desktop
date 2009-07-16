/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _BUFFERED_PIPE_H_
#define _BUFFERED_PIPE_H_

#include <kroll/kroll.h>
#include "pipe.h"

namespace ti
{
	class BufferedPipe;
	typedef AutoPtr<BufferedPipe> SharedBufferedPipe;
	
	class BufferedPipe : public Pipe
	{
		public:
			BufferedPipe();
			
			virtual AutoPtr<Blob> Read(int bufsize=-1);
			virtual AutoPtr<Blob> ReadLine();
			virtual void Close();
			virtual bool IsClosed();
			virtual int Write(AutoPtr<Blob> data);
			void Write(char *data, int length);
			virtual void Flush();
			
			int GetSize();
			const char* GetBuffer();
			AutoPtr<Blob> ToBlob() { return new Blob(&(buffer[0]), buffer.size()); }
			
		protected:
			Poco::Mutex mutex;
			std::vector<char> buffer;
			bool closed, asyncRead;
	};
}

#endif
