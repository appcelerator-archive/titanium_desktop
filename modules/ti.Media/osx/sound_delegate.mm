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

-(void)completed:(NSNumber*)finished
{
	if (sound)
		sound->SoundCompletedIteration();
}

-(void)sound:(NSSound*)s didFinishPlaying:(BOOL)finished
{
	//TODO: change to Host method
	NSNumber *result = [NSNumber numberWithBool:finished];
	[self performSelectorOnMainThread:@selector(completed:) 
		withObject:result waitUntilDone:NO];
}

@end
