/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license. 
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#include <kroll/kroll.h>
#include "monkey_module.h"
#include "monkey_binding.h"

using namespace kroll;
using namespace ti;

namespace ti
{
	KROLL_MODULE(MonkeyModule, STRING(MODULE_NAME), STRING(MODULE_VERSION));
	
	void MonkeyModule::Initialize()
	{
		// load our variables
		this->binding = new MonkeyBinding(host,host->GetGlobalObject());

		// set our ti.Monkey
		KValueRef value = Value::NewObject(this->binding);
		host->GetGlobalObject()->Set("Monkey", value);
	}

	void MonkeyModule::Stop()
	{
	}
}
