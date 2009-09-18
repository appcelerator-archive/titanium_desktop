/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#include "gst_media_binding.h"
#include <gdk/gdk.h>

namespace ti
{
	GstMediaBinding::GstMediaBinding(SharedKObject global) : MediaBinding(global)
	{
		char **argv;
		int argc = 0;
		gst_init(&argc, &argv);
	}

	GstMediaBinding::~GstMediaBinding()
	{
	}

	void GstMediaBinding::Beep()
	{
		gdk_beep();
	}

	SharedKObject GstMediaBinding::CreateSound(std::string& url)
	{
		//This is a path so, turn it into a file:// URL
		std::string myurl = url;
		std::string path = URLUtils::URLToPath(url);
		if (path.find("://") == std::string::npos)
		{
			myurl = URLUtils::PathToFileURL(path);
		}
		return new GstSound(myurl);
	}
}
