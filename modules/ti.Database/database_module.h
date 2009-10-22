/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license. 
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef TI_DATABASE_MODULE_H_
#define TI_DATABASE_MODULE_H_

#include <kroll/kroll.h>

#if defined(OS_OSX) || defined(OS_LINUX)
#define EXPORT __attribute__((visibility("default")))
#define TITANIUM_DATABASE_API EXPORT
#elif defined(OS_WIN32)
# ifdef TITANIUM_DATABASE_API_EXPORT
#  define TITANIUM_DATABASE_API __declspec(dllexport)
# else
#  define TITANIUM_DATABASE_API __declspec(dllimport)
# endif
# define EXPORT __declspec(dllexport)
#endif

namespace ti 
{
	/**
	 * Database module which exposes a synchronous API for Database
	 * access and interoperates with the WebKit Database seamlessly.
	 *
	 * You can interchangeably use the WebKit HTML 5 Database API or
	 * this API on the same database.  Additionally, if you create 
	 * databases in this API, they will be exposed in the Web
	 * Inspector.
	 *
	 */
	class TITANIUM_DATABASE_API DatabaseModule : public kroll::Module, public StaticBoundObject
	{
		KROLL_MODULE_CLASS(DatabaseModule)

	private:
		kroll::KObjectRef binding;
		void Open(const ValueList& args, KValueRef result);
		void OpenFile(const ValueList& args, KValueRef result);
	};

}
#endif
