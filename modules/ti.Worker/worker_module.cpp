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
		GlobalObject::GetInstance()->SetObject("Worker", new WorkerBinding());
	}

	void WorkerModule::Stop()
	{
		GlobalObject::GetInstance()->SetNull("Worker");
	}
}
