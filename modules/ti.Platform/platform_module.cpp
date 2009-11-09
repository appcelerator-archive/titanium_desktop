/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license. 
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#include <kroll/kroll.h>
#include "platform_module.h"
#include "platform_binding.h"

using namespace kroll;
using namespace ti;

namespace ti
{
	KROLL_MODULE(PlatformModule, STRING(MODULE_NAME), STRING(MODULE_VERSION));
	
	void PlatformModule::Initialize()
	{
		// load our variables
		this->binding = new PlatformBinding(host->GetGlobalObject());

		// set our ti.Platform
		KValueRef value = Value::NewObject(this->binding);
		host->GetGlobalObject()->Set("Platform", value);
	}

	void PlatformModule::Stop()
	{
	}
	
}
