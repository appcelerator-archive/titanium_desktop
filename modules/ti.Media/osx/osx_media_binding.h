/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _OSX_MEDIA_BINDING_H_
#define _OSX_MEDIA_BINDING_H_

#include <kroll/kroll.h>
#import <Cocoa/Cocoa.h>
#include "../sound.h"
#include "../media_binding.h"
#include "osx_sound.h"

using namespace kroll;

namespace ti
{
	class OSXMediaBinding : public MediaBinding
	{
		public:
		OSXMediaBinding(KObjectRef);
		~OSXMediaBinding();

		protected:
		void Beep();
		KObjectRef CreateSound(std::string&);
	};
}

#endif
