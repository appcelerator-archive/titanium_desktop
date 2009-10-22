/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef TI_DESKTOP_MODULE_H_
#define TI_DESKTOP_MODULE_H_

#include <kroll/kroll.h>

#if defined(OS_OSX) || defined(OS_LINUX)
#define EXPORT __attribute__((visibility("default")))
#define TITANIUM_DESKTOP_API EXPORT
#elif defined(OS_WIN32)
# ifdef TITANIUM_DESKTOP_API_EXPORT
#  define TITANIUM_DESKTOP_API __declspec(dllexport)
# else
#  define TITANIUM_DESKTOP_API __declspec(dllimport)
# endif
# define EXPORT __declspec(dllexport)
#endif

namespace ti
{
	class TITANIUM_DESKTOP_API DesktopModule : public kroll::Module
	{
		KROLL_MODULE_CLASS(DesktopModule)

	private:
		KObjectRef variables;
	};

}
#endif
