/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#include "../ui_module.h"

using std::vector;
using std::map;
namespace ti
{
	Win32MenuItem::Win32MenuItem(MenuItemType type) :
		MenuItem(type),
		oldSubmenu(0),
		wideOldLabel(::UTF8ToWide(label))
	{
	}

	Win32MenuItem::~Win32MenuItem()
	{
	}

	void Win32MenuItem::SetLabelImpl(std::string newLabel)
	{
		if (!this->IsSeparator())
		{
			this->wideOldLabel = ::UTF8ToWide(newLabel);;
			this->RecreateAllNativeItems();
		}

		if (this->nativeItems.size() > 0)
		{
			Win32UserWindow::RedrawAllMenus();
		}
	}

	void Win32MenuItem::SetIconImpl(std::string newIconPath)
	{
		this->iconPath = newIconPath;

		if (!this->IsCheck() && !this->IsSeparator())
		{
			this->RecreateAllNativeItems();
		}

		if (this->nativeItems.size() > 0)
		{
			Win32UserWindow::RedrawAllMenus();
		}
	}

	void Win32MenuItem::SetStateImpl(bool newState)
	{
		if (this->IsCheck())
		{
			this->RecreateAllNativeItems();
		}

		if (this->nativeItems.size() > 0)
		{
			Win32UserWindow::RedrawAllMenus();
		}
	}

	void Win32MenuItem::SetSubmenuImpl(AutoMenu newSubmenu)
	{
		if (!this->IsSeparator())
		{
			this->RecreateAllNativeItems();
			this->oldSubmenu = newSubmenu.cast<Win32Menu>();
		}

		if (this->nativeItems.size() > 0)
		{
			Win32UserWindow::RedrawAllMenus();
		}
	}

	void Win32MenuItem::SetEnabledImpl(bool enabled)
	{
		if (!this->IsSeparator())
		{
			this->RecreateAllNativeItems();
		}

		if (this->nativeItems.size() > 0)
		{
			Win32UserWindow::RedrawAllMenus();
		}
	}

	void Win32MenuItem::RecreateAllNativeItems()
	{
		std::vector<NativeItemBits*>::iterator i = this->nativeItems.begin();
		while (i != this->nativeItems.end())
		{
			NativeItemBits* bits = (*i++);
			this->RecreateMenuItem(bits);
		}
	}

	void Win32MenuItem::RecreateMenuItem(NativeItemBits* bits)
	{
		int nativePosition = GetNativeMenuItemPosition(bits);
		Win32Menu::RemoveItemAtFromNativeMenu(this, bits->parentMenu, nativePosition);
		Win32Menu::InsertItemIntoNativeMenu(this, bits->parentMenu, true, nativePosition);
	}

	void Win32MenuItem::CreateNative(
		LPMENUITEMINFO itemInfo, HMENU nativeParentMenu, bool registerNative)
	{
		ZeroMemory(itemInfo, sizeof(MENUITEMINFO)); 
		itemInfo->cbSize = sizeof(MENUITEMINFO);
		itemInfo->wID = ++Win32UIBinding::nextItemId;
		itemInfo->dwItemData = (ULONG_PTR) this;
		itemInfo->fMask = MIIM_ID | MIIM_FTYPE | MIIM_DATA;

		HMENU nativeSubmenu = 0;
		if (this->IsSeparator())
		{
			itemInfo->fType = MFT_SEPARATOR;
		}
		else
		{
			itemInfo->fMask = itemInfo->fMask | MIIM_STRING | MIIM_SUBMENU | MIIM_STATE;
			itemInfo->fType = MFT_STRING;
			itemInfo->fState = this->IsEnabled() ? MFS_ENABLED : MFS_DISABLED;
			itemInfo->dwTypeData = (LPWSTR) this->wideOldLabel.c_str();

			AutoPtr<Win32Menu> wsubmenu = this->submenu.cast<Win32Menu>();
			if (!wsubmenu.isNull())
				nativeSubmenu = wsubmenu->CreateNative(registerNative);

			itemInfo->hSubMenu = nativeSubmenu;

			if (this->IsCheck())
			{
				itemInfo->fState |= this->GetState() ? MFS_CHECKED : MFS_UNCHECKED;
			}
			else if (!this->iconPath.empty())
			{
				HBITMAP bitmap = Win32UIBinding::LoadImageAsBitmap(iconPath,
					GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON));
				if (bitmap)
				{
					itemInfo->fMask = itemInfo->fMask | MIIM_BITMAP;
					itemInfo->hbmpItem = bitmap;
				}
				else
				{
					std::string error = Win32Utils::QuickFormatMessage(GetLastError());
					Logger::Get("UI.MenuItem")->Error("Could not load icon (%s): %s",
						iconPath.c_str(), error.c_str());
				}
			}
		}

		if (registerNative)
		{
			NativeItemBits* bits = new NativeItemBits;
			bits->id = itemInfo->wID;
			bits->parentMenu = nativeParentMenu;
			bits->submenu = nativeSubmenu;
			this->nativeItems.push_back(bits);
		}
	}

	void Win32MenuItem::DestroyNative(NativeItemBits* bits)
	{
		// Erase the reference to this item in our registry
		vector<NativeItemBits*>::iterator i = nativeItems.begin();
		while (i != nativeItems.end())
		{
			if (*i == bits)
				i = nativeItems.erase(i);
			else
				i++;
		}

		if (bits->submenu)
		{
			this->oldSubmenu->DestroyNative(bits->submenu);
		}

		delete bits;
	}

	void Win32MenuItem::DestroyNative(HMENU nativeParent, int position)
	{
		UINT nativeId = GetMenuItemID(nativeParent, position);
		HMENU submenu = GetSubMenu(nativeParent, position);

		vector<NativeItemBits*>::iterator i = nativeItems.begin();
		while (i != nativeItems.end())
		{
			NativeItemBits* b = *i++;

			if ((b->submenu && submenu) ||
				(!b->submenu && b->id == nativeId))
			{
				this->DestroyNative(b);
				return;
			}
		}
	}

	/*static*/
	int Win32MenuItem::GetNativeMenuItemPosition(NativeItemBits* bits)
	{
		int count = GetMenuItemCount(bits->parentMenu);
		for (int i = 0; i < count; i++)
		{

			if ((bits->submenu && GetSubMenu(bits->parentMenu, i) == bits->submenu) ||
				(bits->id == GetMenuItemID(bits->parentMenu, i)))
				return i;
		}

		throw ValueException::FromString("Could not find native item index in native menu.");
	}

	/*static*/
	bool Win32MenuItem::HandleClickEvent(HMENU nativeMenu, UINT position)
	{
		MENUITEMINFO itemInfo;
		ZeroMemory(&itemInfo, sizeof(MENUITEMINFO)); 
		itemInfo.cbSize = sizeof(MENUITEMINFO);
		itemInfo.fMask = MIIM_DATA;
		GetMenuItemInfo(nativeMenu, position, TRUE, &itemInfo);

		if (itemInfo.dwItemData)
		{
			MenuItem* item = (MenuItem*) itemInfo.dwItemData;
			item->HandleClickEvent(NULL);
			return true;
		}
			
		return false;
	}
}


