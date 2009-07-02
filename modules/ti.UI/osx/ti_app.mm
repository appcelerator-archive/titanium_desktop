/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */
#import <Cocoa/Cocoa.h>
#import <AppKit/AppKit.h>
#import "ti_app.h"
#import "osx_ui_binding.h"
#import "osx_menu_item.h"

static TiApplication *tiAppInstance = NULL;

@implementation TiApplication
+(TiApplication*)instance
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
	OSXUIBinding *ui = static_cast<OSXUIBinding*>(binding);
	SharedPtr<OSXMenu> menu = ui->GetDockMenu().cast<OSXMenu>();

	if (!menu.isNull()) {
		NSMenu* nativeMenu = menu->CreateNative(false);
		[nativeMenu autorelease];
		return nativeMenu;

	} else {
		return nil;
	}
}
- (id)initWithBinding:(ti::UIBinding*)b host:(kroll::Host*)h
{
	self = [super init];
	if (self)
	{
		binding = b;
		host = h;
		tiAppInstance = self;
	}
	return self;
}
-(kroll::Host*)host
{
	return host;
}
@end

