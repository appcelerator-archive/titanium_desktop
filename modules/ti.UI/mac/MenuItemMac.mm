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

#include "MenuItemMac.h"

#include "MenuItemDelegate.h"
#include "MenuMac.h"
#include "UIMac.h"

namespace Titanium {

MenuItemMac::MenuItemMac(MenuItemType type)
    : MenuItem(type)
{
}

MenuItemMac::~MenuItemMac()
{
}

void MenuItemMac::SetLabelImpl(std::string newLabel)
{
    if (this->type == SEPARATOR)
        return;
    this->UpdateNativeMenuItems();
}

void MenuItemMac::SetIconImpl(std::string newIconPath)
{
    if (this->type == SEPARATOR || this->type == CHECK)
        return;
    this->UpdateNativeMenuItems();
}

void MenuItemMac::SetStateImpl(bool newState)
{
    if (this->type != CHECK)
        return;
    this->UpdateNativeMenuItems();
}

void MenuItemMac::SetSubmenuImpl(AutoPtr<Menu> newSubmenu)
{
    if (this->type == SEPARATOR)
        return;
    this->UpdateNativeMenuItems();
}

void MenuItemMac::SetEnabledImpl(bool enabled)
{
    if (this->type == SEPARATOR)
        return;
    this->UpdateNativeMenuItems();
}

/*static*/
void MenuItemMac::SetNSMenuItemTitle(NSMenuItem* item, std::string& title)
{
    NSString* nstitle = [NSString stringWithUTF8String:title.c_str()];
    [item setTitle:nstitle];

    NSMenu* submenu = [item submenu];
    if (submenu != nil)
    {
        // Need to set the new native menu's title as this item's label. Each
        // native menu will have to use the title of the item it is attached to.
        [submenu setTitle:nstitle];
    }
}

/*static*/
void MenuItemMac::SetNSMenuItemIconPath(
    NSMenuItem* item, std::string& iconPath, NSImage* image)
{
    bool needsRelease = false;

    // If we weren't passed an image, create one for this call. This
    // allows callers to do one image creation in cases where the same
    // image is used over and over again.
    if (image == nil) {
        image = UIMac::MakeImage(iconPath);
        needsRelease = true;
    }

    if (!iconPath.empty()) {
        [item setImage:image];
    } else {
        [item setImage:nil];
    }

    if (needsRelease) {
        [image release];
    }
}

/*static*/
void MenuItemMac::SetNSMenuItemState(NSMenuItem* item, bool state)
{
    [item setState:state ? NSOnState : NSOffState];
}

/*static*/
void MenuItemMac::SetNSMenuItemSubmenu(
    NSMenuItem* item, AutoPtr<Menu> submenu, bool registerNative)
{
    if (!submenu.isNull()) {
        AutoPtr<MenuMac> osxSubmenu = submenu.cast<MenuMac>();
        NSMenu* nativeMenu = osxSubmenu->CreateNativeLazily(registerNative);
        [nativeMenu setTitle:[item title]];
        [item setSubmenu:nativeMenu];

    } else {
        [item setSubmenu:nil];
    }
}

/*static*/
void MenuItemMac::SetNSMenuItemEnabled(NSMenuItem* item, bool enabled)
{
    [item setEnabled:(enabled ? YES : NO)];
}

NSMenuItem* MenuItemMac::CreateNative(bool registerNative)
{
    if (this->IsSeparator()) {
        return [NSMenuItem separatorItem];
    } else {
        NSMenuItem* item = [[NSMenuItem alloc] 
            initWithTitle:@"Temp" action:@selector(invoke:) keyEquivalent:@""];

        MenuItemDelegate* delegate = [[MenuItemDelegate alloc] initWithMenuItem:this];
        [item setTarget:delegate];

        SetNSMenuItemTitle(item, this->label);
        SetNSMenuItemIconPath(item, this->iconPath);
        SetNSMenuItemState(item, this->state);
        SetNSMenuItemEnabled(item, this->enabled);
        SetNSMenuItemSubmenu(item, this->submenu, registerNative);

        if (registerNative)
        {
            this->nativeItems.push_back(item);
        }

        return item;
    }
}

void MenuItemMac::DestroyNative(NSMenuItem* realization)
{
    std::vector<NSMenuItem*>::iterator i = this->nativeItems.begin();
    while (i != this->nativeItems.end())
    {
        NSMenuItem* item = *i;
        if (item == realization)
        {
            i = this->nativeItems.erase(i);
            if (!this->submenu.isNull() && [item submenu] != nil)
            {
                AutoPtr<MenuMac> osxSubmenu = this->submenu.cast<MenuMac>();
                osxSubmenu->DestroyNative([item submenu]);
            }
            [item release];
        }
        else
        {
            i++;
        }
    }
}

void MenuItemMac::UpdateNativeMenuItems()
{
    std::vector<NSMenuItem*>::iterator i = this->nativeItems.begin();
    while (i != this->nativeItems.end())
    {
        NSMenuItem* nativeItem = (*i++);
        if ([nativeItem menu]) {
            MenuMac::UpdateNativeMenu([nativeItem menu]);
        }
    }

    // Must now iterate through the native menus and fix
    // the main menu -- it will modify this iterator so we
    // must do it in isolation.
    i = this->nativeItems.begin();
    while (i != this->nativeItems.end())
    {
        NSMenuItem* nativeItem = (*i++);
        if ([nativeItem menu] == [NSApp mainMenu]) {
            UIMac* binding =
                dynamic_cast<UIMac*>(UI::GetInstance());
            binding->SetupMainMenu(true);
            break;
        }
    }
}

void MenuItemMac::HandleClickEvent(KObjectRef source)
{
    MenuItem::HandleClickEvent(source);
}

} // namespace Titanium
