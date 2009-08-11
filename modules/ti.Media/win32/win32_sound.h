/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _MEDIA_WIN32_SOUND_H_
#define _MEDIA_WIN32_SOUND_H_

#include <kroll/kroll.h>
#include <vector>
#include <dshow.h>
#include "../sound.h"

using namespace kroll;

namespace ti
{
	class Win32Sound : public Sound
	{
	public:
		Win32Sound(std::string& url);
		virtual ~Win32Sound();

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

		void LoadFile();
		static UINT graphNotifyMessage;
		bool GraphCallback(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
		static bool StaticGraphCallback(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

	private:
		bool looping;
		SharedKMethod callback;
		std::string path;
		enum PlayState {
			PLAYING,
			STOPPED,
			PAUSED
		} state;

		IGraphBuilder* graphBuilder;
		IMediaControl* mediaControl;
		IMediaEventEx* mediaEventEx;
		IMediaSeeking* mediaSeeking;
		IBasicAudio* basicAudio;

			
		void Completed();
		void InitGraphBuilder();
	};
}

#endif
