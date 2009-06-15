/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#include "gst_media_binding.h"

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
		printf("\a"); // \a to console should cause the beep sound
	}

	SharedKObject GstMediaBinding::CreateSound(std::string& url)
	{
		//This is a path so, turn it into a file:// URL
		std::string url_or_path = this->GetResourcePath(url.c_str());
		if (url_or_path.find(':') == std::string::npos)
		{
			url_or_path = std::string("file://") + url_or_path;
		}
		SharedGstSound sound = new GstSound(url_or_path);
		GstSound::RegisterSound(sound);
		return sound;
	}
}
