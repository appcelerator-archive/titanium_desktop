/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#ifndef _OSX_MENU_ITEM_H_
#define _OSX_MENU_ITEM_H_
#include <Cocoa/Cocoa.h>
#include "../menu_item.h"
#include "../menu.h"
namespace ti
{
	class OSXMenuItem : public MenuItem
	{
	public:
		OSXMenuItem(MenuItemType type, std::string label,
			SharedKMethod callback, std::string iconURL);
		virtual ~OSXMenuItem();

		void SetLabelImpl(std::string newLabel);
		void SetIconImpl(std::string newIconPath);
		void SetCallbackImpl(SharedKMethod callback);
		void SetSubmenuImpl(SharedMenu newSubmenu);
		void EnableImpl();
		void DisableImpl();
		bool HasSubmenu();

		NSMenuItem* CreateNative(bool registerNative=true);
		void DestroyNative(NSMenuItem* realization);
		void UpdateNativeMenuItems();

	private:
		static void SetNSMenuItemTitle(NSMenuItem* item, std::string& title);
		static void SetNSMenuItemIconPath(
			NSMenuItem* item,
			std::string& iconPath,
			NSImage* image = nil);
		static void SetNSMenuItemSubmenu(
			NSMenuItem* item,
			SharedMenu submenu,
			bool registerNative=true);
		static void SetNSMenuItemEnabled(NSMenuItem* item, bool enabled);

		std::vector<NSMenuItem*> realizations;
	};
}
#endif
