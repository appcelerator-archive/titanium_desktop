/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#ifndef TI_WIN32_TRAY_ITEM_H_
#define TI_WIN32_TRAY_ITEM_H_

#include <kroll/base.h>
#include <kroll/kroll.h>
#include <windows.h>
#include <shellapi.h>
#include "../tray_item.h"
#include "win32_menu_item_impl.h"

namespace ti
{

class Win32TrayItem: public TrayItem
{
public:
	Win32TrayItem(SharedString iconPath, SharedKMethod cb);
	virtual ~Win32TrayItem();

	void SetIcon(SharedString iconPath);
	void SetMenu(SharedPtr<MenuItem> menu);
	void SetHint(SharedString hint);
	void Remove();

	static bool InvokeLeftClickCallback(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static bool InvokeLeftClickCallback(int trayIconID);
	static bool ShowTrayMenu(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static bool ShowTrayMenu(int trayIconID);
private:
	SharedKMethod callback;
	SharedPtr<Win32MenuItemImpl> trayMenu;
	HMENU trayMenuHandle;

	NOTIFYICONDATA* trayIconData;
	void CreateTrayIcon(std::string &iconPath, std::string &caption);
};

}

#endif /* TI_WIN32_TRAY_ITEM_H_ */
