/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license. 
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#include <kroll/kroll.h>
#include "process_module.h"
#include "process_binding.h"

using namespace kroll;
using namespace ti;

namespace ti
{
	KROLL_MODULE(ProcessModule, STRING(MODULE_NAME), STRING(MODULE_VERSION));
	
	void ProcessModule::Initialize()
	{
		// load our variables
		this->binding = new ProcessBinding();

		// set our ti.Process
		KValueRef value = Value::NewObject(this->binding);
		host->GetGlobalObject()->Set("Process", value);

// simple test to make the app crash
#ifdef TEST_CRASH_DETECTION		
		char *s = NULL;
		(*s)++;
#endif
	}

	void ProcessModule::Stop()
	{
	}
	
}
