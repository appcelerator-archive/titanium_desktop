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

#include "MenuMac.h"

#include "MenuDelegate.h"
#include "MenuItemMac.h"
#include "UIMac.h"

using namespace std;

// The front offset accounts for the appplication and edit menus:w
// The rear offset accounts for the window and help menus
#define MAINMENU_FRONT_OFFSET 2
#define MAINMENU_REAR_OFFSET 2

namespace Titanium {

MenuMac::MenuMac()
    : Menu()
{
}

MenuMac::~MenuMac()
{
    vector<NSMenu*>::iterator i = this->nativeMenus.begin();
    while (i != this->nativeMenus.end()) {
        this->ClearNativeMenu((*i++));
    }
    nativeMenus.clear();
}

void MenuMac::AppendItemImpl(AutoPtr<MenuItem> item)
{
    this->UpdateNativeMenus();
}

void MenuMac::InsertItemAtImpl(AutoPtr<MenuItem> item, unsigned int index)
{
    this->UpdateNativeMenus();
}

void MenuMac::RemoveItemAtImpl(unsigned int index)
{
    this->UpdateNativeMenus();
}

void MenuMac::ClearImpl()
{
    this->Clear();
}

void MenuMac::Clear()
{
    this->UpdateNativeMenus();
}

/*static*/
void MenuMac::ClearNativeMenu(NSMenu* nativeMenu)
{
    SEL getMenuItemSelector = @selector(getMenuItem:);

    while ([nativeMenu numberOfItems] > 0)
    {
        NSMenuItem* nativeItem = [nativeMenu itemAtIndex: 0];
        id delegate = [nativeItem target];

        // This delegate is one of our MenuItemMac delegates, so grab its
        // MenuItemMac reference and tell it to destroy this native menu item
        if (delegate && [delegate respondsToSelector:getMenuItemSelector]) {
            MenuItemMac* menuItem = (MenuItemMac*) [delegate performSelector:getMenuItemSelector];
            menuItem->DestroyNative(nativeItem);
        }
        [nativeMenu removeItemAtIndex:0];
    }
}

void MenuMac::DestroyNative(NSMenu* nativeMenu)
{
    // Remove the native menu from our list of known native menus
    vector<NSMenu*>::iterator i = this->nativeMenus.begin();
    while (i != this->nativeMenus.end()) {
        if (*i == nativeMenu) {
            i = this->nativeMenus.erase(i);
        } else {
            i++;
        }
    }

    // Clear the native menu and release it
    this->ClearNativeMenu(nativeMenu);
    [nativeMenu release];
}

void MenuMac::UpdateNativeMenus()
{
    vector<NSMenu*>::iterator i = this->nativeMenus.begin();
    while (i != this->nativeMenus.end()) {
        MenuMac::UpdateNativeMenu(*i++);
    }

    // If this native menu is the application's current main menu,
    // we want to force the main menu to reload.
    UIMac* binding = dynamic_cast<UIMac*>(UI::GetInstance());
    if (binding->GetActiveMenu().get() == this) {
        binding->SetupMainMenu(true);
    }
}

/*static*/
void MenuMac::UpdateNativeMenu(NSMenu* nativeMenu)
{
    // If this is the application's main menu, just wait until
    // later to fix it -- it will modify the parent iterator.
    if (nativeMenu == [NSApp mainMenu]) {

    // Otherwise, just mark this menu as dirty and the next time OS X
    // shows it, it will trigger a redraw.
    } else {
        [[nativeMenu delegate] performSelector:@selector(markAsDirty)];
    }
}

NSMenu* MenuMac::CreateNativeNow(bool registerNative)
{
    return this->CreateNative(false, registerNative);
}

NSMenu* MenuMac::CreateNativeLazily(bool registerNative)
{
    return this->CreateNative(true, registerNative);
}

NSMenu* MenuMac::CreateNative(bool lazy, bool registerNative)
{
    // This title should be set by the callee - see MenuItemMac::NSMenuSetSubmenu
    NSMenu* menu = [[NSMenu alloc] initWithTitle:@"TopLevelMenu"];
    MenuDelegate* delegate = [[MenuDelegate alloc] 
        initWithMenu:this
        willRegister: registerNative ? YES : NO];

    // Add menu children lazily
    [delegate markAsDirty];
    [menu setDelegate:delegate];
    [menu setAutoenablesItems:NO];

    if (!lazy)
        this->AddChildrenToNativeMenu(menu, registerNative);

    if (registerNative)
    {
        this->nativeMenus.push_back(menu);
    }
    return menu;
}

void MenuMac::FillNativeMainMenu(NSMenu* defaultMenu, NSMenu* nativeMainMenu)
{
    // We are keeping a reference to this NSMenu*, so bump the reference
    // count. This method must be matched with a DestroyNative(...) call
    // to avoid a memory leak.
    [nativeMainMenu retain];

    MenuMac::CopyMenu(defaultMenu, nativeMainMenu);
    MenuMac::SetupInspectorItem(nativeMainMenu);

    this->AddChildrenToNativeMenu(nativeMainMenu, true, true);

    // The main menu needs all NSMenuItems in it to have submenus for 
    // proper display. So we just add empty submenus to those items missing them.
    MenuMac::EnsureAllItemsHaveSubmenus(nativeMainMenu);

    this->nativeMenus.push_back(nativeMainMenu);
}

void MenuMac::AddChildrenToNativeMenu(NSMenu* nativeMenu, bool registerNative, bool mainMenu)
{
    vector<AutoPtr<MenuItem> >::iterator i = this->children.begin();
    while (i != this->children.end()) {
        AutoPtr<MenuItem> item = *i++;
        AutoPtr<MenuItemMac> osxItem = item.cast<MenuItemMac>();
        NSMenuItem* nativeItem = osxItem->CreateNative(registerNative);

        int rearOffset = mainMenu ?  MAINMENU_REAR_OFFSET : 0;
        int index = [nativeMenu numberOfItems] - rearOffset;
        [nativeMenu insertItem:nativeItem atIndex:index];
    }
    [nativeMenu sizeToFit];
}

void MenuMac::AddChildrenToNSArray(NSMutableArray* array)
{
    vector<AutoPtr<MenuItem> >::iterator i = this->children.begin();
    while (i != this->children.end())
    {
        AutoPtr<MenuItem> item = *i++;
        AutoPtr<MenuItemMac> osxItem = item.cast<MenuItemMac>();
        NSMenuItem* nsItem = osxItem->CreateNative(false);
        [array addObject:nsItem];
        [nsItem release];
    }
}

/*static*/
void MenuMac::CopyMenu(NSMenu* from, NSMenu* to)
{
    [to setTitle:[from title]];
    [to setAutoenablesItems:NO];
    for (int i = 0; i < [from numberOfItems]; i++)
    {
        NSMenuItem* duplicateItem = MenuMac::CopyMenuItem([from itemAtIndex:i]);
        [to addItem:duplicateItem];
    }
}

/*static*/
NSMenuItem* MenuMac::CopyMenuItem(NSMenuItem* item)
{
    if ([item isSeparatorItem])
    {
        return [NSMenuItem separatorItem];
    }
    else
    {
        NSMenuItem* duplicate = [[NSMenuItem alloc] 
            initWithTitle:[item title]
            action:[item action]
            keyEquivalent:[item keyEquivalent]];
        [duplicate setState:[item state]];
        [duplicate setTag:[item tag]];
        [duplicate setEnabled:[item isEnabled]];
        [duplicate setKeyEquivalentModifierMask:[item keyEquivalentModifierMask]];

        if ([item submenu] != nil)
        {
            NSMenu* newSubmenu = [[NSMenu alloc] init];
            MenuMac::CopyMenu([item submenu], newSubmenu);
            [duplicate setSubmenu:newSubmenu];
            [newSubmenu release];
        }
        return duplicate;
    }

}

/*static*/
bool MenuMac::IsNativeMenuAMainMenu(NSMenu* menu)
{
    return ([menu numberOfItems] > 0 &&
        ([[[[menu itemAtIndex:0] submenu] title] isEqualToString:@"Apple"]
         || [[[[menu itemAtIndex:0] submenu] title] isEqualToString:@"APPNAME"]));
}

/*static*/
NSMenu* MenuMac::GetWindowMenu(NSMenu* menu)
{
    if (IsNativeMenuAMainMenu(menu))
    {
        return [[menu itemAtIndex:([menu numberOfItems] - 2)] submenu];
    }
    else
    {
        return nil;
    }
}

/*static*/
NSMenu* MenuMac::GetAppleMenu(NSMenu* menu)
{
    if (IsNativeMenuAMainMenu(menu))
    {
        return [[menu itemAtIndex:0] submenu];
    }
    else
    {
        return nil;
    }
}

/*static*/
NSMenu* MenuMac::GetServicesMenu(NSMenu* menu)
{
    if (!IsNativeMenuAMainMenu(menu))
    {
        return nil;
    }

    NSMenu* submenu = [[menu itemAtIndex:0] submenu];
    if (submenu == nil)
    {
        return nil;
    }

    for (int i = 0; i < [submenu numberOfItems]; i++)
    {
        NSMenuItem* item = [submenu itemAtIndex:i];
        if ([[item title] isEqualToString:@"Services"])
        {
            return [item submenu];
        }
    }
    return nil;
}

/*static*/
void MenuMac::EnsureAllItemsHaveSubmenus(NSMenu* menu)
{
    for (int i = 0; i < [menu numberOfItems]; i++)
    {
        NSMenuItem* item = [menu itemAtIndex:i];
        if ([item submenu] == nil)
        {
            NSMenu* sm = [[NSMenu alloc] initWithTitle:[item title]];
            [item setSubmenu:sm];
            [sm release];
        }
    }
}

/*static*/
void MenuMac::FixWindowMenu(NSMenu* menu)
{
    NSMenu* windowMenu = MenuMac::GetWindowMenu(menu);
    if (windowMenu == nil)
    {
        return;
    }

    int lastSeparator = -1;
    for (int i = 0; i < [windowMenu numberOfItems]; i++)
    {
        if ([[windowMenu itemAtIndex:i] isSeparatorItem])
        {
            lastSeparator = i;
        }
    }

    if (lastSeparator == -1)
    {
        return;
    }

    while ([windowMenu numberOfItems] > lastSeparator+1)
    {
        [windowMenu removeItemAtIndex:lastSeparator+1];
    }
}

/*static*/
void MenuMac::SetupInspectorItem(NSMenu* menu)
{
    NSMenu* windowMenu = MenuMac::GetWindowMenu(menu);
    NSMenuItem* showInspector = [windowMenu
        itemWithTitle:NSLocalizedString(@"Show Inspector", @"")];
    NSMenuItem* showInspectorSeparator = [windowMenu
        itemWithTitle:NSLocalizedString(@"Show Inspector Separator", @"")];

    if (!Host::GetInstance()->DebugModeEnabled())
    {
        if (showInspector != nil)
            [windowMenu removeItem:showInspector];
        if (showInspectorSeparator != nil)
            [windowMenu removeItem:showInspectorSeparator];
    }
}

/*static*/
void MenuMac::ReplaceAppNameStandinInMenu(NSMenu* menu, NSString* appName)
{
    static NSString* appNameStandin = @"APPNAME";

    for (int i = 0; i < [menu numberOfItems]; i++)
    {
        NSMenuItem* item = [menu itemAtIndex:i];
        NSString* title = [item title];
        if ([title rangeOfString:appNameStandin].location != NSNotFound)
        {
            title = [title stringByReplacingOccurrencesOfString:appNameStandin
                withString:appName];
            [item setTitle:title];
        }

        if ([item submenu])
            ReplaceAppNameStandinInMenu([item submenu], appName);
    }
}

} // namespace Titanium
