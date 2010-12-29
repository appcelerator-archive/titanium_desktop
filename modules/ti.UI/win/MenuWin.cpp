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

#include "MenuWin.h"

#include "MenuItemWin.h"
#include "UserWindowWin.h"

using namespace std;

namespace Titanium {

MenuWin::MenuWin()
    : Menu()
{
}

MenuWin::~MenuWin()
{
    vector<HMENU>::iterator i = this->nativeMenus.begin();
    while (i != this->nativeMenus.end()) {
        this->ClearNativeMenu((*i++));
    }
    nativeMenus.clear();
}

void MenuWin::AppendItemImpl(AutoPtr<MenuItem> item)
{
    AutoPtr<MenuItemWin> win32Item = item.cast<MenuItemWin>();

    vector<HMENU>::iterator i = this->nativeMenus.begin();
    while (i != this->nativeMenus.end()) {
        HMENU nativeMenu = (*i++);
        MenuWin::InsertItemIntoNativeMenu(win32Item, nativeMenu, true);
    }

    this->oldChildren = this->children;

    if (this->nativeMenus.size() > 0) {
        UserWindowWin::RedrawAllMenus();
    }
}

void MenuWin::InsertItemAtImpl(AutoPtr<MenuItem> item, unsigned int index)
{
    AutoPtr<MenuItemWin> win32Item = item.cast<MenuItemWin>();
    
    vector<HMENU>::iterator i = this->nativeMenus.begin();
    while (i != this->nativeMenus.end())
    {
        HMENU nativeMenu = (*i++);
        MenuWin::InsertItemIntoNativeMenu(win32Item, nativeMenu, true, index);
    }

    this->oldChildren = this->children;

    if (this->nativeMenus.size() > 0) {
        UserWindowWin::RedrawAllMenus();
    }
}

void MenuWin::RemoveItemAtImpl(unsigned int index)
{
    AutoPtr<MenuItemWin> win32Item = oldChildren.at(index).cast<MenuItemWin>();

    vector<HMENU>::iterator i = this->nativeMenus.begin();
    while (i != this->nativeMenus.end())
    {
        HMENU nativeMenu = (*i++);
        MenuWin::RemoveItemAtFromNativeMenu(win32Item, nativeMenu, index);
    }

    this->oldChildren = this->children;

    if (this->nativeMenus.size() > 0) {
        UserWindowWin::RedrawAllMenus();
    }
}

void MenuWin::ClearImpl()
{
    vector<HMENU>::iterator i = this->nativeMenus.begin();
    while (i != this->nativeMenus.end())
    {
        HMENU nativeMenu = (*i++);
        this->ClearNativeMenu(nativeMenu);
    }

    this->oldChildren = this->children;

    if (this->nativeMenus.size() > 0) {
        UserWindowWin::RedrawAllMenus();
    }
}

/*static*/
void MenuWin::InsertItemIntoNativeMenu(
    MenuItemWin* item, HMENU nativeMenu, bool registerNative, int position)
{
    
    // A -1 value means to stick the new item at the end
    if (position == -1) {
        position = GetMenuItemCount(nativeMenu);
        if (position == 0) position = 1;
    }

    MENUITEMINFO itemInfo;
    item->CreateNative(&itemInfo, nativeMenu, registerNative);
    BOOL success = InsertMenuItem(nativeMenu, position, TRUE, &itemInfo);

    if (!success) {
        string error = Win32Utils::QuickFormatMessage(GetLastError());
        throw ValueException::FromString("Could not insert native item: " + error);
    }

}

/*static*/
void MenuWin::RemoveItemAtFromNativeMenu(
    MenuItemWin* item, HMENU nativeMenu, int position)
{
    item->DestroyNative(nativeMenu, position);

    BOOL success = DeleteMenu(nativeMenu, position, MF_BYPOSITION);
    if (!success) {
        string error = Win32Utils::QuickFormatMessage(GetLastError());
        throw ValueException::FromString("Could not remove native item: " + error);
    }
}

void MenuWin::ClearNativeMenu(HMENU nativeMenu)
{
    for (int i = GetMenuItemCount(nativeMenu) - 1; i >= 0; i--) {
        AutoPtr<MenuItemWin> win32Item =
            this->oldChildren.at(i).cast<MenuItemWin>();
        RemoveItemAtFromNativeMenu(win32Item.get(), nativeMenu, i);
    }
}

void MenuWin::DestroyNative(HMENU nativeMenu)
{
    if (!nativeMenu) {
        return;
    }

    // Remove the native menu from our list of known native menus
    vector<HMENU>::iterator i = this->nativeMenus.begin();
    while (i != this->nativeMenus.end()) {
        if (*i == nativeMenu)
            i = this->nativeMenus.erase(i);
        else
            i++;
    }

    // Clear the native menu and release, so that children will be freed
    this->ClearNativeMenu(nativeMenu);
}

HMENU MenuWin::CreateNative(bool registerNative)
{
    HMENU nativeMenu = CreatePopupMenu();
    ApplyNotifyByPositionStyleToNativeMenu(nativeMenu);
    this->AddChildrenToNativeMenu(nativeMenu, registerNative);

    if (registerNative)
        this->nativeMenus.push_back(nativeMenu);

    return nativeMenu;
}

HMENU MenuWin::CreateNativeTopLevel(bool registerNative)
{
    HMENU nativeMenu = CreateMenu();
    ApplyNotifyByPositionStyleToNativeMenu(nativeMenu);
    this->AddChildrenToNativeMenu(nativeMenu, registerNative);

    if (registerNative)
        this->nativeMenus.push_back(nativeMenu);

    return nativeMenu;
}

void MenuWin::AddChildrenToNativeMenu(HMENU nativeMenu, bool registerNative)
{
    vector<AutoPtr<MenuItem> >::iterator i = this->children.begin();
    while (i != this->children.end()) {
        AutoPtr<MenuItem> item = *i++;
        AutoPtr<MenuItemWin> win32Item = item.cast<MenuItemWin>();
        MenuWin::InsertItemIntoNativeMenu(win32Item.get(), nativeMenu, registerNative);
    }
}

/*static*/
void MenuWin::ApplyNotifyByPositionStyleToNativeMenu(HMENU nativeMenu)
{
    MENUINFO menuInfo;
    ZeroMemory(&menuInfo, sizeof(MENUINFO)); 
    menuInfo.cbSize = sizeof(MENUINFO);
    menuInfo.fMask = MIM_APPLYTOSUBMENUS | MIM_STYLE;
    menuInfo.dwStyle = MNS_CHECKORBMP | MNS_NOTIFYBYPOS; 
    BOOL success = SetMenuInfo(nativeMenu, &menuInfo);

    if (!success) {
        string error = Win32Utils::QuickFormatMessage(GetLastError());
        throw ValueException::FromString("Could not set native menu style: " + error);
    }
}

} // namespace Titanium
