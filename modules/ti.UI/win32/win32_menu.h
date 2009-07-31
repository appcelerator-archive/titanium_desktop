/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#ifndef _WIN32_MENU_H_
#define _WIN32_MENU_H_
namespace ti
{
	class Win32Menu : public Menu
	{
		public:
		Win32Menu();
		~Win32Menu();

		void AppendItemImpl(AutoMenuItem item);
		void InsertItemAtImpl(AutoMenuItem item, unsigned int index);
		void RemoveItemAtImpl(unsigned int index);
		void ClearImpl();

		void ClearNativeMenu(HMENU nativeMenu);
		void DestroyNative(HMENU nativeMenu);
		HMENU CreateNative(bool registerNative);
		HMENU CreateNativeTopLevel(bool registerNative);
		void AddChildrenToNativeMenu(HMENU nativeMenu, bool registerNative);

		static void InsertItemIntoNativeMenu(
			Win32MenuItem* menuItem, HMENU nativeMenu,
			bool registerNative, int position=-1);
		static void RemoveItemAtFromNativeMenu(
			Win32MenuItem* item, HMENU nativeMenu, int position);
		static void ApplyNotifyByPositionStyleToNativeMenu(HMENU nativeMenu);

		private:
		std::vector<AutoMenuItem> oldChildren;
		std::vector<HMENU> nativeMenus;
	};
}
#endif
