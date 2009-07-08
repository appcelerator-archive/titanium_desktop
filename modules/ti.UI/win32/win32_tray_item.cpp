/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#include "../ui_module.h"

#define STUB() printf("Method is still a stub, %s:%i\n", __FILE__, __LINE__)

namespace ti
{
	std::vector<SharedPtr<Win32TrayItem> > Win32TrayItem::trayItems;
	
	Win32TrayItem::Win32TrayItem(SharedString iconPath, SharedKMethod cb)
	{
		this->callback = cb;
		this->CreateTrayIcon(*iconPath, std::string("Titanium Application"));
		trayItems.push_back(this);
	}
	
	Win32TrayItem::~Win32TrayItem()
	{
		this->Remove();
	}
	
	void Win32TrayItem::SetIcon(SharedString iconPath)
	{
		if (this->trayIconData == NULL)
		{
			// nothing to do
			return;
		}
	
		this->trayIconData->hIcon = (HICON) LoadImage(
				::GetModuleHandle(NULL), (*iconPath).c_str(), IMAGE_ICON,
				GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON),
				LR_DEFAULTCOLOR | LR_LOADFROMFILE);
	
		Shell_NotifyIcon(NIM_MODIFY, this->trayIconData);
	}
	
	void Win32TrayItem::SetMenu(SharedMenu menu)
	{
		this->menu = menu;
	}
	
	void Win32TrayItem::SetHint(SharedString hint)
	{
		if (this->trayIconData == NULL)
		{
			// nothing to do
			return;
		}
	
		lstrcpy(this->trayIconData->szTip, (*hint).c_str());
		Shell_NotifyIcon(NIM_MODIFY, this->trayIconData);
	}
	
	void Win32TrayItem::Remove()
	{
		if (this->trayIconData == NULL)
		{
			// nothing to do
			return;
		}
	
		Shell_NotifyIcon(NIM_DELETE, this->trayIconData);
	
		this->trayIconData = NULL;
	}

	void Win32TrayItem::ShowTrayMenu()
	{

		if (this->oldNativeMenu) {
			DestroyMenu(this->oldNativeMenu);
			this->oldNativeMenu = 0;
		}

		if (this->menu.isNull())
			return;

		SharedPtr<Win32Menu> win32menu = this->menu.cast<Win32Menu>();
		if (win32menu.isNull())
			return;

		this->oldNativeMenu = win32menu->CreateNative(false);
		POINT pt;
		GetCursorPos(&pt);
		TrackPopupMenu(this->oldNativeMenu, TPM_BOTTOMALIGN, 
			pt.x, pt.y, 0, this->trayIconData->hWnd, NULL);
	}
	
	/*static*/
	bool Win32TrayItem::ShowTrayMenu(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		int trayIconID = LOWORD(wParam);

		std::vector<SharedPtr<Win32TrayItem>>::iterator i = trayItems.begin();
		while (i != trayItems.end()) {
			Win32TrayItem* item = (*i++);
	
			if (item->trayIconData && item->trayIconData->uID == trayIconID) {
				item->ShowTrayMenu();
				return true;
			}
		}
		return false;
	
	}
	
	void Win32TrayItem::CreateTrayIcon(std::string &iconPath, std::string &caption)
	{
		SharedUserWindow uw = NULL;
	
		std::vector<SharedUserWindow>& windows = UIBinding::GetInstance()->GetOpenWindows();
		std::vector<SharedUserWindow>::iterator i = windows.begin();
		if (i != windows.end())
		{
			uw = *i;
		}
	
		SharedPtr<Win32UserWindow> wuw = (*i).cast<Win32UserWindow>();
	
		NOTIFYICONDATA* notifyIconData = new NOTIFYICONDATA;
		notifyIconData->cbSize = sizeof(NOTIFYICONDATA);
		notifyIconData->hWnd = wuw->GetWindowHandle();
		notifyIconData->uID = ++Win32UIBinding::nextItemId;
		notifyIconData->uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
		notifyIconData->uCallbackMessage = TI_TRAY_CLICKED;
		notifyIconData->hIcon = (HICON) LoadImage(
				::GetModuleHandle(NULL), iconPath.c_str(), IMAGE_ICON,
				GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON),
				LR_DEFAULTCOLOR | LR_LOADFROMFILE);
	
		lstrcpy(notifyIconData->szTip, caption.c_str());
	
		Shell_NotifyIcon(NIM_ADD, notifyIconData);
	
		this->trayIconData = notifyIconData;
	}
	
	/*static*/
	bool Win32TrayItem::InvokeLeftClickCallback(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		int trayIconID = LOWORD(wParam);
	
		return InvokeLeftClickCallback(trayIconID);
	}
	
	/*static*/
	bool Win32TrayItem::InvokeLeftClickCallback(int trayIconID)
	{
		for(size_t i = 0; i < trayItems.size(); i++)
		{
			SharedPtr<Win32TrayItem> item = trayItems[i];
	
			if(item->trayIconData && item->trayIconData->uID == trayIconID)
			{
				if(item->callback)
				{
					KMethod* cb = (KMethod*) item->callback;
	
					// TODO: Handle exceptions in some way
					try
					{
						ValueList args;
						cb->Call(args);
					}
					catch(...)
					{
						std::cout << "Menu callback failed" << std::endl;
					}
	
					return true;
				}
			}
		}
	
		return false;
	}
}
