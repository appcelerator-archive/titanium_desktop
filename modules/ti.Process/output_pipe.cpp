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
	AutoOutputPipe OutputPipe::CreateOutputPipe()
	{
#if defined(OS_OSX)
		AutoOutputPipe pipe = new OSXOutputPipe();
#elif defined(OS_WIN32)
		AutoOutputPipe pipe = new Win32OutputPipe();
#elif defined(OS_LINUX)
		AutoOutputPipe pipe = new LinuxOutputPipe();
#endif
		return pipe;
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
		AutoPtr<Blob> blob = new Blob();
		if (args.at(0)->IsObject())
		{
			blob = args.at(0)->ToObject().cast<Blob>();
		}
		else if (args.at(0)->IsString())
		{
			blob = new Blob(args.at(0)->ToString());
		}
		
		if (blob.isNull())
		{
			throw ValueException::FromString("OutputPipe.write argument should be a Blob or string");
		}
		
		int written = this->Write(blob);
		result->SetInt(written);
		
	}
}