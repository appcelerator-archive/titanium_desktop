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

#ifndef UIMac_h
#define UIMac_h

#include "../UI.h"

#import <AppKit/NSMenu.h>

namespace Titanium {

class MenuItemMac;
class MenuMac;
class UserWindowMac;

class UIMac : public UI {
public:
	UIMac();
	~UIMac();

	AutoPtr<Menu> CreateMenu();
	AutoPtr<MenuItem> CreateMenuItem();
	AutoPtr<MenuItem> CreateCheckMenuItem();
	AutoPtr<MenuItem> CreateSeparatorMenuItem();
	AutoPtr<Menu> GetMenu();
	AutoPtr<Menu> GetContextMenu();
	AutoPtr<Menu> GetDockMenu();
	NSMenu* GetNativeDockMenu();
	void SetMenu(AutoPtr<Menu>);
	void SetContextMenu(AutoPtr<Menu>);
	void SetDockMenu(AutoPtr<Menu>);

	AutoPtr<TrayItem> AddTray(std::string& icon_path, KMethodRef cb);
	void SetIcon(std::string& iconPath);
	virtual void SetDockIcon(std::string& iconPath);
	virtual void SetBadge(std::string& badgeLabel);
	virtual void SetBadgeImage(std::string& badgeImage);

	AutoPtr<MenuMac> GetActiveMenu();
	void WindowFocused(AutoPtr<UserWindowMac> window);
	void WindowUnfocused(AutoPtr<UserWindowMac> window);
	void SetupMainMenu(bool force = false);
	void SetupAppMenuParts(NSMenu* mainMenu);
	void ReplaceMainMenu();
	NSMenu* GetDefaultMenu();
	long GetIdleTime();

	static NSImage* MakeImage(std::string&);
	static void ErrorDialog(std::string);

protected:
	NSMenu* defaultMenu;
	AutoPtr<MenuMac> menu;
	NSMenu* nativeMenu;
	AutoPtr<MenuMac> contextMenu;
	AutoPtr<MenuMac> dockMenu;
	NSMenu* nativeDockMenu;
	NSView *savedDockView;
	NSObject* application;
	AutoPtr<MenuMac> activeMenu;
	AutoPtr<UserWindowMac> activeWindow;

	void InstallMenu (MenuItemMac*);
};

} // namespace Titanium

#endif
