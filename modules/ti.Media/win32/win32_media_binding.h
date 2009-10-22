/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _MEDIA_WIN32_H_
#define _MEDIA_WIN32_H_

#include <kroll/kroll.h>
#include <windows.h>
#include "../sound.h"
#include "../media_binding.h"
#include "win32_sound.h"

using namespace kroll;

namespace ti
{
	class Win32MediaBinding : public MediaBinding
	{
		public:
		Win32MediaBinding(KObjectRef);
		~Win32MediaBinding();

		protected:
		void Beep();
		KObjectRef CreateSound(std::string&);
	};
}

#endif
