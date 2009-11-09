/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license. 
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef TI_MEDIA_MODULE_H_
#define TI_MEDIA_MODULE_H_

#include <kroll/kroll.h>

#if defined(OS_OSX) || defined(OS_LINUX)
#define EXPORT __attribute__((visibility("default")))
#define TITANIUM_API EXPORT
#elif defined(OS_WIN32)
# ifdef TITANIUM_API_EXPORT
#  define TITANIUM_API __declspec(dllexport)
# else
#  define TITANIUM_API __declspec(dllimport)
# endif
# define EXPORT __declspec(dllexport)
#endif

namespace ti 
{
	class TITANIUM_API MediaModule : public kroll::Module
	{
		KROLL_MODULE_CLASS(MediaModule)

	private:
		KObjectRef binding;
	};

}

#endif
