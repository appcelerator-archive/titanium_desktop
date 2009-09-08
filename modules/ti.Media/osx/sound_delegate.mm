/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */

#import "sound_delegate.h"
#import "osx_sound.h"

@implementation SoundDelegate

-(id)initWithSound:(OSXSound*)s
{
	self = [super init];
	sound = s;
	return self;
}

-(void)dealloc
{
	sound = nil;
	[super dealloc];
}

-(void)completed:(NSNumber*)finished
{
	sound->Complete([finished boolValue]);
}

-(void)sound:(NSSound*)s didFinishPlaying:(BOOL)finished
{
	//TODO: change to Host method
	NSNumber *result = [NSNumber numberWithBool:finished];
	[self performSelectorOnMainThread:@selector(completed:) 
		withObject:result waitUntilDone:NO];
}

@end
