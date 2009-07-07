/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#ifndef _GTK_MENU_ITEM_H_
#define _GTK_MENU_ITEM_H_
namespace ti
{
	class Win32MenuItem : public MenuItem
	{
	public:
		Win32MenuItem(MenuItemType type, std::string label,
			SharedKMethod callback, std::string iconURL);
		virtual ~Win32MenuItem();

		void SetLabelImpl(std::string newLabel);
		void SetIconImpl(std::string newIconPath);
		void SetStateImpl(bool newState);
		void SetCallbackImpl(SharedKMethod callback);
		void SetSubmenuImpl(SharedMenu newSubmenu);
		void SetEnabledImpl(bool enabled);

		void ReplaceNativeItem(::Win32MenuItem* nativeItem, ::Win32MenuItem* newNativeItem);
		void SetNativeItemIcon(::Win32MenuItem* nativeItem, std::string& newIconPath);
		void SetNativeItemSubmenu(::Win32MenuItem* nativeItem, SharedMenu newSubmenu);
		::Win32MenuItem* CreateNative(bool registerNative);
		void DestroyNative(::Win32MenuItem* nativeItem);

	private:
		std::vector< ::Win32MenuItem* > nativeItems;
		SharedPtr<Win32Menu> oldSubmenu;
	};
}
#endif
