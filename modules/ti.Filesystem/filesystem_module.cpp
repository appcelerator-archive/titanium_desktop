/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#include <kroll/kroll.h>
#include "filesystem_module.h"
#include "filesystem_binding.h"

using namespace kroll;
using namespace ti;

namespace ti
{
	KROLL_MODULE(FilesystemModule, STRING(MODULE_NAME), STRING(MODULE_VERSION));

	void FilesystemModule::Initialize()
	{
		// load our variables
		this->binding= new FilesystemBinding(host,host->GetGlobalObject());

		// set our ti.Filesystem
		KValueRef value = Value::NewObject(this->binding);
		host->GetGlobalObject()->Set("Filesystem",value);
	}

	void FilesystemModule::Stop()
	{

	}

}
