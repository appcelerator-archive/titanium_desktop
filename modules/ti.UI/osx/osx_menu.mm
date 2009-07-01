/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#include <kroll/kroll.h>
#include "../ui_module.h" 

// The front offset accounts for the appplication and edit menus:w
// The rear offset accounts for the window and help menus
#define MAINMENU_FRONT_OFFSET 2
#define MAINMENU_REAR_OFFSET 2
namespace ti
{
	using std::vector;
	using std::string;

	OSXMenu::OSXMenu() :
		Menu()
	{
	}

	OSXMenu::~OSXMenu()
	{
		std::vector<NSMenu*>::iterator i = this->nativeMenus.begin();
		while (i != this->nativeMenus.end()) {
			this->ClearNativeMenu((*i++));
		}
		nativeMenus.clear();
	}

	void OSXMenu::AppendItemImpl(SharedMenuItem item)
	{
		this->UpdateNativeMenus();
	}

	void OSXMenu::InsertItemAtImpl(SharedMenuItem item, unsigned int index)
	{
		this->UpdateNativeMenus();
	}

	void OSXMenu::RemoveItemAtImpl(unsigned int index)
	{
		this->UpdateNativeMenus();
	}

	void OSXMenu::ClearImpl()
	{
		this->Clear();
	}

	void OSXMenu::Clear()
	{
		this->UpdateNativeMenus();
	}

	/*static*/
	void OSXMenu::ClearNativeMenu(NSMenu* nativeMenu)
	{
		SEL getMenuItemSelector = @selector(getMenuItem:);

		while ([nativeMenu numberOfItems] > 0)
		{
			NSMenuItem* nativeItem = [nativeMenu itemAtIndex: 0];
			id delegate = [nativeItem target];

			// This delegate is one of our OSXMenuItem delgates, so grab its
			// OSXMenuItem reference and tell it to destroy this native menu item
			if (delegate && [delegate respondsToSelector:getMenuItemSelector]) {
				OSXMenuItem* menuItem = (OSXMenuItem*) [delegate performSelector:getMenuItemSelector];
				menuItem->DestroyNative(nativeItem);
			}
			[nativeMenu removeItemAtIndex:0];
		}
	}

	void OSXMenu::DestroyNative(NSMenu* nativeMenu)
	{
		// Remove the native menu from our list of known native menus
		std::vector<NSMenu*>::iterator i = this->nativeMenus.begin();
		while (i != this->nativeMenus.end()) {
			if (*i == nativeMenu) {
				i = this->nativeMenus.erase(i);
			} else {
				i++;
			}
		}

		// Clear the native menu and release it
		this->ClearNativeMenu(nativeMenu);
		[nativeMenu release];
	}

	void OSXMenu::UpdateNativeMenus()
	{
		std::vector<NSMenu*>::iterator i = this->nativeMenus.begin();
		while (i != this->nativeMenus.end()) {
			OSXMenu::UpdateNativeMenu(*i);
		}
	}

	/*static*/
	void OSXMenu::UpdateNativeMenu(NSMenu* nativeMenu)
	{
		OSXUIBinding* binding = dynamic_cast<OSXUIBinding*>(UIBinding::GetInstance());
		// If this native menu is the application's current main menu, we
		// want to replace replace that menu with a fresh copy reflecting
		// the modifications.
		if (nativeMenu == [NSApp mainMenu]) {
			binding->RefreshMainMenu();

		// Otherwise, just mark this menu as dirty and the next time OS X
		// shows it, it will trigger a redraw.
		} else {
			[[nativeMenu delegate] performSelector:@selector(markDirty:)];
		}
	}

	NSMenu* OSXMenu::CreateNative(bool registerNative)
	{
		// This title should be set by the callee - see OSXMenuItem::NSMenuSetSubmenu
		NSMenu* menu = [[NSMenu alloc] initWithTitle:@"TopLevelMenu"];
		[menu setAutoenablesItems:NO];
		this->AddChildrenToNativeMenu(menu, registerNative);

		if (registerNative)
		{
			this->nativeMenus.push_back(menu);
		}
		return menu;
	}

	void OSXMenu::FillNativeMainMenu(NSMenu* defaultMenu, NSMenu* nativeMainMenu)
	{
		OSXMenu::CopyMenu(defaultMenu, nativeMainMenu);
		this->AddChildrenToNativeMenu(nativeMainMenu, true, true);

		// The main menu needs all NSMenuItems in it to have submenus for 
		// proper display. So we just add empty submenus to those items missing them.
		OSXMenu::EnsureAllItemsHaveSubmenus(nativeMainMenu);

		this->nativeMenus.push_back(nativeMainMenu);
	}

	void OSXMenu::AddChildrenToNativeMenu(NSMenu* nativeMenu, bool registerNative, bool mainMenu)
	{
		vector<SharedMenuItem>::iterator i = this->children.begin();
		while (i != this->children.end()) {
			SharedMenuItem item = *i++;
			SharedPtr<OSXMenuItem> osxItem = item.cast<OSXMenuItem>();
			NSMenuItem* nativeItem = osxItem->CreateNative(registerNative);

			int rearOffset = mainMenu ?  MAINMENU_REAR_OFFSET : 0;
			int index = [nativeMenu numberOfItems] - rearOffset;
			[nativeMenu insertItem:nativeItem atIndex:index];
		}
		[nativeMenu sizeToFit];
	}

	void OSXMenu::AddChildrenToNSArray(NSMutableArray* array)
	{
		vector<SharedMenuItem>::iterator i = this->children.begin();
		while (i != this->children.end())
		{
			SharedMenuItem item = *i++;
			SharedPtr<OSXMenuItem> osxItem = item.cast<OSXMenuItem>();
			NSMenuItem* nsItem = osxItem->CreateNative(false);
			[array addObject:nsItem];
			[nsItem release];
		}
	}

	/*static*/
	void OSXMenu::CopyMenu(NSMenu* from, NSMenu* to)
	{
		[to setTitle:[from title]];
		[to setAutoenablesItems:NO];
		for (int i = 0; i < [from numberOfItems]; i++)
		{
			NSMenuItem* duplicateItem = OSXMenu::CopyMenuItem([from itemAtIndex:i]);
			[to addItem:duplicateItem];
		}
	}

	/*static*/
	NSMenuItem* OSXMenu::CopyMenuItem(NSMenuItem* item)
	{
		if ([item isSeparatorItem])
		{
			return [NSMenuItem separatorItem];
		}
		else
		{
			NSMenuItem* duplicate = [[NSMenuItem alloc] 
				initWithTitle:[item title]
				action:[item action]
				keyEquivalent:[item keyEquivalent]];
			[duplicate setState:[item state]];
			[duplicate setTag:[item tag]];
			[duplicate setEnabled:[item isEnabled]];
			[duplicate setKeyEquivalentModifierMask:[item keyEquivalentModifierMask]];

			if ([item submenu] != nil)
			{
				NSMenu* newSubmenu = [[NSMenu alloc] init];
				OSXMenu::CopyMenu([item submenu], newSubmenu);
				[duplicate setSubmenu:newSubmenu];
				[newSubmenu release];
			}
			return duplicate;
		}

	}

	/*static*/
	bool OSXMenu::IsNativeMenuAMainMenu(NSMenu* menu)
	{
		return ([menu numberOfItems] > 0 &&
			[[[[menu itemAtIndex:0] submenu] title] isEqualToString:@"Apple"]);
	}

	/*static*/
	NSMenu* OSXMenu::GetWindowMenu(NSMenu* menu)
	{
		if (IsNativeMenuAMainMenu(menu))
		{
			return [[menu itemAtIndex:([menu numberOfItems] - 2)] submenu];
		}
		else
		{
			return nil;
		}
	}

	/*static*/
	NSMenu* OSXMenu::GetAppleMenu(NSMenu* menu)
	{
		if (IsNativeMenuAMainMenu(menu))
		{
			return [[menu itemAtIndex:0] submenu];
		}
		else
		{
			return nil;
		}
	}

	/*static*/
	NSMenu* OSXMenu::GetServicesMenu(NSMenu* menu)
	{
		if (!IsNativeMenuAMainMenu(menu))
		{
			return nil;
		}

		NSMenu* submenu = [[menu itemAtIndex:0] submenu];
		if (submenu == nil)
		{
			return nil;
		}

		for (int i = 0; i < [submenu numberOfItems]; i++)
		{
			NSMenuItem* item = [submenu itemAtIndex:i];
			if ([[item title] isEqualToString:@"Services"])
			{
				return [item submenu];
			}
		}
		return nil;
	}

	/*static*/
	void OSXMenu::EnsureAllItemsHaveSubmenus(NSMenu* menu)
	{
		for (int i = 0; i < [menu numberOfItems]; i++)
		{
			NSMenuItem* item = [menu itemAtIndex:i];
			if ([item submenu] == nil)
			{
				NSMenu* sm = [[NSMenu alloc] initWithTitle:[item title]];
				[item setSubmenu:sm];
				[sm release];
			}
		}
	}

	/*static*/
	void OSXMenu::FixWindowMenu(NSMenu* menu)
	{
		NSMenu* windowMenu = OSXMenu::GetWindowMenu(menu);
		if (windowMenu == nil)
		{
			return;
		}

		int lastSeparator = -1;
		for (int i = 0; i < [windowMenu numberOfItems]; i++)
		{
			if ([[windowMenu itemAtIndex:i] isSeparatorItem])
			{
				lastSeparator = i;
			}
		}

		if (lastSeparator == -1)
		{
			return;
		}

		while ([windowMenu numberOfItems] > lastSeparator+1)
		{
			[windowMenu removeItemAtIndex:lastSeparator+1];
		}
	}
}
