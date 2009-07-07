/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#include <kroll/kroll.h>
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
		std::vector<HEMNU>::iterator i = this->nativeMenus.begin();
		while (i != this->nativeMenus.end()) {
			this->ClearNativeMenu((*i++));
		}
		nativeMenus.clear();
	}

	void Win32Menu::AppendItemImpl(SharedMenuItem item)
	{
		SharedPtr<Win32MenuItem> win32Item = item.cast<Win32MenuItem>();

		std::vector<HMENU>::iterator i = this->nativeMenus.begin();
		while (i != this->nativeMenus.end()) {
			HMENU nativeMenu = (*i++);
			Win32Menu::InsertItemIntoNativeMenu(win32Item, nativeMenu, true);
		}

		this->oldChildren = this->children;
	}

	void Win32Menu::InsertItemAtImpl(SharedMenuItem item, unsigned int index)
	{
		SharedPtr<Win32MenuItem> win32Item = item.cast<Win32MenuItem>();
		
		std::vector<HMENU>::iterator i = this->nativeMenus.begin();
		while (i != this->nativeMenus.end())
		{
			HMENU nativeMenu = (*i++);
			Win32Menu::InsertItemIntoNativeMenu(win32Item, nativeMenu, true, index);
		}

		this->oldChildren = this->children;
	}

	void Win32Menu::RemoveItemAtImpl(unsigned int index)
	{
		SharedPtr<Win32MenuItem> win32Item = oldChildren.at(index).cast<Win32MenuItem>();

		std::vector<Win32MenuShell*>::iterator i = this->nativeMenus.begin();
		while (i != this->nativeMenus.end())
		{
			HMENU nativeMenu = (*i++);
			Win32Menu::RemoveItemAtFromNativeMenu(win32Item, nativeMenu, index);
		}

		this->oldChildren = this->children;
	}

	void Win32Menu::ClearImpl()
	{
		std::vector<Win32MenuShell*>::iterator i = this->nativeMenus.begin();
		while (i != this->nativeMenus.end())
		{
			HMENU nativeMenu = (*i++);
			this->ClearNativeMenu(nativeMenu);
		}

		this->oldChildren = this->children;
	}

	void Win32Menu::InsertItemIntoNativeMenu(
		Win32MenuItem* item, HMENU nativeMenu, bool registerNative, int position )
	{
		MENUITEMINFO itemInfo;
		item->CreateNative(&itemInfo, nativeMenu, registerNative);
		success = InsertMenuItem(nativeMenu, position, TRUE, &itemInfo);

		if (!success) {
			std::string error = Win32Utils::QuickFormatMessage(GetLastError());
			throw ValueException::FromString("Could not insert native item: " + error);
		}
	}

	void Win32Menu:RemoveItemAtFromNativeMenu(
		Win32MenuItem* item, HMENU nativeMenu, int position)
	{
		UINT nativeId = GetMenuItemID(nativeMenu, position);
		item->DestroyNative(nativeId);

		BOOL success = DeleteMenu(nativeMenu, position, MF_BYPOSITION);
		if (!success) {
			std::string error = Win32Utils::QuickFormatMessage(GetLastError());
			throw ValueException::FromString("Could not remove native item: " + error);
		}
	}

	void Win32Menu::ClearNativeMenu(HMENU nativeMenu)
	{
		int index = GetMenuItemCount(nativeMenu) - 1;
		while (index >= 0) {
			SharedPtr<Win32MenuItem> win32Item =
				this->oldChildren.at(index).cast<Win32MenuItem>();
			RemoveItemAtFromNativeMenu(win32Item.get(), nativeMenu, index);
		}
	}

	void Win32Menu::DestroyNative(::Win32MenuShell* nativeMenu)
	{
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
		this->AddChildrenToNativeMenu(nativeMenu, registerNative);

		if (registerNative)
			this->nativeMenus.push_back(nativeMenu);

		return nativeMenu;
	}

	HMENU Win32Menu::CreateNativeTopLevel(bool registerNative)
	{
		HMENU nativeMenu = CreateMenu();
		this->AddChildrenToNativeMenu(nativeMenu, registerNative);

		if (registerNative)
			this->nativeMenus.push_back(nativeMenu);

		return nativeMenu;
	}

	void Win32Menu::AddChildrenToNativeMenu(HMENU nativeMenu, bool registerNative)
	{
		vector<SharedMenuItem>::iterator i = this->children.begin();
		while (i != this->children.end()) {
			SharedMenuItem item = *i++;
			SharedPtr<Win32MenuItem> win32Item = item.cast<Win32MenuItem>();
			Win32Menu::InsertItemIntonativeMenu(win32Item.get(), nativeMenu, true);
		}
	}
}
