/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _DESKTOP_LINUX_H_
#define _DESKTOP_LINUX_H_

#include <kroll/kroll.h>
#include <map>
#include <vector>
#include <string>

using namespace kroll;

namespace ti
{
	class LinuxDesktop
	{
	public:
		static bool OpenApplication(std::string &name);
		static bool OpenURL(std::string &url);
	private:
		LinuxDesktop();
		~LinuxDesktop();
	};
}

#endif
