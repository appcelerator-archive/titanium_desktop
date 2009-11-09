/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#include <kroll/kroll.h>
#include "growl_module.h"
#include "growl_binding.h"

#if defined(OS_OSX)
#include "osx/growl_osx.h"
#elif defined(OS_WIN32)
#include "win32/snarl_win32.h"
#elif defined(OS_LINUX)
#include "libnotify/libnotify_binding.h"
#endif

using namespace kroll;
using namespace ti;

namespace ti
{
	KROLL_MODULE(GrowlModule, STRING(MODULE_NAME), STRING(MODULE_VERSION));

	void GrowlModule::Initialize()
	{

#if defined(OS_OSX)
		// load our variables
		GrowlOSX *g = new GrowlOSX(host->GetGlobalObject());
		binding = g;
		g->CopyToApp(host,this);
#elif defined(OS_WIN32)
		binding = new SnarlWin32(host->GetGlobalObject());
#elif defined(OS_LINUX)
		binding = new LibNotifyBinding(host->GetGlobalObject());
#endif

		// set our ti.Growl
		KValueRef value = Value::NewObject(binding);
		host->GetGlobalObject()->Set("Growl", value);
	}

	void GrowlModule::Stop()
	{
	}

}
