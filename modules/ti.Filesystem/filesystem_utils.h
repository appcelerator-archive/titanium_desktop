/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#ifndef _TI_FILESYSTEM_UTILS_H_
#define _TI_FILESYSTEM_UTILS_H_

#include <kroll/kroll.h>

namespace ti
{
	namespace FilesystemUtils
	{
		std::string FilenameFromValue(KValueRef);
		std::string FilenameFromArguments(const ValueList& args);
	}
}

#endif
