/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#ifndef _OSX_MENU_H_
#define _OSX_MENU_H_
namespace ti
{
	class OSXMenu : public Menu
	{
	public:
		OSXMenu();
		~OSXMenu();

		void AppendItemImpl(AutoMenuItem item);
		void InsertItemAtImpl(AutoMenuItem item, unsigned int index);
		void RemoveItemAtImpl(unsigned int index);
		void ClearImpl();

		NSMenu* CreateNativeNow(bool registerMenu=true);
		NSMenu* CreateNativeLazily(bool registerMenu=true);
		void FillNativeMainMenu(NSMenu* defaultMenu, NSMenu* nativeMainMenu);
		static void CopyMenu(NSMenu* from, NSMenu* to);
		static NSMenuItem* CopyMenuItem(NSMenuItem* item);

		void AddChildrenToNativeMenu(
			NSMenu* nativeMenu, bool registerMenu=true, bool isMainMenu=false);
		void AddChildrenToNSArray(NSMutableArray* array);
		void DestroyNative(NSMenu* nativeMenu);
		void UpdateNativeMenus();
		static void ClearNativeMenu(NSMenu* nativeMenu);
		static void UpdateNativeMenu(NSMenu* nativeMenu);
		static NSMenu* GetWindowMenu(NSMenu* nsMenu);
		static NSMenu* GetAppleMenu(NSMenu* nsMenu);
		static NSMenu* GetServicesMenu(NSMenu* nsMenu);
		static void EnsureAllItemsHaveSubmenus(NSMenu* menu);
		static void FixWindowMenu(NSMenu* menu);
		static bool IsNativeMenuAMainMenu(NSMenu* menu);
		static void SetupInspectorItem(NSMenu* menu);
		static void ReplaceAppNameStandinInMenu(NSMenu* menu, NSString* appName);

	private:
		void Clear();
		NSMenu* CreateNative(bool lazy, bool registerMenu);
		std::vector<NSMenu*> nativeMenus;
	};
}
#endif
