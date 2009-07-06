/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _BLOB_INPUT_PIPE_H_
#define _BLOB_INPUT_PIPE_H_

#include <kroll/kroll.h>
#include "input_pipe.h"

namespace ti
{
	class BlobInputPipe;
	typedef SharedPtr<BlobInputPipe> SharedBlobInputPipe;
	
	class BlobInputPipe : public InputPipe
	{
		public:
			BlobInputPipe();
			
			virtual SharedPtr<Blob> Read(int bufsize=-1);
			virtual void Close();
			virtual bool IsClosed();
			
			int GetSize();
			const char* GetBuffer();
			void Append(SharedPtr<Blob> blob);
			
		protected:
			Poco::Mutex mutex;
			std::vector<char> buffer;
			bool closed;
	};
}

#endif