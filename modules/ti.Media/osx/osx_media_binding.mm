/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */
#include "osx_media_binding.h"

namespace ti
{
	OSXMediaBinding::OSXMediaBinding(SharedKObject global) : MediaBinding(global)
	{
	}
	OSXMediaBinding::~OSXMediaBinding()
	{
	}
	void OSXMediaBinding::Beep()
	{
		NSBeep();
	}
	SharedKObject OSXMediaBinding::CreateSound(std::string& url)
	{
		return new OSXSound(url);
	}
}
