/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */
#import <Cocoa/Cocoa.h>
#import <AppKit/AppKit.h>
#import "../ui_module.h"
#import "osx_ui_binding.h"
#import "osx_menu_item.h"

static TiApplicationDelegate *tiAppInstance = NULL;

@implementation TiApplicationDelegate
+(TiApplicationDelegate*)instance
{
	return tiAppInstance;
}
+(NSString*)appID
{
	AppConfig *config = AppConfig::Instance();
	return [NSString stringWithCString:config->GetAppID().c_str()];
}
- (NSMenu *)applicationDockMenu:(NSApplication *)sender
{
	AutoPtr<OSXMenu> menu = binding->GetDockMenu().cast<OSXMenu>();
	if (!menu.isNull()) {
		NSMenu* nativeMenu = menu->CreateNativeNow(false);
		return nativeMenu;
	} else {
		return nil;
	}
}
- (id)initWithBinding:(ti::OSXUIBinding*)b
{
	self = [super init];
	if (self)
	{
		binding = b;
		tiAppInstance = self;
	}
	return self;
}
@end

