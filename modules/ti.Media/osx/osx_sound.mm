/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */

#import "osx_sound.h"
#import "sound_delegate.h"

@interface NSSound (MakeTheErrorsGoAway)
//These all are in 10.5, but building for 10.4 produces warnings unless I mention them here.
- (void)setVolume:(float)volume;
- (float)volume;
- (BOOL)loops;
- (void)setLoops:(BOOL)loops;
@end

namespace ti
{
	OSXSound::OSXSound(std::string &url) : 
		Sound(url),
		callback(0),
		sound(0),
		playing(false),
		paused(false),
		looping(false)
	{
		this->url = [NSURL URLWithString:[NSString stringWithUTF8String:url.c_str()]];
		[this->url retain];
		delegate = [[SoundDelegate alloc] initWithSound:this];
		this->Load();
	}

	OSXSound::~OSXSound()
	{
		this->Unload();
		[delegate release];
		[url release];
	}

	void OSXSound::Unload()
	{
		if (sound)
		{
			if (playing)
				[sound stop];

			[sound setDelegate:nil];
			[sound release];
			sound = NULL;
			playing = false;
			paused = false;
		}
	}

	void OSXSound::Load()
	{
		this->Unload();
		@try
		{
			sound = [[NSSound alloc] initWithContentsOfURL:url byReference:NO];
			[sound setDelegate:delegate];
		}
		@catch(NSException *ex)
		{
			throw ValueException::FromFormat("Error loading (%s): %s",
				[[url absoluteString] UTF8String], [[ex reason] UTF8String]);
		}
		@catch(...)
		{
			throw ValueException::FromFormat("Unknown error loading (%s): %s",
				[[url absoluteString] UTF8String]);
		}
	}

	void OSXSound::Play()
	{
		if (paused)
			[sound resume];
		else
			[sound play];

		playing = true;
		paused = false;
	}

	void OSXSound::Pause()
	{
		[sound pause];
		paused = true;
		playing = false;
	}

	void OSXSound::Stop()
	{
		if (sound && playing)
		{
			[sound stop];
		}
		paused = false;
		playing = false;
	}

	void OSXSound::Reload()
	{
		this->Load();
	}

	void OSXSound::SetVolume(double volume)
	{
		// TODO: 10.4 doesn't have setVolume on NSSound.
		if ([sound respondsToSelector:@selector(setVolume:)])
			[sound setVolume:volume];
	}

	double OSXSound::GetVolume()
	{
		// TODO: 10.4 doesn't have volume on NSSound.
		if ([sound respondsToSelector:@selector(volume)])
			return [sound volume];
		else
			return 0.0;
	}

	void OSXSound::SetLooping(bool loop)
	{
		this->looping = loop;
	}

	bool OSXSound::IsLooping()
	{
		return looping;
	}

	bool OSXSound::IsPlaying()
	{
		return playing;
	}

	bool OSXSound::IsPaused()
	{
		return paused;
	}

	void OSXSound::Complete(bool finished)
	{
		// this is called from SoundDelegate and it
		// will be on the main thread already.
		this->playing = false;
		this->paused = false;

		if (!this->callback.isNull())
		{
			try
			{
				this->callback->Call(Value::NewBool(finished));
			}
			catch (ValueException& e)
			{
				SharedString s = e.GetValue()->DisplayString();
				Logger::Get("Media.Sound")->Error("onComplete callback failed: %s",
					s->c_str());
			}
		}

		if (this->IsLooping())
			this->Play();
	}

	void OSXSound::OnComplete(SharedKMethod callback)
	{
		this->callback = callback;
	}
}
