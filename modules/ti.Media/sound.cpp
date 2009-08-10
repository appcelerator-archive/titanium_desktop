/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */
#include "sound.h"

namespace ti
{
	Sound::Sound (std::string &s) : StaticBoundObject("Sound"), url(s)
	{
		/**
		 * @tiapi(method=True,name=Media.Sound.play,since=0.2) Plays the file referenced by a Sound object
		 */
		this->SetMethod("play",&Sound::Play);
		/**
		 * @tiapi(method=True,name=Media.Sound.pause,since=0.2) Pauses a currently playing Sound object
		 */
		this->SetMethod("pause",&Sound::Pause);
		/**
		 * @tiapi(method=True,name=Media.Sound.stop,since=0.2) Stops a currently playing Sound object
		 */
		this->SetMethod("stop",&Sound::Stop);
		/**
		 * @tiapi(method=True,name=Media.Sound.reload,since=0.2) Reloads the file referenced by a Sound object
		 */
		this->SetMethod("reload",&Sound::Reload);
		/**
		 * @tiapi(method=True,name=Media.Sound.setVolume,since=0.2) Sets the volume value of a Sound object
		 * @tiarg(for=Media.Sound.setVolume,type=Number,name=volume) the volume value of the sound, from 0.0 to 1.0
		 */
		this->SetMethod("setVolume",&Sound::SetVolume);
		/**
		 * @tiapi(method=True,name=Media.Sound.getVolume,since=0.2) Returns the volume value of a Sound object
		 * @tiresult(for=Media.Sound.getVolume,type=Number) returns the volume of the sound
		 */
		this->SetMethod("getVolume",&Sound::GetVolume);
		/**
		 * @tiapi(method=True,name=Media.Sound.setLooping,since=0.2) Sets the looping of a Sound object
		 * @tiarg(for=Media.Sound.setLooping,name=loop,type=Boolean) true to set the Sound object to loop, false to remove looping
		 */
		this->SetMethod("setLooping",&Sound::SetLooping);
		/**
		 * @tiapi(method=True,name=Media.Sound.isLooping,since=0.2) Checks whether a Sound object is set to loop
		 * @tiresult(for=Media.Sound.isLooping,type=Boolean) true if the Sound object is set to loop, false if otherwise
		 */
		this->SetMethod("isLooping",&Sound::IsLooping);
		/**
		 * @tiapi(method=True,name=Media.Sound.isPlaying,since=0.2) Checks whether a Sound object is currently playing
		 * @tiresult(for=Media.Sound.isPlaying,type=Boolean) returns true if the Sound object is currently playing, false if otherwise
		 */
		this->SetMethod("isPlaying",&Sound::IsPlaying);
		/**
		 * @tiapi(method=True,name=Media.Sound.isPaused,since=0.2) Checks whether a Sound object is paused
		 * @tiresult(for=Media.Sound.isPaused,type=Boolean) returns true if the Sound object is pause, false if otherwise
		 */
		this->SetMethod("isPaused",&Sound::IsPaused);
		/**
		 * @tiapi(method=True,name=Media.Sound.onComplete,since=0.2) Sets an onComplete function that is fired when a sound finished playing
		 * @tiarg(for=Media.Sound.onComplete,type=Function,name=callback) a function to play when the Sound object finishes playing
		 */
		this->SetMethod("onComplete",&Sound::OnComplete);
	}
	Sound::~Sound()
	{
	}
	void Sound::Play(const ValueList& args, SharedValue result)
	{
		this->Play();
	}
	void Sound::Pause(const ValueList& args, SharedValue result)
	{
		if (!this->IsPlaying())
		{
			throw ValueException::FromString("Sounds is not currently playing");
		}
		this->Pause();
	}
	void Sound::Stop(const ValueList& args, SharedValue result)
	{
		if (!this->IsPlaying() && !this->IsPaused())
		{
			return;
		}
		this->Stop();
	}
	void Sound::Reload(const ValueList& args, SharedValue result)
	{
		this->Reload();
	}
	void Sound::SetVolume(const ValueList& args, SharedValue result)
	{
		args.VerifyException("setVolume", "n");
		double volume = args.GetDouble(0);

		if (volume < 0)
			volume = 0;
		if (volume > 1)
			volume = 1;

		this->SetVolume(volume);
	}
	void Sound::GetVolume(const ValueList& args, SharedValue result)
	{
		result->SetDouble(this->GetVolume());
	}
	void Sound::SetLooping(const ValueList& args, SharedValue result)
	{
		if (args.size()!=1)
		{
			throw ValueException::FromString("setLooping takes 1 parameter");
		}
		this->SetLooping(args.at(0)->ToBool());
	}
	void Sound::IsLooping(const ValueList& args, SharedValue result)
	{
		result->SetBool(this->IsLooping());
	}
	void Sound::IsPlaying(const ValueList& args, SharedValue result)
	{
		result->SetBool(this->IsPlaying());
	}
	void Sound::IsPaused(const ValueList& args, SharedValue result)
	{
		result->SetBool(this->IsPaused());
	}
	void Sound::OnComplete(const ValueList& args, SharedValue result)
	{
		if (args.size()!=1)
		{
			throw ValueException::FromString("onComplete takes 1 parameter");
		}
		if (!args.at(0)->IsMethod())
		{
			throw ValueException::FromString("onComplete takes a function parameter");
		}
		SharedKMethod method = args.at(0)->ToMethod();
		this->OnComplete(method);
	}
}
