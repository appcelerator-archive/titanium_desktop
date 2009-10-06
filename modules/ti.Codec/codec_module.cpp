/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license. 
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#include <kroll/kroll.h>
#include "codec_module.h"
#include "codec_binding.h"

using namespace kroll;
using namespace ti;

namespace ti
{
	KROLL_MODULE(CodecModule, STRING(MODULE_NAME), STRING(MODULE_VERSION));
	
	void CodecModule::Initialize()
	{
		// load our variables
		this->binding = new CodecBinding(host->GetGlobalObject());

		// set our ti.Codec
		SharedValue value = Value::NewObject(this->binding);
		host->GetGlobalObject()->Set("Codec", value);
	}

	void CodecModule::Stop()
	{
	}
}
