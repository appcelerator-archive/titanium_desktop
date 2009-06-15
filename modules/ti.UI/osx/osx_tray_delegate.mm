/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#import "../ui_module.h"
#import "osx_tray_delegate.h"
#import "osx_tray_item.h"
#import "osx_menu_item.h"
#import "osx_ui_binding.h"

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
	[submenu release];
	// make sure we remove our tray when this guys dead
	NSStatusBar *statusBar = [NSStatusBar systemStatusBar];
	[statusBar removeStatusItem:item];
	[super dealloc];
}
-(void)invoke:(id)sender
{
	// first the event first...
	ti::OSXTrayItem *tray = static_cast<ti::OSXTrayItem*>(delegate);
	tray->Invoke();
	// then display the submenu
	if (submenu)
	{
		[item popUpStatusItemMenu:submenu];
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
-(void)addMenu:(SharedPtr<ti::MenuItem>)menu
{
	if (submenu)
	{
		[submenu release];
		submenu = nil;
	}
	if (menu.isNull())
	{
		return;
	}
	SharedPtr<ti::OSXMenuItem> osx_menu = menu.cast<ti::OSXMenuItem>();
	int count = osx_menu->GetChildCount();
	if (count == 0) return ;
	submenu = ti::OSXUIBinding::MakeMenu(osx_menu);
	if ([submenu numberOfItems] == 0)
	{
		// this happens if they're all disabled
		[submenu release];
		submenu = nil;
	}
}

@end


