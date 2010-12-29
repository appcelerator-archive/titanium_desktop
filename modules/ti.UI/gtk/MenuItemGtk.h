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

#ifndef MenuItemGtk_h
#define MenuItemGtk_h

#include "../MenuItem.h"

#include <gtk/gtk.h>

#include <string>
#include <vector>

namespace Titanium {

class MenuGtk;

class MenuItemGtk : public MenuItem {
public:
    MenuItemGtk(MenuItemType type);
    virtual ~MenuItemGtk();

    void SetLabelImpl(std::string newLabel);
    void SetIconImpl(std::string newIconPath);
    void SetStateImpl(bool newState);
    void SetCallbackImpl(KMethodRef callback);
    void SetSubmenuImpl(AutoPtr<Menu> newSubmenu);
    void SetEnabledImpl(bool enabled);

    void ReplaceNativeItem(GtkMenuItem* nativeItem, GtkMenuItem* newNativeItem);
    void SetNativeItemIcon(GtkMenuItem* nativeItem, std::string& newIconPath);
    void SetNativeItemSubmenu(GtkMenuItem* nativeItem, AutoPtr<Menu> newSubmenu);
        GtkMenuItem* CreateNative(bool registerNative);
    void DestroyNative(GtkMenuItem* nativeItem);

private:
    std::vector<GtkMenuItem*> nativeItems;
    AutoPtr<MenuGtk> oldSubmenu;
};

} // namespace Titanium

#endif

