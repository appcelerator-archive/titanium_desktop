/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license. 
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef TI_NETWORK_MODULE_H_
#define TI_NETWORK_MODULE_H_

#include <kroll/kroll.h>
#include <curl/curl.h>

#if defined(OS_OSX) || defined(OS_LINUX)
#define EXPORT __attribute__((visibility("default")))
#define TITANIUM_NETWORK_API EXPORT
#elif defined(OS_WIN32)
# ifdef TITANIUM_NETWORK_API_EXPORT
#  define TITANIUM_NETWORK_API __declspec(dllexport)
# else
#  define TITANIUM_NETWORK_API __declspec(dllimport)
# endif
# define EXPORT __declspec(dllexport)
#endif


namespace ti 
{
	class TITANIUM_NETWORK_API NetworkModule : public kroll::Module
	{
		KROLL_MODULE_CLASS(NetworkModule)

	public:
		static std::string& GetRootCertPath();
		static CURLSH* GetCurlShareHandle();

	private:
		KObjectRef variables;
	};

}
#endif
