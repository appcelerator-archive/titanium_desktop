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

#include "MenuItemWin.h"

#include "MenuWin.h"
#include "UserWindowWin.h"
#include "UIWin.h"
#include "../../ti.App/WindowConfig.h"

using namespace std;

namespace Titanium {

MenuItemWin::MenuItemWin(MenuItemType type)
	: MenuItem(type)
	, oldSubmenu(0)
	, wideOldLabel(::UTF8ToWide(label))
{
}

MenuItemWin::~MenuItemWin()
{
}

void MenuItemWin::SetLabelImpl(std::string newLabel)
{
	if (!this->IsSeparator())
	{
		this->wideOldLabel = ::UTF8ToWide(newLabel);;
		this->RecreateAllNativeItems();
	}

	if (this->nativeItems.size() > 0)
	{
		UserWindowWin::RedrawAllMenus();
	}
}

void MenuItemWin::SetIconImpl(std::string newIconPath)
{
	this->iconPath = newIconPath;

	if (!this->IsCheck() && !this->IsSeparator())
	{
		this->RecreateAllNativeItems();
	}

	if (this->nativeItems.size() > 0)
	{
		UserWindowWin::RedrawAllMenus();
	}
}

void MenuItemWin::SetStateImpl(bool newState)
{
	if (this->IsCheck())
	{
		this->RecreateAllNativeItems();
	}

	if (this->nativeItems.size() > 0)
	{
		UserWindowWin::RedrawAllMenus();
	}
}

void MenuItemWin::SetSubmenuImpl(AutoPtr<Menu> newSubmenu)
{
	if (!this->IsSeparator())
	{
		this->RecreateAllNativeItems();
		this->oldSubmenu = newSubmenu.cast<MenuWin>();
	}

	if (this->nativeItems.size() > 0)
	{
		UserWindowWin::RedrawAllMenus();
	}
}

void MenuItemWin::SetEnabledImpl(bool enabled)
{
	if (!this->IsSeparator())
	{
		this->RecreateAllNativeItems();
	}

	if (this->nativeItems.size() > 0)
	{
		UserWindowWin::RedrawAllMenus();
	}
}

void MenuItemWin::RecreateAllNativeItems()
{
	std::vector<NativeItemBits*>::iterator i = this->nativeItems.begin();
	while (i != this->nativeItems.end())
	{
		NativeItemBits* bits = (*i++);
		this->RecreateMenuItem(bits);
	}
}

void MenuItemWin::RecreateMenuItem(NativeItemBits* bits)
{
	int nativePosition = GetNativeMenuItemPosition(bits);
	MenuWin::RemoveItemAtFromNativeMenu(this, bits->parentMenu, nativePosition);
	MenuWin::InsertItemIntoNativeMenu(this, bits->parentMenu, true, nativePosition);
}

void MenuItemWin::CreateNative(
	LPMENUITEMINFO itemInfo, HMENU nativeParentMenu, bool registerNative)
{
	ZeroMemory(itemInfo, sizeof(MENUITEMINFO)); 
	itemInfo->cbSize = sizeof(MENUITEMINFO);
	itemInfo->wID = ++UIWin::nextItemId;
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

		AutoPtr<MenuWin> wsubmenu = this->submenu.cast<MenuWin>();
		if (!wsubmenu.isNull())
			nativeSubmenu = wsubmenu->CreateNative(registerNative);

		itemInfo->hSubMenu = nativeSubmenu;

		if (this->IsCheck())
		{
			itemInfo->fState |= this->GetState() ? MFS_CHECKED : MFS_UNCHECKED;
		}
		else if (!this->iconPath.empty())
		{
			HBITMAP bitmap = UIWin::LoadImageAsBitmap(iconPath,
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

void MenuItemWin::DestroyNative(NativeItemBits* bits)
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

void MenuItemWin::DestroyNative(HMENU nativeParent, int position)
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
int MenuItemWin::GetNativeMenuItemPosition(NativeItemBits* bits)
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
bool MenuItemWin::HandleClickEvent(HMENU nativeMenu, UINT position)
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

} // namespace Titanium
