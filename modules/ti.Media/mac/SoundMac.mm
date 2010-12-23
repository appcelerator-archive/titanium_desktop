/*
 * Copyright (c) 2008-2010 Appcelerator, Inc. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#import "SoundMac.h"

#import "SoundDelegate.h"

@interface NSSound (MakeTheErrorsGoAway)
//These all are in 10.5, but building for 10.4 produces warnings unless I mention them here.
- (void)setVolume:(float)volume;
- (float)volume;
- (BOOL)loops;
- (void)setLoops:(BOOL)loops;
@end

namespace Titanium {

SoundMac::SoundMac(std::string& url) :
	Sound(url),
	sound(nil),
	delegate([[SoundDelegate alloc] init]),
	fileURL(nil)
{
	// Convert the path back into a file:// URL. We don't use the
	// original URL here because it may be an app:// or ti:// URL.
	this->fileURL = [[NSURL URLWithString:[NSString stringWithUTF8String:
		URLUtils::PathToFileURL(this->path).c_str()]] retain];
	[delegate setOSXSound:this];
	this->Load();
}

SoundMac::~SoundMac()
{
	[delegate setOSXSound:nil];
	[sound setDelegate:nil];
	[delegate release];
	[fileURL release];
}

void SoundMac::LoadImpl()
{
	@try
	{
		sound = [[NSSound alloc] initWithContentsOfURL:fileURL byReference:NO];
		[sound setDelegate:delegate];
	}
	@catch(NSException *ex)
	{
		throw ValueException::FromFormat("Error loading (%s): %s",
			[[fileURL absoluteString] UTF8String], [[ex reason] UTF8String]);
	}
	@catch(...)
	{
		throw ValueException::FromFormat("Unknown error loading (%s): %s",
			[[fileURL absoluteString] UTF8String]);
	}
}

void SoundMac::UnloadImpl()
{
	if (!sound)
		return;

	[sound release];
	sound = nil;
}

void SoundMac::PlayImpl()
{
	if (!sound)
		return;

	if (this->state == PAUSED)
		[sound resume];
	else
		[sound play];
}

void SoundMac::PauseImpl()
{
	[sound pause];
}

void SoundMac::StopImpl()
{
	if (!sound)
		return;

	[sound stop];
}

void SoundMac::SetVolumeImpl(double volume)
{
	if (!sound)
		return;

	// TODO: 10.4 doesn't have setVolume on NSSound.
	if ([sound respondsToSelector:@selector(setVolume:)])
		[sound setVolume:volume];
}

double SoundMac::GetVolumeImpl()
{
	// Initialize the sound volume apropriately.
	if ([sound respondsToSelector:@selector(volume)])
		return [sound volume];
	else
		return 0;
}

} // namespace Titanium
