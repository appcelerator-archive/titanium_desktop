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
		Sound(url), callback(0), sound(0), playing(false), paused(false)
	{
		theurl = [NSURL URLWithString:[NSString stringWithCString:url.c_str() encoding:NSUTF8StringEncoding]];
		[theurl retain];
		delegate = [[SoundDelegate alloc] initWithSound:this];
		this->Load();
	}
	OSXSound::~OSXSound()
	{
		this->Unload();
		[delegate release];
		[theurl release];
		
		if (callback)
		{
			delete callback;
			callback = NULL;
		}
	}
	void OSXSound::Unload()
	{
		if (sound)
		{
			if (playing)
			{
				[sound stop];	
			}
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
			sound = [[NSSound alloc] initWithContentsOfURL:theurl byReference:NO];
			[sound setDelegate:delegate];
		}
		@catch(NSException *ex)
		{
			throw [[ex reason] UTF8String];
		}
		@catch(...)
		{
			throw "error loading media";
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
		if ([sound respondsToSelector:@selector(setVolume:)]){
			[sound setVolume:volume];
		}	//TODO: 10.4 doesn't have setVolume on NSSound.
	}
	double OSXSound::GetVolume()
	{
		if ([sound respondsToSelector:@selector(volume)]){
			return [sound volume];
		}	//TODO: 10.4 doesn't have volume on NSSound.
		return 0.0;
	}
	void OSXSound::SetLooping(bool loop)
	{
		if ([sound respondsToSelector:@selector(setLoops:)]){
			[sound setLoops:loop];
		}	//TODO: 10.4 doesn't have setLoops on NSSound.
	}
	bool OSXSound::IsLooping()
	{
		if ([sound respondsToSelector:@selector(loops)]){
			return [sound loops];
		}	//TODO: 10.4 doesn't have loops on NSSound.
		return false;
	}
	bool OSXSound::IsPlaying()
	{
		return playing;
	}
	bool OSXSound::IsPaused()
	{
		return paused;
	}
	void OSXSound::OnComplete(bool finished)
	{
		// this is called from SoundDelegate and it will be 
		// on the main thread already
		this->playing = false;
		this->paused = false;
		if (this->callback)
		{
			ValueList args;
			SharedValue arg = Value::NewBool(finished);
			args.push_back(arg);
			(*this->callback)->Call(args);
		}
	}
	void OSXSound::OnComplete(SharedKMethod callback)
	{
		if (this->callback)
		{
			delete this->callback;
		}
		this->callback = new SharedKMethod(callback);
	}
}
