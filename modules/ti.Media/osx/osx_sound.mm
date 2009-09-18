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
		sound(nil),
		delegate([[SoundDelegate alloc] init]),
		nsurl([[NSURL URLWithString:[NSString stringWithUTF8String:url.c_str()]] retain])
	{
		[delegate setOSXSound:this];
		this->Load();
	}

	OSXSound::~OSXSound()
	{
		[delegate setOSXSound:nil];
		[sound setDelegate:nil];
		[delegate release];
		[nsurl release];
	}

	void OSXSound::LoadImpl()
	{
		@try
		{
			sound = [[NSSound alloc] initWithContentsOfURL:nsurl byReference:NO];
			[sound setDelegate:delegate];
		}
		@catch(NSException *ex)
		{
			throw ValueException::FromFormat("Error loading (%s): %s",
				[[nsurl absoluteString] UTF8String], [[ex reason] UTF8String]);
		}
		@catch(...)
		{
			throw ValueException::FromFormat("Unknown error loading (%s): %s",
				[[nsurl absoluteString] UTF8String]);
		}
	}

	void OSXSound::UnloadImpl()
	{
		if (!sound)
			return;

		[sound release];
		sound = nil;
	}

	void OSXSound::PlayImpl()
	{
		if (!sound)
			return;

		if (this->state == PAUSED)
			[sound resume];
		else
			[sound play];
	}

	void OSXSound::PauseImpl()
	{
		[sound pause];
	}

	void OSXSound::StopImpl()
	{
		if (!sound)
			return;

		[sound stop];
	}

	void OSXSound::SetVolumeImpl(double volume)
	{
		if (!sound)
			return;

		// TODO: 10.4 doesn't have setVolume on NSSound.
		if ([sound respondsToSelector:@selector(setVolume:)])
			[sound setVolume:volume];
	}

	double OSXSound::GetVolumeImpl()
	{
		// Initialize the sound volume apropriately.
		if ([sound respondsToSelector:@selector(volume)])
			return [sound volume];
		else
			return 0;
	}
}
