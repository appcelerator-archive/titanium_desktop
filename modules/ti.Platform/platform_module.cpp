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
		this->binding = new PlatformBinding();
		host->GetGlobalObject()->SetObject("Platform", this->binding);
		host->GetGlobalObject()->SetObject("Desktop", this->binding);
	}

	void PlatformModule::Start()
	{
		// Duplicate the network module address, macaddress and interfaces here for
		// backward compatibility. The network module should be initialized when
		// Start() is called.
		if (!GlobalObject::GetInstance()->GetObject("Network").isNull())
		{
			KObjectRef network = GlobalObject::GetInstance()->GetObject("Network");
			this->binding->Set("getAddress", network->Get("getAddress"));
			this->binding->Set("getMACAddress", network->Get("getMACAddress"));
			this->binding->Set("getInterfaces", network->Get("getInterfaces"));
		}
	}

	void PlatformModule::Stop()
	{
		host->GetGlobalObject()->SetNull("Platform");
		host->GetGlobalObject()->SetNull("Desktop");
	}
	
}
