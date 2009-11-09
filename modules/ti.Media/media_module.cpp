/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */
#include "media_module.h"
#include "media_binding.h"

#ifdef OS_LINUX
#include "gst/gst_media_binding.h"
#elif OS_WIN32
#include "win32/win32_media_binding.h"
#elif OS_OSX
#include "osx/osx_media_binding.h"
#endif


using namespace kroll;
using namespace ti;


namespace ti
{
	KROLL_MODULE(MediaModule, STRING(MODULE_NAME), STRING(MODULE_VERSION));

	void MediaModule::Initialize()
	{
#ifdef OS_LINUX
		this->binding = new GstMediaBinding(host->GetGlobalObject());
#elif OS_WIN32
		this->binding = new Win32MediaBinding(host->GetGlobalObject());
#elif OS_OSX
		this->binding = new OSXMediaBinding(host->GetGlobalObject());
#endif
		KValueRef value = Value::NewObject(this->binding);
		host->GetGlobalObject()->Set("Media", value);
	}

	void MediaModule::Stop()
	{
	}
}
