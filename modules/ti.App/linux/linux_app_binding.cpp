/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#include <kroll/kroll.h>
#include "../app_binding.h"

namespace ti
{

void AppBinding::Restart(const ValueList& args, KValueRef result)
{
	Host* host = Host::GetInstance();
	std::string cmdline(host->GetApplication()->arguments.at(0));

	// Remove all "'s
	while ((size_t idx = cmdline.find('\"')) != std::string::npos)
		cmdline.replace(idx, 1, "");

	std::string script = "\"" + cmdline + "\" &";
	system(script.c_str());
}

}
