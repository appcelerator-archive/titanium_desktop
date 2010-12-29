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

#ifndef UI_h
#define UI_h

#include <string>
#include <vector>

#include <kroll/kroll.h>

namespace Titanium {

class Menu;
class MenuItem;
class TrayItem;
class UserWindow;
class WindowConfig;

class UI : public KAccessorObject {
public:
    UI();
    virtual ~UI();

    void CreateMainWindow(AutoPtr<WindowConfig> config);
    AutoPtr<UserWindow> GetMainWindow();
    std::vector<AutoPtr<UserWindow> >& GetOpenWindows();
    void AddToOpenWindows(AutoPtr<UserWindow>);
    void RemoveFromOpenWindows(AutoPtr<UserWindow>);
    void ClearTray();
    void UnregisterTrayItem(TrayItem*);
    void _GetOpenWindows(const ValueList& args, KValueRef result);
    void _GetMainWindow(const ValueList& args, KValueRef result);
    void _CreateWindow(const ValueList& args, KValueRef result);
    void _CreateNotification(const ValueList& args, KValueRef result);
    void _CreateMenu(const ValueList& args, KValueRef result);
    void _CreateMenuItem(const ValueList& args, KValueRef result);
    void _CreateCheckMenuItem(const ValueList& args, KValueRef result);
    void _CreateSeparatorMenuItem(const ValueList& args, KValueRef result);
    AutoPtr<Menu> __CreateMenu(const ValueList& args);
    AutoPtr<MenuItem> __CreateMenuItem(const ValueList& args);
    AutoPtr<MenuItem> __CreateCheckMenuItem(const ValueList& args);
    AutoPtr<MenuItem> __CreateSeparatorMenuItem(const ValueList& args);
    void _SetMenu(const ValueList& args, KValueRef result);
    void _GetMenu(const ValueList& args, KValueRef result);
    void _SetContextMenu(const ValueList& args, KValueRef result);
    void _GetContextMenu(const ValueList& args, KValueRef result);
    void _SetIcon(const ValueList& args, KValueRef result);
    void _SetIcon(std::string iconURL);
    void _AddTray(const ValueList& args, KValueRef result);
    void _ClearTray(const ValueList& args, KValueRef result);
    void _GetIdleTime(const ValueList& args, KValueRef result);

    /* OS X specific callbacks */
    void _SetDockIcon(const ValueList& args, KValueRef result);
    void _SetDockMenu(const ValueList& args, KValueRef result);
    void _SetBadge(const ValueList& args, KValueRef result);
    void _SetBadgeImage(const ValueList& args, KValueRef result);

    virtual AutoPtr<Menu> CreateMenu() = 0;
    virtual AutoPtr<MenuItem> CreateMenuItem() = 0;;
    virtual AutoPtr<MenuItem> CreateCheckMenuItem() = 0;
    virtual AutoPtr<MenuItem> CreateSeparatorMenuItem() = 0;
    virtual void SetMenu(AutoPtr<Menu>) = 0;
    virtual void SetContextMenu(AutoPtr<Menu>) = 0;
    virtual void SetIcon(std::string& iconPath) = 0;
    virtual AutoPtr<TrayItem> AddTray(std::string& iconPath, KMethodRef cbSingleClick) = 0;
    virtual AutoPtr<Menu> GetMenu() = 0;
    virtual AutoPtr<Menu> GetContextMenu() = 0;
    virtual long GetIdleTime() = 0;

    /* These have empty impls, because are OS X-only for now */
#if defined(OS_OSX)
    virtual void SetDockIcon(std::string& icon_path) {}
    virtual void SetDockMenu(AutoPtr<Menu>) {}
    virtual void SetBadge(std::string& badgeLabel) {}
    virtual void SetBadgeImage(std::string& badgeImagePath) {}
#endif

    static void ErrorDialog(std::string);
    static inline UI* GetInstance() { return instance; }

protected:
    static void Log(Logger::Level level, std::string& message);

    static UI* instance;
    AutoPtr<UserWindow> mainWindow;
    std::vector<AutoPtr<UserWindow> > openWindows;
    std::vector<AutoPtr<TrayItem> > trayItems;
    std::string iconURL;
};

} // namespace Titanium

#endif
