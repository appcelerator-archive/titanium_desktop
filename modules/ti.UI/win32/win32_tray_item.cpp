/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#include "win32_tray_item.h"
#include "win32_user_window.h"
#include "../user_window.h"

#define STUB() printf("Method is still a stub, %s:%i\n", __FILE__, __LINE__)

namespace ti
{
std::vector<Win32TrayItem *> trayItems;

Win32TrayItem::Win32TrayItem(SharedString iconPath, SharedKMethod cb)
{
	this->callback = cb;
	this->trayMenu = NULL;

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
void Win32TrayItem::SetMenu(SharedPtr<MenuItem> menu)
{
	SharedPtr<Win32MenuItemImpl> menuToUse = menu.cast<Win32MenuItemImpl>();
	if (menuToUse == this->trayMenu)
		return;	// nothing to change since it's the same menu

	if (!this->trayMenu.isNull())
	{
		this->trayMenu->ClearRealization(trayMenuHandle);
	}

	// create menu handle
	this->trayMenu = menuToUse;
	this->trayMenuHandle = this->trayMenu->GetMenu();
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
/*static*/
bool Win32TrayItem::ShowTrayMenu(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int trayIconID = LOWORD(wParam);

	return ShowTrayMenu(trayIconID);
}
/*static*/
bool Win32TrayItem::ShowTrayMenu(int trayIconID)
{
	for(size_t i = 0; i < trayItems.size(); i++)
	{
		Win32TrayItem* item = trayItems[i];

		if(item->trayIconData && item->trayMenu && item->trayIconData->uID == trayIconID)
		{
			// handle the tray menu
			POINT pt;
			GetCursorPos(&pt);
			TrackPopupMenu(item->trayMenuHandle,
				TPM_BOTTOMALIGN,
				pt.x, pt.y, 0,
				item->trayIconData->hWnd, NULL);

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
	notifyIconData->uID = Win32MenuItemImpl::nextMenuUID();
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
		Win32TrayItem* item = trayItems[i];

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
