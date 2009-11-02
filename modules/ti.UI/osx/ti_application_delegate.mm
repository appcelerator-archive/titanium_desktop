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

@implementation TiApplicationDelegate

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
	}
	return self;
}

-(BOOL)application:(NSApplication*)theApplication openFile:(NSString*)filename
{
	AutoPtr<GlobalObject> globalObject(GlobalObject::GetInstance());
	AutoPtr<Event> event(globalObject->CreateEvent(Event::OPEN_REQUEST));

	KListRef files(new StaticBoundList());
	files->Append(Value::NewString([filename UTF8String]));
	event->SetList("files", files);

	globalObject->FireEvent(event);
	return YES;
}

-(BOOL)application:(NSApplication*)theApplication openFiles:(NSArray*)filenames
{
	AutoPtr<GlobalObject> globalObject(GlobalObject::GetInstance());
	AutoPtr<Event> event(globalObject->CreateEvent(Event::OPEN_REQUEST));

	KListRef files(new StaticBoundList());

	int arrayCount = [filenames count];
	for (int i = 0; i < arrayCount; i++)
	{
		files->Append(Value::NewString(
			[[filenames objectAtIndex:i] UTF8String]));
	}
	event->SetList("files", files);

	globalObject->FireEvent(event);
	return YES;
}

@end

