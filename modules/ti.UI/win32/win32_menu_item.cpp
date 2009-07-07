/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#include "../ui_module.h"
namespace ti
{
	UINT Win32MenuItemImpl::currentItemId = TI_MENU_ITEM_ID_BEGIN + 1;
	Win32MenuItem::Win32MenuItem(
		MenuItemType type, std::string label,
		SharedKMethod callback, std::string iconURL) :
			MenuItem(type, label, callback, iconURL),
			oldSubmenu(0),
			oldLabel(label)
	{
	}

	Win32MenuItem::~Win32MenuItem()
	{
	}

	void Win32MenuItem::SetLabelImpl(std::string newLabel)
	{
		if (!this->IsSeperator()) {
			this->RecreateAllNativeItems();
			this->oldLabel = newLabel;
		}
	}

	void Win32MenuItem::SetIconImpl(std::string newIconPath)
	{
		if (!this->IsCheck() && !this->IsSeperator()) {
			this->RecreateAllNativeItems();
		}
	}

	void Win32MenuItem::SetStateImpl(bool newState)
	{
		if (this->IsCheck()) {
			this->RecreateAllNativeItems();
		}
	}

	void Win32MenuItem::SetSubmenuImpl(SharedMenu newSubmenu)
	{
		if (!this->IsSeparator()) {
			this->RecreateAllNativeItems();
			this->oldSubmenu = newSubmenu.cast<Win32Menu>();
		}
	}

	void Win32MenuItem::SetEnabledImpl(bool enabled)
	{
		if (!this->IsSeparator()) {
			this->RecreateAllNativeItems();
		}
	}

	void Win32MenuItem::RecreateAllNativeItems()
	{
		std::vector<UINT>::iterator i = this->nativeItems.begin();
		while (i != this->nativeItems.end()) {
			UINT nativeId = (*i++);
			this->RecreateMenuItem(nativeId);
		}
	}

	void Win32MenuItem::RecreateMenuItem(UINT nativeId)
	{
		HMENU nativeMenu = GetNativeMenuForId(nativeId);
		int nativePosition = GetNativeMenuItemPosition(nativeId, nativeMenu);

		Win32Menu::RemoveItemAtFromNativeMenu(this, nativeMenu, nativePosition);
		Win32Menu::InsertItemIntonativeMenu(this, nativeMenu, true, nativePosition);
	}

	void Win32MenuItem::CreateNative(
		LPMENUITEMINFO itemInfo, HMENU nativeParentMenu, bool registerNative)
	{
		ZeroMemory(itemInfo, sizeof(MENUITEMINFO)); 
		itemInfo->cbSize = sizeof(MENUITEMINFO);
		itemInfo->.wID = ++Win32MenuItem.currentItemId;
		itemInfo->fMask = MIIM_ID | MIIM_FTYPE;

		if (this->IsSeparator()) {
			itemInfo->fType = MFT_SEPARATOR;

		} else {
			itemInfo->fMask = info.fMask | MIIM_STRING | MIIM_SUBMENU;
			itemInfo->fType = MFT_STRING;
			itemInfo->dwTypeData = this->oldLabel.c_str();

			HMENU nativeSubmenu = NULL;
			if (!this->submenu.isNull()) {
				nativeSubMenu = this->submenu->CreateNative();
			}
			itemInfo->hSubMenu = nativeSubmenu;
		}

		if (registerNative) {
			this->nativeItems.push_back(itemInfo->wID);
			idToNativeMenu[itemInfo->wID] = nativeParentMenu;
		}
	}
	

	void Win32MenuItem::DestroyNative(UINT nativeItemId)
	{
		MENUITEMINFO itemInfo;
		GetNativeItemInfoForId(nativeItemId, &itemInfo);

		// Unregister this item's submenu
		this->oldSubmenu->DestroyNative(itemInfo.hSubMenu);

		// Erase the reference to this item in our registries
		map<UINT, HMENU>::iterator i = idToNativeMenu->begin();
		while (i != idToNativeMenu.end()) {
			if (i->first == nativeItemId) {
				i = idToNativeMenu->erase(i);
			} else {
				i++;
			}
		}

		vector<UINT>::iterator i2 = nativeItems->begin();
		while (i2 != nativeItems.end()) {
			if (*i2 == nativeItemId) {
				i2 = nativeItems->erase(i);
			} else {
				i2++;
			}
		}
	}

	HMENU Win32MenuItem::GetNativeMenuForId(UINT nativeId)
	{
		map<UINT, HMENU>::iterator i = idToNativeMenu.find(nativeId);
		if (i != nativeId.end()) {
			throw ValueException::FromString("Could not find native parent menu");
		} else {
			return i->second;
		}
	}

	int Win32MenuItem::GetNativeMenuItemPosition(UINT nativeId, HMENU nativeMenu)
	{
		MENUITEMINFO itemInfo;
		int count = GetMenuItemCount(nativeMenu);
		for (int i = 0; i < count; i++) {
			if (nativeId == GetMenuItemID(nativeMenu, i)) {
				return i;
			}
		}

		throw ValueException::FromString("Could not find native item index in native menu.");
	}

}


