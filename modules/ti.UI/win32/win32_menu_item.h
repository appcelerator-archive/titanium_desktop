/**
* Appcelerator Titanium - licensed under the Apache Public License 2
* see LICENSE in the root folder for details on the license.
* Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
*/
#ifndef _WIN32_MENU_ITEM_H_
#define _WIN32_MENU_ITEM_H_
namespace ti
{
	class Win32MenuItem : public MenuItem
	{
	public:
		// Pieces of information we can use to do a reverse lookup
		// on an item's position in a parent menu.
		struct NativeItemBits
		{
			UINT id;
			HMENU parentMenu;
			HMENU submenu;
		};

		Win32MenuItem(MenuItemType type);
		virtual ~Win32MenuItem();

		void SetLabelImpl(std::string newLabel);
		void SetIconImpl(std::string newIconPath);
		void SetStateImpl(bool newState);
		void SetCallbackImpl(KMethodRef callback);
		void SetSubmenuImpl(AutoMenu newSubmenu);
		void SetEnabledImpl(bool enabled);

		void RecreateAllNativeItems();
		void RecreateMenuItem(NativeItemBits* bits);
		void CreateNative(LPMENUITEMINFO itemInfo,
			HMENU nativeParentMenu, bool registerNative);
		void DestroyNative(NativeItemBits* bits);
		void DestroyNative(HMENU nativeParent, int position);
		static int GetNativeMenuItemPosition(NativeItemBits* bits);
		static bool HandleClickEvent(HMENU nativeMenu, UINT position);

	private:
		std::string iconPath;
		AutoPtr<Win32Menu> oldSubmenu;
		std::wstring wideOldLabel;
		std::vector<NativeItemBits*> nativeItems;

	};
}
#endif