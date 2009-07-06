/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#include "output_pipe.h"
#if defined(OS_OSX)
# include "osx/osx_output_pipe.h"
#elif defined(OS_WIN32)
# include "win32/win32_output_pipe.h"
#elif defined(OS_LINUX)
# include "linux/linux_output_pipe.h"
#endif

namespace ti
{
	/*static*/
	SharedOutputPipe OutputPipe::CreateOutputPipe()
	{
#if defined(OS_OSX)
		return new OSXOutputPipe();
#elif defined(OS_WIN32)
		return new Win32OutputPipe();
#elif defined(OS_LINUX)
		return new LinuxOutputPipe();
#endif
	}
	
	OutputPipe::OutputPipe()
	{
		//TODO doc me
		SetMethod("write", &OutputPipe::_Write);
	}
	
	void OutputPipe::_Write(const ValueList& args, SharedValue result)
	{
		if (args.size() == 0)
		{
			throw ValueException::FromString("No data passed to write");
		}
		else if (!args.at(0)->IsObject())
		{
			throw ValueException::FromString("First argument should be Blob");
		}
		
		SharedPtr<Blob> blob = args.at(0)->ToObject().cast<Blob>();
		int size = -1;
		
		if (args.size() > 1 && args.at(1)->IsNumber())
		{
			size = args.at(1)->ToInt();
		}
		
		int written = this->Write(blob, size);
		result->SetInt(written);
	}
}