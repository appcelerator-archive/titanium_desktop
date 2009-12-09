/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#ifndef _TI_FILESYSTEM_UTILS_H_
#define _TI_FILESYSTEM_UTILS_H_

#include <kroll/kroll.h>

#ifdef OS_WIN32
#include <windows.h>
#elif OS_OSX
#import <Foundation/Foundation.h>
#endif

#include "file.h"
#include <string>
#include <Poco/FileStream.h>

namespace ti
{
	class FileSystemUtils
	{
		public:
			
			static SharedString GetFileName(KValueRef);
			static AutoPtr<File> ToFile(KObjectRef object);
			
		private:
			FileSystemUtils();
			virtual ~FileSystemUtils();
	};
}

#endif
