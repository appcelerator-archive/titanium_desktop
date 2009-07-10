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
		virtual ~GstSound();
		
		virtual void Play();
		virtual void Load();
		virtual void Pause();
		virtual void Stop();
		virtual void Reload();
		virtual void SetVolume(double volume);
		virtual double GetVolume();
		virtual void SetLooping(bool looping);
		virtual bool IsLooping();
		virtual bool IsPlaying();
		virtual bool IsPaused();
		virtual void OnComplete(SharedKMethod callback);
		virtual void Complete();

		static void RegisterSound(SharedGstSound);
		static void UnregisterSound(GstSound*);
		static SharedGstSound GetRegisteredSound(GstSound* sound);

	private:
		SharedKMethod callback;
		GstElement *pipeline;
		bool looping;

		enum PlayState {
			PLAYING,
			STOPPED,
			PAUSED
		} state;

		static std::vector<SharedGstSound> active;
		static Mutex active_mutex;

	};
}

#endif
