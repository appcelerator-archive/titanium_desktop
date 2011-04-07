/*
 * Copyright (c) 2008-2010 Appcelerator, Inc. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#import "TitaniumApplicationDelegate.h"

#import <Cocoa/Cocoa.h>
#import <AppKit/AppKit.h>

#include "MenuMac.h"

@implementation TitaniumApplicationDelegate

- (NSMenu *)applicationDockMenu:(NSApplication *)sender
{
    AutoPtr<Titanium::MenuMac> menu = binding->GetDockMenu().cast<Titanium::MenuMac>();
    if (!menu.isNull()) {
        NSMenu* nativeMenu = menu->CreateNativeNow(false);
        return nativeMenu;
    } else {
        return nil;
    }
}

- (id)initWithBinding:(Titanium::UIMac*)b
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

-(BOOL) applicationShouldHandleReopen:(NSApplication*)theApplication hasVisibleWindows:(BOOL)visibleWindows
{
    // Allow application to handle the dock click event in a custom way.
    AutoPtr<KEventObject> target = GlobalObject::GetInstance();
    AutoPtr<Event> event = target->CreateEvent(Event::REOPEN);
    event->SetBool("hasVisibleWindows", visibleWindows);
    return target->FireEvent(event) ? YES : NO;
}

-(NSApplicationTerminateReply) applicationShouldTerminate:(NSApplication*)sender
{
    kroll::Host::GetInstance()->Exit(0);
    return NO;
}

@end
