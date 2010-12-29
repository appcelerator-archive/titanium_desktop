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

#ifndef MenuGtk_h
#define MenuGtk_h

#include "../Menu.h"

#include <gtk/gtk.h>

namespace Titanium {

class MenuGtk : public Menu {
public:
    MenuGtk();
    ~MenuGtk();

    void AppendItemImpl(AutoPtr<MenuItem> item);
    void InsertItemAtImpl(AutoPtr<MenuItem> item, unsigned int index);
    void RemoveItemAtImpl(unsigned int index);
    void ClearImpl();

    void ClearNativeMenu(GtkMenuShell* nativeMenu);
    void RemoveItemAtFromNativeMenu(GtkMenuShell* nativeMenu, unsigned int index);
    void DestroyNative(GtkMenuShell* nativeMenu);
    GtkMenuShell* CreateNativeBar(bool registerNative);
    GtkMenuShell* CreateNative(bool registerNative);
    void AddChildrenToNativeMenu(GtkMenuShell* nativeMenu, bool registerNative);
    void RegisterNativeMenuItem(AutoPtr<MenuItem> item, GtkMenuItem* nativeItem);
    void DestroyNativeMenuItem(GtkMenuItem* nativeItem);

private:
    std::vector<AutoPtr<MenuItem> > oldChildren;
    std::vector<GtkMenuShell*> nativeMenus;
};

} // namespace Titanium

#endif

