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

#ifndef MenuMac_h
#define MenuMac_h

#include "../Menu.h"

#import <AppKit/NSMenu.h>

namespace Titanium {

class MenuMac : public Menu {
public:
    MenuMac();
    ~MenuMac();

    void AppendItemImpl(AutoPtr<MenuItem> item);
    void InsertItemAtImpl(AutoPtr<MenuItem> item, unsigned int index);
    void RemoveItemAtImpl(unsigned int index);
    void ClearImpl();

    NSMenu* CreateNativeNow(bool registerMenu=true);
    NSMenu* CreateNativeLazily(bool registerMenu=true);
    void FillNativeMainMenu(NSMenu* defaultMenu, NSMenu* nativeMainMenu);
    static void CopyMenu(NSMenu* from, NSMenu* to);
    static NSMenuItem* CopyMenuItem(NSMenuItem* item);

    void AddChildrenToNativeMenu(
        NSMenu* nativeMenu, bool registerMenu=true, bool isMainMenu=false);
    void AddChildrenToNSArray(NSMutableArray* array);
    void DestroyNative(NSMenu* nativeMenu);
    void UpdateNativeMenus();
    static void ClearNativeMenu(NSMenu* nativeMenu);
    static void UpdateNativeMenu(NSMenu* nativeMenu);
    static NSMenu* GetWindowMenu(NSMenu* nsMenu);
    static NSMenu* GetAppleMenu(NSMenu* nsMenu);
    static NSMenu* GetServicesMenu(NSMenu* nsMenu);
    static void EnsureAllItemsHaveSubmenus(NSMenu* menu);
    static void FixWindowMenu(NSMenu* menu);
    static bool IsNativeMenuAMainMenu(NSMenu* menu);
    static void SetupInspectorItem(NSMenu* menu);
    static void ReplaceAppNameStandinInMenu(NSMenu* menu, NSString* appName);

private:
    void Clear();
    NSMenu* CreateNative(bool lazy, bool registerMenu);
    std::vector<NSMenu*> nativeMenus;
};

} // namespace Titanium

#endif
