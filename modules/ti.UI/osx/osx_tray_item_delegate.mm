/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#import "../ui_module.h"

@implementation OSXTrayDelegate
-(id)initWithTray:(OSXTrayItem*)d
{
	self = [super init];
	if (self!=nil)
	{
		delegate = d;
		NSStatusBar *statusBar = [NSStatusBar systemStatusBar];
		item = [statusBar statusItemWithLength:NSVariableStatusItemLength];
		[item setTarget:self];
		[item setAction:@selector(invoke:)];
	}
	return self;
}
-(void)dealloc
{
	// make sure we remove our tray when this guys dead
	NSStatusBar *statusBar = [NSStatusBar systemStatusBar];
	[statusBar removeStatusItem:item];
	[super dealloc];
}
-(void)invoke:(id)sender
{
	// 1. Fire the activated event
	ti::OSXTrayItem *tray = static_cast<ti::OSXTrayItem*>(delegate);
	tray->Invoke();

	// 2. Display the tray icon menu
	if (menu)
	{
		[item popUpStatusItemMenu:menu];
	}
}
-(void)setIcon:(NSImage*)path
{
	[item setImage:path];
}
-(void)setHint:(NSString*)hint
{
	[item setToolTip:hint];
}
-(void)setMenu:(NSMenu*)newMenu
{
	menu = newMenu;
}

@end


