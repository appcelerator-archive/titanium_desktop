/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */
#include "desktop_binding.h"
#include <kroll/kroll.h>

#ifdef OS_OSX
#include "osx/osx_desktop.h"
#elif defined(OS_WIN32)
#include "win32/win32_desktop.h"
#elif defined(OS_LINUX)
#include "linux/linux_desktop.h"
#endif

#if defined(OS_OSX)
	#define TI_DESKTOP OSXDesktop
#elif defined(OS_WIN32)
	#define TI_DESKTOP Win32Desktop
#elif defined(OS_LINUX)
	#define TI_DESKTOP LinuxDesktop
#endif

namespace ti
{
	DesktopBinding::DesktopBinding(KObjectRef global) : global(global)
	{
		/**
		 * @tiapi(method=True,name=Desktop.openApplication,since=0.2) Opens an application on the users system
		 * @tiarg(for=Desktop.openApplication,type=String,name=path) the name or path to the application to open
		 */
		this->SetMethod("openApplication",&DesktopBinding::OpenApplication);
		/**
		 * @tiapi(method=True,name=Desktop.openURL,since=0.2) Opens a URL in the default system browser
		 * @tiarg(for=Desktop.openURL,name=url,type=String) the url
		 */
		this->SetMethod("openURL",&DesktopBinding::OpenURL);
		/**
		 * @tiapi(method=True,name=Desktop.takeScreenshot,since=0.4) Takes a screenshot of the users desktop and saves it to a file
		 * @tiarg(for=Desktop.takeScreenshot,name=file,type=String) file to write the output
		 */
		this->SetMethod("takeScreenshot", &DesktopBinding::TakeScreenshot);
	}

	DesktopBinding::~DesktopBinding()
	{
	}

	void DesktopBinding::OpenApplication(const ValueList& args, KValueRef result)
	{
		if (args.size()!=1)
		{
			throw ValueException::FromString("openApplication takes 1 parameter");
		}
		std::string app = args.at(0)->ToString();
		result->SetBool(TI_DESKTOP::OpenApplication(app));
	}

	void DesktopBinding::OpenURL(const ValueList& args, KValueRef result)
	{
		if (args.size()!=1)
		{
			throw ValueException::FromString("openURL takes 1 parameter");
		}
		std::string url = args.at(0)->ToString();
		result->SetBool(TI_DESKTOP::OpenURL(url));
	}

	void DesktopBinding::TakeScreenshot(const ValueList& args, KValueRef result)
	{
		if (args.size() < 1) {
			throw ValueException::FromString("takeScreenshot takes 1 parameter");
		}

		std::string filePath = args.at(0)->ToString();

#ifdef OS_WIN32
		TI_DESKTOP::TakeScreenshot(filePath);
#elif OS_OSX
		TI_DESKTOP::TakeScreenshot(filePath);
#elif OS_LINUX
		// TODO - implement for linux
#endif
	}
}
