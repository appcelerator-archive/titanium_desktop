/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */
#include "sound.h"

namespace ti
{
	Sound::Sound(std::string& url) :
		StaticBoundObject("Media.Sound"),
		state(STOPPED),
		url(url),
		path(URLUtils::URLToPath(url)),
		callback(0),
		looping(false)
	{
		/**
		 * @tiapi(method=True,name=Media.Sound.play,since=0.2)
		 * @tiapi Play a sound object. If the sound is paused, it will resume
		 * @tiapi from the current location.
		 */
		this->SetMethod("play", &Sound::Play);

		/**
		 * @tiapi(method=True,name=Media.Sound.pause,since=0.2)
		 * @tiapi Pause a currently playing sound.
		 */
		this->SetMethod("pause", &Sound::Pause);

		/**
		 * @tiapi(method=True,name=Media.Sound.stop,since=0.2)
		 * @tiapi Stop a currently playing sound.
		 */
		this->SetMethod("stop", &Sound::Stop);

		/**
		 * @tiapi(method=True,name=Media.Sound.reload,since=0.2)
		 * @tiapi Reload this sound from the original URL.
		 */
		this->SetMethod("reload", &Sound::Reload);

		/**
		 * @tiapi(method=True,name=Media.Sound.setVolume,since=0.2)
		 * @tiapi Set the volume of this sound.
		 * @tiarg[Number, volume] The new volume value of the sound between 0.0 to 1.0
		 */
		this->SetMethod("setVolume", &Sound::SetVolume);

		/**
		 * @tiapi(method=True,name=Media.Sound.getVolume,since=0.2)
		 * @tiresult[Number] The current volume of the sound.
		 */
		this->SetMethod("getVolume", &Sound::GetVolume);

		/**
		 * @tiapi(method=True,name=Media.Sound.setLooping,since=0.2)
		 * Set whether or not this sound should loop.
		 * @tiarg[Boolean, looping] True if the sound should loop, false otherwise.
		 */
		this->SetMethod("setLooping", &Sound::SetLooping);

		/**
		 * @tiapi(method=True,name=Media.Sound.isLooping,since=0.2) 
		 * @tiresult[Boolean] True if the sound is looping, false otherwise.
		 */
		this->SetMethod("isLooping", &Sound::IsLooping);

		/**
		 * @tiapi(method=True,name=Media.Sound.isPlaying,since=0.2)
		 * @tiresult[Boolean] True if the sound is playing, false otherwise.
		 */
		this->SetMethod("isPlaying", &Sound::IsPlaying);

		/**
		 * @tiapi(method=True,name=Media.Sound.isPaused,since=0.2)
		 * @tiresult[Boolean] True if the sound is paused, false otherwise.
		 */
		this->SetMethod("isPaused", &Sound::IsPaused);

		/**
		 * @tiapi(method=True,name=Media.Sound.onComplete,since=0.2)
		 * Set a callback to be fired fired when the sound finishes playing. If
		 * this sound is looping this callback will be fired after every iteration.
		 * @tiarg[Function, callback|null] The new callback function or null to unset it.
		 */
		this->SetMethod("onComplete", &Sound::SetOnComplete);
	}

	Sound::~Sound()
	{
	}

	void Sound::Play(const ValueList& args, KValueRef result)
	{
		this->Play();
	}

	void Sound::Pause(const ValueList& args, KValueRef result)
	{
		this->Pause();
	}

	void Sound::Stop(const ValueList& args, KValueRef result)
	{
		this->Stop();
	}

	void Sound::Reload(const ValueList& args, KValueRef result)
	{
		this->Reload();
	}

	void Sound::SetVolume(const ValueList& args, KValueRef result)
	{
		args.VerifyException("setVolume", "n");
		double volume = args.GetDouble(0);

		this->SetVolume(volume);
	}

	void Sound::GetVolume(const ValueList& args, KValueRef result)
	{
		result->SetDouble(this->GetVolume());
	}

	void Sound::SetLooping(const ValueList& args, KValueRef result)
	{
		args.VerifyException("setLooping", "b");
		this->SetLooping(args.at(0)->ToBool());
	}

	void Sound::IsLooping(const ValueList& args, KValueRef result)
	{
		result->SetBool(this->IsLooping());
	}

	void Sound::IsPlaying(const ValueList& args, KValueRef result)
	{
		result->SetBool(this->IsPlaying());
	}

	void Sound::IsPaused(const ValueList& args, KValueRef result)
	{
		result->SetBool(this->IsPaused());
	}

	void Sound::SetOnComplete(const ValueList& args, KValueRef result)
	{
		args.VerifyException("onComplete", "m|0");
		this->SetOnComplete(args.GetMethod(0, 0));
	}

	void Sound::Play()
	{
		this->PlayImpl();
		this->state = PLAYING;
	}

	void Sound::Pause()
	{
		if (this->state == END_OF_ITERATION || this->state == PLAYING)
		{
			this->PauseImpl();
			this->state = PAUSED;
		}
		else
		{
			throw ValueException::FromString("Sounds is not currently playing");
		}
	}

	void Sound::Stop()
	{
		if (this->state == END_OF_ITERATION ||
			this->state == PLAYING || this->state == PAUSED)
		{
			this->StopImpl();
		}

		this->state = STOPPED;
	}

	void Sound::Reload()
	{
		this->Unload();
		this->Load();
	}

	void Sound::SetVolume(double newVolume)
	{
		if (newVolume < 0)
			newVolume = 0;
		if (newVolume > 1)
			newVolume = 1;

		this->SetVolumeImpl(newVolume);
	}

	double Sound::GetVolume()
	{
		return this->GetVolumeImpl();
	}

	void Sound::SetLooping(bool newLooping)
	{
		this->looping = newLooping;
	}

	bool Sound::IsLooping()
	{
		return looping;
	}

	bool Sound::IsPlaying()
	{
		return state == PLAYING;
	}

	bool Sound::IsPaused()
	{
		return state == PAUSED;
	}

	void Sound::SetOnComplete(KMethodRef newCallback)
	{
		this->callback = newCallback;
	}

	void Sound::Load()
	{
		this->Unload();
		this->LoadImpl();
	}

	void Sound::Unload()
	{
		this->Stop();
		this->UnloadImpl();
	}

	void Sound::SoundCompletedIteration()
	{
		this->state = END_OF_ITERATION;

		if (!this->callback.isNull())
		{
			try
			{
				ValueList args(Value::NewBool(this->IsLooping()));
				this->callback->Call(args);
			}
			catch (ValueException& e)
			{
				Logger::Get("Media.Sound")->Error(
					"Error during sound onComplete callback: %s",
					e.ToString().c_str());
			}
		}

		if (this->IsLooping() && this->state != STOPPED)
		{
			this->Stop();
			this->Play();

			if (this->state == PAUSED)
				this->Pause();
		}
	}
}
