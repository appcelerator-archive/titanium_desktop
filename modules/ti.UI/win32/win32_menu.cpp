/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#include "../ui_module.h" 

namespace ti
{
	using std::vector;
	using std::map;
	using std::string;

	Win32Menu::Win32Menu() :
		Menu()
	{
	}

	Win32Menu::~Win32Menu()
	{
		std::vector<HMENU>::iterator i = this->nativeMenus.begin();
		while (i != this->nativeMenus.end()) {
			this->ClearNativeMenu((*i++));
		}
		nativeMenus.clear();
	}

	void Win32Menu::AppendItemImpl(AutoMenuItem item)
	{
		AutoPtr<Win32MenuItem> win32Item = item.cast<Win32MenuItem>();

		std::vector<HMENU>::iterator i = this->nativeMenus.begin();
		while (i != this->nativeMenus.end()) {
			HMENU nativeMenu = (*i++);
			Win32Menu::InsertItemIntoNativeMenu(win32Item, nativeMenu, true);
		}

		this->oldChildren = this->children;

		if (this->nativeMenus.size() > 0) {
			Win32UserWindow::RedrawAllMenus();
		}
	}

	void Win32Menu::InsertItemAtImpl(AutoMenuItem item, unsigned int index)
	{
		AutoPtr<Win32MenuItem> win32Item = item.cast<Win32MenuItem>();
		
		std::vector<HMENU>::iterator i = this->nativeMenus.begin();
		while (i != this->nativeMenus.end())
		{
			HMENU nativeMenu = (*i++);
			Win32Menu::InsertItemIntoNativeMenu(win32Item, nativeMenu, true, index);
		}

		this->oldChildren = this->children;

		if (this->nativeMenus.size() > 0) {
			Win32UserWindow::RedrawAllMenus();
		}
	}

	void Win32Menu::RemoveItemAtImpl(unsigned int index)
	{
		AutoPtr<Win32MenuItem> win32Item = oldChildren.at(index).cast<Win32MenuItem>();

		std::vector<HMENU>::iterator i = this->nativeMenus.begin();
		while (i != this->nativeMenus.end())
		{
			HMENU nativeMenu = (*i++);
			Win32Menu::RemoveItemAtFromNativeMenu(win32Item, nativeMenu, index);
		}

		this->oldChildren = this->children;

		if (this->nativeMenus.size() > 0) {
			Win32UserWindow::RedrawAllMenus();
		}
	}

	void Win32Menu::ClearImpl()
	{
		std::vector<HMENU>::iterator i = this->nativeMenus.begin();
		while (i != this->nativeMenus.end())
		{
			HMENU nativeMenu = (*i++);
			this->ClearNativeMenu(nativeMenu);
		}

		this->oldChildren = this->children;

		if (this->nativeMenus.size() > 0) {
			Win32UserWindow::RedrawAllMenus();
		}
	}

	/*static*/
	void Win32Menu::InsertItemIntoNativeMenu(
		Win32MenuItem* item, HMENU nativeMenu, bool registerNative, int position)
	{
		
		// A -1 value means to stick the new item at the end
		if (position == -1) {
			position = GetMenuItemCount(nativeMenu);
			if (position == 0) position = 1;
		}

		MENUITEMINFO itemInfo;
		item->CreateNative(&itemInfo, nativeMenu, registerNative);
		BOOL success = InsertMenuItem(nativeMenu, position, TRUE, &itemInfo);

		if (!success) {
			std::string error = Win32Utils::QuickFormatMessage(GetLastError());
			throw ValueException::FromString("Could not insert native item: " + error);
		}

	}

	/*static*/
	void Win32Menu::RemoveItemAtFromNativeMenu(
		Win32MenuItem* item, HMENU nativeMenu, int position)
	{
		item->DestroyNative(nativeMenu, position);

		BOOL success = DeleteMenu(nativeMenu, position, MF_BYPOSITION);
		if (!success) {
			std::string error = Win32Utils::QuickFormatMessage(GetLastError());
			throw ValueException::FromString("Could not remove native item: " + error);
		}
	}

	void Win32Menu::ClearNativeMenu(HMENU nativeMenu)
	{
		for (int i = GetMenuItemCount(nativeMenu) - 1; i >= 0; i--) {
			AutoPtr<Win32MenuItem> win32Item =
				this->oldChildren.at(i).cast<Win32MenuItem>();
			RemoveItemAtFromNativeMenu(win32Item.get(), nativeMenu, i);
		}
	}

	void Win32Menu::DestroyNative(HMENU nativeMenu)
	{
		if (!nativeMenu) {
			return;
		}

		// Remove the native menu from our list of known native menus
		std::vector<HMENU>::iterator i = this->nativeMenus.begin();
		while (i != this->nativeMenus.end()) {
			if (*i == nativeMenu)
				i = this->nativeMenus.erase(i);
			else
				i++;
		}

		// Clear the native menu and release, so that children will be freed
		this->ClearNativeMenu(nativeMenu);
	}

	HMENU Win32Menu::CreateNative(bool registerNative)
	{
		HMENU nativeMenu = CreatePopupMenu();
		ApplyNotifyByPositionStyleToNativeMenu(nativeMenu);
		this->AddChildrenToNativeMenu(nativeMenu, registerNative);

		if (registerNative)
			this->nativeMenus.push_back(nativeMenu);

		return nativeMenu;
	}

	HMENU Win32Menu::CreateNativeTopLevel(bool registerNative)
	{
		HMENU nativeMenu = CreateMenu();
		ApplyNotifyByPositionStyleToNativeMenu(nativeMenu);
		this->AddChildrenToNativeMenu(nativeMenu, registerNative);

		if (registerNative)
			this->nativeMenus.push_back(nativeMenu);

		return nativeMenu;
	}

	void Win32Menu::AddChildrenToNativeMenu(HMENU nativeMenu, bool registerNative)
	{
		vector<AutoMenuItem>::iterator i = this->children.begin();
		while (i != this->children.end()) {
			AutoMenuItem item = *i++;
			AutoPtr<Win32MenuItem> win32Item = item.cast<Win32MenuItem>();
			Win32Menu::InsertItemIntoNativeMenu(win32Item.get(), nativeMenu, registerNative);
		}
	}

	/*static*/
	void Win32Menu::ApplyNotifyByPositionStyleToNativeMenu(HMENU nativeMenu)
	{
		MENUINFO menuInfo;
		ZeroMemory(&menuInfo, sizeof(MENUINFO)); 
		menuInfo.cbSize = sizeof(MENUINFO);
		menuInfo.fMask = MIM_APPLYTOSUBMENUS | MIM_STYLE;
		menuInfo.dwStyle = MNS_CHECKORBMP | MNS_NOTIFYBYPOS; 
		BOOL success = SetMenuInfo(nativeMenu, &menuInfo);

		if (!success) {
			std::string error = Win32Utils::QuickFormatMessage(GetLastError());
			throw ValueException::FromString("Could not set native menu style: " + error);
		}
	}

}
