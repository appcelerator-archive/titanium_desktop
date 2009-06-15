/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _MEDIA_SOUND_H_
#define _MEDIA_SOUND_H_
#include <kroll/kroll.h>

using namespace kroll;

namespace ti
{
	class Sound : public StaticBoundObject
	{
	public:
		Sound(std::string& url);
		virtual ~Sound();

		// bound object API
		void Play(const ValueList& args, SharedValue result);
		void Pause(const ValueList& args, SharedValue result);
		void Stop(const ValueList& args, SharedValue result);
		void Reload(const ValueList& args, SharedValue result);
		void SetVolume(const ValueList& args, SharedValue result);
		void GetVolume(const ValueList& args, SharedValue result);
		void SetLooping(const ValueList& args, SharedValue result);
		void IsLooping(const ValueList& args, SharedValue result);
		void IsPlaying(const ValueList& args, SharedValue result);
		void IsPaused(const ValueList& args, SharedValue result);
		void OnComplete(const ValueList& args, SharedValue result);

		// main API
		virtual void Play() = 0;
		virtual void Pause() = 0;
		virtual void Stop() = 0;
		virtual void Reload() = 0;
		virtual void SetVolume(double volume) = 0;
		virtual double GetVolume() = 0;
		virtual void SetLooping(bool loop) = 0;
		virtual bool IsLooping() = 0;
		virtual bool IsPlaying() = 0;
		virtual bool IsPaused() = 0;
		virtual void OnComplete(SharedKMethod callback) = 0;
		
	protected:
		std::string url;
	};
}

#endif
