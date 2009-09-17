/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _MEDIA_OSX_SOUND_H_
#define _MEDIA_OSX_SOUND_H_

#include <kroll/kroll.h>
#include <vector>
#include "../sound.h"
#import <Cocoa/Cocoa.h>

using namespace kroll;

namespace ti
{
	class OSXSound : public Sound
	{
		public:
		OSXSound(std::string& url);
		virtual ~OSXSound();

		// called from SoundDelegate callback
		virtual void Complete(bool finished);

		// API delegates
		virtual void Play();
		virtual void Pause();
		virtual void Stop();
		virtual void Reload();
		virtual void SetVolume(double volume);
		virtual double GetVolume();
		virtual void SetLooping(bool loop);
		virtual bool IsLooping();
		virtual bool IsPlaying();
		virtual bool IsPaused();
		virtual void OnComplete(SharedKMethod callback);

		private:
		SharedKMethod callback;
		NSURL* url;
		NSSound* sound;
		id delegate;
		bool playing;
		bool paused;
		bool looping;

		void Load();
		void Unload();
	};
}

#endif
