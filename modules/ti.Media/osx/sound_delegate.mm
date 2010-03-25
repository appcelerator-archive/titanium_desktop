/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */

#import "sound_delegate.h"
#import "osx_sound.h"

@implementation SoundDelegate

-(void)setOSXSound:(OSXSound*)newSound
{
	sound = newSound;
}

-(void)dealloc
{
	sound = nil;
	[super dealloc];
}

-(void)completed:(id) unused
{
	if (sound)
		sound->SoundCompletedIteration();
}

-(void)sound:(NSSound*)s didFinishPlaying:(BOOL)finished
{
	if (finished)
	{
		[self performSelectorOnMainThread:@selector(completed:)
			withObject:nil waitUntilDone:NO];
	}
}

@end
