/**s
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#include "../ui_module.h"

#define STUB() printf("Method is still a stub, %s:%i\n", __FILE__, __LINE__)

namespace ti
{
	std::vector<AutoPtr<Win32TrayItem> > Win32TrayItem::trayItems;
	Win32TrayItem::Win32TrayItem(std::string& iconURL, SharedKMethod cb) :
		TrayItem(iconURL),
		callback(cb),
		oldNativeMenu(0),
		trayIconData(0)
	{
		AutoUserWindow uw = NULL;
		std::vector<AutoUserWindow>& windows = UIBinding::GetInstance()->GetOpenWindows();
		std::vector<AutoUserWindow>::iterator i = windows.begin();
		if (i != windows.end())
		{
			uw = *i;
		}

		AutoPtr<Win32UserWindow> wuw = (*i).cast<Win32UserWindow>();
		NOTIFYICONDATA* notifyIconData = new NOTIFYICONDATA;
		notifyIconData->cbSize = sizeof(NOTIFYICONDATA);
		notifyIconData->hWnd = wuw->GetWindowHandle();
		notifyIconData->uID = ++Win32UIBinding::nextItemId;
		notifyIconData->uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
		notifyIconData->uCallbackMessage = TI_TRAY_CLICKED;

		HICON icon = Win32UIBinding::LoadImageAsIcon(iconPath);
		notifyIconData->hIcon = icon;

		lstrcpy(notifyIconData->szTip, "Titanium Application");
		Shell_NotifyIcon(NIM_ADD, notifyIconData);
		this->trayIconData = notifyIconData;

		trayItems.push_back(this);
	}
	
	Win32TrayItem::~Win32TrayItem()
	{
		this->Remove();
	}
	
	void Win32TrayItem::SetIcon(std::string& iconPath)
	{
		if (this->trayIconData == NULL)
		{
			HICON icon = Win32UIBinding::LoadImageAsIcon(iconPath);
			this->trayIconData->hIcon = icon;
			Shell_NotifyIcon(NIM_MODIFY, this->trayIconData);
		}
	}
	
	void Win32TrayItem::SetMenu(AutoMenu menu)
	{
		this->menu = menu;
	}
	
	void Win32TrayItem::SetHint(std::string& hint)
	{
		if (this->trayIconData == NULL)
		{
			// NotifyIconData.szTip has 128 character limit.
			ZeroMemory(this->trayIconData->szTip, 128);
			// make sure we don't overflow the static buffer.
			lstrcpyn(this->trayIconData->szTip, hint.c_str(), 128);
			Shell_NotifyIcon(NIM_MODIFY, this->trayIconData);
		}
	}
	
	void Win32TrayItem::Remove()
	{
		if (this->trayIconData == NULL)
		{
			Shell_NotifyIcon(NIM_DELETE, this->trayIconData);
			this->trayIconData = NULL;
		}
	}

	void Win32TrayItem::HandleRightClick()
	{
		if (this->oldNativeMenu) {
			DestroyMenu(this->oldNativeMenu);
			this->oldNativeMenu = 0;
		}

		if (this->menu.isNull())
			return;

		AutoPtr<Win32Menu> win32menu = this->menu.cast<Win32Menu>();
		if (win32menu.isNull())
			return;

		this->oldNativeMenu = win32menu->CreateNative(false);
		POINT pt;
		GetCursorPos(&pt);
		TrackPopupMenu(this->oldNativeMenu, TPM_BOTTOMALIGN, 
			pt.x, pt.y, 0, this->trayIconData->hWnd, NULL);
	}

	void Win32TrayItem::HandleLeftClick()
	{
		if (callback.isNull())
			return;

		try {
			ValueList args;
			callback->Call(args);

		} catch (ValueException& e) {
			Logger* logger = Logger::Get("UI.Win32TrayItem");
			SharedString ss = e.DisplayString();
			logger->Error("Tray icon callback failed: %s", ss->c_str());
		}
	}
	
	UINT Win32TrayItem::GetId()
	{
		return this->trayIconData->uID;
	}

	/*static*/
	void Win32TrayItem::HandleClickEvent(
		HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		UINT button = (UINT) lParam;
		int id = LOWORD(wParam);

		for (size_t i = 0; i < trayItems.size(); i++) {
			AutoPtr<Win32TrayItem> item = trayItems[i];

			if (item->GetId() == id && button == WM_LBUTTONDOWN) {
				item->HandleLeftClick();
			} else if (item->GetId() == id && button == WM_RBUTTONDOWN) {
				item->HandleRightClick();
			}
		}
	}
}
