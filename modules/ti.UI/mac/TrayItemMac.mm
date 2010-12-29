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

#include "TrayItemMac.h"

#import <AppKit/NSStatusBar.h>

#import "TrayItemDelegate.h"

#include "MenuMac.h"
#include "UIMac.h"

namespace Titanium {

TrayItemMac::TrayItemMac(std::string& iconURL, KMethodRef cb) :
    TrayItem(iconURL),
    nativeMenu(0),
    menu(0),
    callback(cb),
    nativeItem(0)
{
    TrayItemDelegate* delegate = [[TrayItemDelegate alloc] initWithTray:this];
    NSStatusBar *statusBar = [NSStatusBar systemStatusBar];
    nativeItem = [statusBar statusItemWithLength:NSVariableStatusItemLength];
    [nativeItem retain];
    [nativeItem setTarget:delegate];
    [nativeItem setAction:@selector(invoke:)];
    [nativeItem setHighlightMode:YES];

    this->SetIcon(this->iconPath);
}

TrayItemMac::~TrayItemMac()
{
    if (!this->menu.isNull() && this->nativeMenu) {
        this->menu->DestroyNative(this->nativeMenu);
    }
}

void TrayItemMac::SetIcon(std::string& iconPath)
{
    NSImage* image = UIMac::MakeImage(iconPath);
    [nativeItem setImage:image];
}

void TrayItemMac::SetMenu(AutoPtr<Menu> menu)
{
    if (menu.get() == this->menu.get()) {
        return;
    }

    AutoPtr<MenuMac> newMenu = menu.cast<MenuMac>();
    NSMenu* newNativeMenu = nil;
    if (!newMenu.isNull()) {
        newNativeMenu = newMenu->CreateNativeNow(true);
    }

    if (!this->menu.isNull() && this->nativeMenu) {
        this->menu->DestroyNative(this->nativeMenu);
    }

    this->menu = newMenu;
    this->nativeMenu = newNativeMenu;
}

void TrayItemMac::SetHint(std::string& hint)
{
    if (hint.empty()) {
        [nativeItem setToolTip:@""];
    } else {
        [nativeItem setToolTip:[NSString stringWithUTF8String:hint.c_str()]];
    }
}

void TrayItemMac::Remove()
{
    [[NSStatusBar systemStatusBar] removeStatusItem:nativeItem];
    [[nativeItem target] release];
    [nativeItem release];
}

void TrayItemMac::InvokeCallback()
{
    if (nativeMenu != nil)
    {
        [nativeItem popUpStatusItemMenu:nativeMenu];
    }

    if (callback.isNull())
        return;

    try {
        callback->Call(ValueList());
    } catch (ValueException& e) {
        Logger* logger = Logger::Get("UI.TrayItemMac");
        SharedString ss = e.DisplayString();
        logger->Error("Tray icon callback failed: %s", ss->c_str());
    }
}

} // namespace Titanium
