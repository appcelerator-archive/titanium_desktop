/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license. 
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#include <kroll/kroll.h>
#include "worker_module.h"
#include "worker_binding.h"

using namespace kroll;
using namespace ti;

KROLL_MODULE(WorkerModule, STRING(MODULE_NAME), STRING(MODULE_VERSION));

namespace ti
{
	void WorkerModule::Initialize()
	{
		// load our variables
		this->binding = new WorkerBinding(host,host->GetGlobalObject());

		// set our ti.Worker
		KValueRef value = Value::NewObject(this->binding);
		host->GetGlobalObject()->Set("Worker", value);
	}

	void WorkerModule::Stop()
	{
	}
	
}
