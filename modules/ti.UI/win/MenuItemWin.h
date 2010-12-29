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

#ifndef MenuItemWin_h
#define MenuItemWin_h

#include "../MenuItem.h"

#include <string>
#include <vector>

namespace Titanium {

class MenuWin;

class MenuItemWin : public MenuItem {
public:
    // Pieces of information we can use to do a reverse lookup
    // on an item's position in a parent menu.
    struct NativeItemBits {
        UINT id;
        HMENU parentMenu;
        HMENU submenu;
    };

    MenuItemWin(MenuItemType type);
    virtual ~MenuItemWin();

    void SetLabelImpl(std::string newLabel);
    void SetIconImpl(std::string newIconPath);
    void SetStateImpl(bool newState);
    void SetCallbackImpl(KMethodRef callback);
    void SetSubmenuImpl(AutoPtr<Menu> newSubmenu);
    void SetEnabledImpl(bool enabled);

    void RecreateAllNativeItems();
    void RecreateMenuItem(NativeItemBits* bits);
    void CreateNative(LPMENUITEMINFO itemInfo,
        HMENU nativeParentMenu, bool registerNative);
    void DestroyNative(NativeItemBits* bits);
    void DestroyNative(HMENU nativeParent, int position);
    static int GetNativeMenuItemPosition(NativeItemBits* bits);
    static bool HandleClickEvent(HMENU nativeMenu, UINT position);

private:
    std::string iconPath;
    AutoPtr<MenuWin> oldSubmenu;
    std::wstring wideOldLabel;
    std::vector<NativeItemBits*> nativeItems;

};

} // namespace Titanium

#endif
