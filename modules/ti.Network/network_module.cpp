/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */
#include "network_module.h"
#include "network_binding.h"

using namespace kroll;
using namespace ti;


namespace ti
{
	KROLL_MODULE(NetworkModule, STRING(MODULE_NAME), STRING(MODULE_VERSION));

	void NetworkModule::Initialize()
	{
		// load our variables
		this->variables = new NetworkBinding(host,GetPath());

		// set our ti.Network
		KValueRef value = Value::NewObject(this->variables);
		host->GetGlobalObject()->Set("Network", value);
	}

	void NetworkModule::Stop()
	{
		AutoPtr<NetworkBinding> b = this->variables.cast<NetworkBinding>();
		b->Shutdown();
	}
	
}
