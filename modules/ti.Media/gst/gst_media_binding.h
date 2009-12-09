/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _GST_MEDIA_BINDING_H_
#define _GST_MEDIA_BINDING_H_

#include <kroll/kroll.h>
#include <gst/gst.h>
#include "../sound.h"
#include "../media_binding.h"
#include "gst_sound.h"

using namespace kroll;

namespace ti
{
	class GstMediaBinding : public MediaBinding
	{
		public:
		GstMediaBinding(KObjectRef);
		~GstMediaBinding();

		protected:
		void Beep();
		KObjectRef CreateSound(std::string&);

	};
}

#endif
