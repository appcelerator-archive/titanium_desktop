/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2010 Appcelerator, Inc. All Rights Reserved.
 */
#include <kroll/kroll.h>
#include <Poco/Environment.h>
#include <Poco/Process.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <gdk/gdk.h>
#include "platform_binding.h"

namespace ti
{

std::string PlatformBinding::GetVersionImpl()
{
	return Poco::Environment::osVersion();
}

bool PlatformBinding::OpenApplicationImpl(const std::string& name)
{
	std::vector<std::string> args;
	args.push_back(name);
	std::string processName("xdg-open");
	Poco::Process::launch(processName, args);
	return true;
}

bool PlatformBinding::OpenURLImpl(const std::string& url)
{
	return this->OpenApplicationImpl(url);
}

void PlatformBinding::TakeScreenshotImpl(const std::string& targetFile)
{
	GdkWindow* rootWindow = gdk_get_default_root_window();

	int width, height;
	gdk_drawable_get_size(rootWindow, &width, &height);

	GdkPixbuf* pixbuf = gdk_pixbuf_get_from_drawable(0, GDK_DRAWABLE(rootWindow),
		0, 0, 0, 0, 0, width, height);
	gdk_pixbuf_save(pixbuf, targetFile.c_str(), "png", 0, "compression", "9", NULL);
}

}
