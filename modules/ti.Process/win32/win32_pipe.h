/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _WIN32_PIPE_H_
#define _WIN32_PIPE_H_

#include <kroll/kroll.h>

namespace ti
{
	class Win32Pipe : public StaticBoundObject
	{
	public:
		Win32Pipe();
		virtual ~Win32Pipe();
		
		HANDLE GetReadHandle() { return read; }
		HANDLE GetWriteHandle() { return write; }
		
		int Read(char *buffer, int size);
		int Write(char *buffer, int length);
		
	private:
		HANDLE read, write;
		bool closed;

	public:
		void Write(const ValueList& args, SharedValue result);
		void Read(const ValueList& args, SharedValue result);
		void Close(const ValueList& args, SharedValue result);
		void Close();
	};
}

#endif
