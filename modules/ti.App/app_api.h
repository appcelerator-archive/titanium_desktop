/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license. 
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */
#ifndef _APP_API__H_
#define _APP_API__H_

#ifdef __cplusplus
#include <kroll/kroll.h>
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <algorithm>
#endif

#if defined(OS_OSX) || defined(OS_LINUX)
#define EXPORT __attribute__((visibility("default")))
#define TITANIUM_APP_API EXPORT
#elif defined(OS_WIN32)
# ifdef TITANIUM_APP_API_EXPORT
#  define TITANIUM_APP_API __declspec(dllexport)
# else
#  define TITANIUM_APP_API __declspec(dllimport)
# endif
# define EXPORT __declspec(dllexport)
#endif

#endif
