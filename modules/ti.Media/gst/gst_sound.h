/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _TI_MEDIA_GST_SOUND_H_
#define _TI_MEDIA_GST_SOUND_H_

#include <kroll/kroll.h>
#include <vector>

using namespace kroll;

namespace ti
{
	class GstSound;
	typedef AutoPtr<GstSound> SharedGstSound;

	class GstSound : public Sound
	{
	public:
		GstSound(std::string& url);
		virtual void LoadImpl();
		virtual void UnloadImpl();
		virtual void PlayImpl();
		virtual void PauseImpl();
		virtual void StopImpl();
		virtual void SetVolumeImpl(double volume);
		virtual double GetVolumeImpl();

	private:
		GstElement* pipeline;
		guint watchEventSourceId;
		std::string fileURL;
	};
}

#endif
