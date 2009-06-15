/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _OSX_PIPE_H_
#define _OSX_PIPE_H_
#include <kroll/kroll.h>
#include <Cocoa/Cocoa.h>

namespace ti
{
	class OSXPipe : public StaticBoundObject
	{
	public:
		OSXPipe(NSFileHandle *pipe);
		virtual ~OSXPipe();
	private:
		NSFileHandle *handle;
		NSMutableString *data;
		bool closed;

	public:
		
		void OnData(NSString *data);
		
		void Write(const ValueList& args, SharedValue result);
		void Read(const ValueList& args, SharedValue result);
		void Close(const ValueList& args, SharedValue result);
		void Close();
		
		NSString* GetData();
	};
}

#endif
