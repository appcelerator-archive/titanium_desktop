/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */
#import <Cocoa/Cocoa.h>
#import <kroll/kroll.h>
#import "osx_menu_item.h"


@interface OSXMenuDelegate : NSMenuItem
{
	ti::OSXMenuItem *delegate;
}
-(id)initWithMenu:(ti::OSXMenuItem*)item menu:(NSMenu*)menu;
-(void)invoke:(id)sender;
@end
