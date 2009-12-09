/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#ifndef _GTK_MENU_ITEM_H_
#define _GTK_MENU_ITEM_H_
namespace ti
{
	class GtkMenuItem : public MenuItem
	{
	public:
		GtkMenuItem(MenuItemType type);
		virtual ~GtkMenuItem();

		void SetLabelImpl(std::string newLabel);
		void SetIconImpl(std::string newIconPath);
		void SetStateImpl(bool newState);
		void SetCallbackImpl(KMethodRef callback);
		void SetSubmenuImpl(AutoMenu newSubmenu);
		void SetEnabledImpl(bool enabled);

		void ReplaceNativeItem(::GtkMenuItem* nativeItem, ::GtkMenuItem* newNativeItem);
		void SetNativeItemIcon(::GtkMenuItem* nativeItem, std::string& newIconPath);
		void SetNativeItemSubmenu(::GtkMenuItem* nativeItem, AutoMenu newSubmenu);
		::GtkMenuItem* CreateNative(bool registerNative);
		void DestroyNative(::GtkMenuItem* nativeItem);
		static void BlockSignal(::GtkMenuItem* nativeItem, MenuItem* item);
		static void UnblockSignal(::GtkMenuItem* nativeItem, MenuItem* item);

	private:
		std::vector< ::GtkMenuItem* > nativeItems;
		AutoPtr<GtkMenu> oldSubmenu;
	};
}
#endif
