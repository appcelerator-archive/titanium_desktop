/*
 * Copyright (c) 2009-2010 Appcelerator, Inc. All Rights Reserved.
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

#import "MenuDelegate.h"

#include "MenuMac.h"

@implementation MenuDelegate

- (id)initWithMenu:(Titanium::MenuMac*)inMenu willRegister:(BOOL)willRegister
{
    if ([super init]) {
        dirty = YES;
        menu = inMenu;
        registerNative = willRegister;
    }
    return self;
}

- (NSInteger)numberOfItemsInMenu:(NSMenu *)menu
{
    // We will setup the menu in menuNeedsUpdate.
    return -1;
}

- (void)menuNeedsUpdate:(NSMenu *)nativeMenu
{
    if (!dirty) {
        return;
    }

    dirty = NO;
    Titanium::MenuMac::ClearNativeMenu(nativeMenu);
    menu->AddChildrenToNativeMenu(nativeMenu, registerNative ? true : false);
}

- (void)markAsDirty
{
    dirty = YES;
}

@end
