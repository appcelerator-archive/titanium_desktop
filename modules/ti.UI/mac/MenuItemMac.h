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

#ifndef MenuItemMac_h
#define MenuItemMac_h

#include "../MenuItem.h"

#import <Cocoa/Cocoa.h>

namespace Titanium {

class Menu;

class MenuItemMac : public MenuItem {
public:
    MenuItemMac(MenuItemType type);
    virtual ~MenuItemMac();

    void SetLabelImpl(std::string newLabel);
    void SetIconImpl(std::string newIconPath);
    void SetStateImpl(bool newState);
    void SetCallbackImpl(KMethodRef callback);
    void SetSubmenuImpl(AutoPtr<Menu> newSubmenu);
    void SetEnabledImpl(bool enabled);

    NSMenuItem* CreateNative(bool registerNative=true);
    void DestroyNative(NSMenuItem* realization);
    void UpdateNativeMenuItems();
    virtual void HandleClickEvent(KObjectRef source);

private:
    static void SetNSMenuItemTitle(NSMenuItem* item, std::string& title);
    static void SetNSMenuItemState(NSMenuItem* item, bool state);
    static void SetNSMenuItemIconPath(
        NSMenuItem* item, std::string& iconPath, NSImage* image = nil);
    static void SetNSMenuItemSubmenu(
        NSMenuItem* item, AutoPtr<Menu> submenu, bool registerNative=true);
    static void SetNSMenuItemEnabled(NSMenuItem* item, bool enabled);

    std::vector<NSMenuItem*> nativeItems;
};

} // namespace Titanium

#endif
