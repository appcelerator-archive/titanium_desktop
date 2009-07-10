/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _PIPE_H_
#define _PIPE_H_

#include <kroll/kroll.h>

namespace ti
{
	class Pipe;
	typedef AutoPtr<Pipe> SharedPipe;
	
	class Pipe : public StaticBoundObject
	{
	public:
		Pipe(const char *type = "Pipe");
		virtual ~Pipe() {};
	
		virtual void Close() = 0;
		virtual bool IsClosed() = 0;
		
		void Closed();
		void SetOnClose(SharedKMethod onClose);
		
	protected:
		void _Close(const ValueList& args, SharedValue result);
		void _IsClosed(const ValueList& args, SharedValue result);
		void _SetOnClose(const ValueList& args, SharedValue result);
		
		SharedKMethod *onClose;
	};
}

#endif
