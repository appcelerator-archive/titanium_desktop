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

#ifndef UIGtk_h
#define UIGtk_h

#include "../UI.h"

namespace Titanium {

class MenuGtk;
class MenuItemGtk;

class UIGtk : public UI {
public:
    UIGtk();

    AutoPtr<Menu> CreateMenu();
    AutoPtr<MenuItem> CreateMenuItem();
    AutoPtr<MenuItem> CreateSeparatorMenuItem();
    AutoPtr<MenuItem> CreateCheckMenuItem();
    void SetMenu(AutoPtr<Menu> newMenu);
    void SetContextMenu(AutoPtr<Menu>);
    void SetIcon(std::string&);
    AutoPtr<TrayItem> AddTray(std::string&, KMethodRef cb);
    long GetIdleTime();
    AutoPtr<Menu> GetMenu();
    AutoPtr<Menu> GetContextMenu();
    std::string& GetIcon();

    static void ErrorDialog(std::string);

protected:
    AutoPtr<MenuGtk> menu;
    AutoPtr<MenuGtk> contextMenu;
    std::string iconPath;
};

} // namespace Titanium

#endif

