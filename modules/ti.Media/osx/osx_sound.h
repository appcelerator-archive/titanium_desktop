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
		virtual void LoadImpl();
		virtual void UnloadImpl();
		virtual void PlayImpl();
		virtual void PauseImpl();
		virtual void StopImpl();
		virtual void SetVolumeImpl(double volume);
		virtual double GetVolumeImpl();

		private:
		NSSound* sound;
		id delegate;
		NSURL* fileURL;
	};
}

#endif
