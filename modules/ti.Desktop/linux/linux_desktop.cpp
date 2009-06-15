/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */
#include "linux_desktop.h"
#include <Poco/Process.h>
using Poco::Process;

namespace ti
{
	LinuxDesktop::LinuxDesktop()
	{
	}

	LinuxDesktop::~LinuxDesktop()
	{
	}

	bool LinuxDesktop::OpenApplication(std::string &name)
	{
		std::vector<std::string> args;
		Process::launch(name, args);
		return true;
	}

	bool LinuxDesktop::OpenURL(std::string &url)
	{
		std::vector<std::string> args;
		args.push_back(url);
		std::string name = "xdg-open";
		Process::launch(name, args);
		return true;
	}

}
