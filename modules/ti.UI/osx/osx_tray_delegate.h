/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */
#import <Cocoa/Cocoa.h>
#import <kroll/kroll.h>
#import "osx_tray_item.h"
#import "../menu_item.h"


@interface OSXTrayDelegate : NSObject
{
	ti::OSXTrayItem *delegate;
	NSStatusItem *item;
	NSMenu *submenu;
}
-(id)initWithTray:(ti::OSXTrayItem*)item;
-(void)invoke:(id)sender;
-(void)setIcon:(NSImage*)path;
-(void)setHint:(NSString*)hint;
-(void)addMenu:(SharedPtr<ti::MenuItem>)menu;

@end
