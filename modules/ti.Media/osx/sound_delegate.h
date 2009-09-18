/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */
#import <Cocoa/Cocoa.h>
#import "osx_sound.h"

using namespace ti;

@interface SoundDelegate : NSObject
{
	OSXSound* sound;
}
-(void)setOSXSound:(OSXSound*)sound;
-(void)dealloc;
@end

