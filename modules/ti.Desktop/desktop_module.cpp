/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */
#include "desktop_module.h"
#include "desktop_binding.h"

using namespace kroll;
using namespace ti;


namespace ti
{
	KROLL_MODULE(DesktopModule, STRING(MODULE_NAME), STRING(MODULE_VERSION));

	void DesktopModule::Initialize()
	{
		// load our variables
		this->variables = new DesktopBinding(host->GetGlobalObject());

		// set our ti.Desktop
		KValueRef value = Value::NewObject(this->variables);
		host->GetGlobalObject()->Set("Desktop",value);
	}

	void DesktopModule::Stop()
	{
	}
}
