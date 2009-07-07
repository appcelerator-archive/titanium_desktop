/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#ifndef _GTK_MENU_H_
#define _GTK_MENU_H_
namespace ti
{
	class Win32Menu : public Menu
	{
	public:
		Win32Menu();
		~Win32Menu();

		void AppendItemImpl(SharedMenuItem item);
		void InsertItemAtImpl(SharedMenuItem item, unsigned int index);
		void RemoveItemAtImpl(unsigned int index);
		void ClearImpl();

		void ClearNativeMenu(::Win32MenuShell* nativeMenu);
		void RemoveItemAtFromNativeMenu(::Win32MenuShell* nativeMenu, unsigned int index);
		void DestroyNative(::Win32MenuShell* nativeMenu);
		Win32MenuShell* CreateNativeBar(bool registerNative);
		::Win32MenuShell* CreateNative(bool registerNative);
		void AddChildrenToNativeMenu(::Win32MenuShell* nativeMenu, bool registerNative);
		void RegisterNativeMenuItem(SharedMenuItem item, ::Win32MenuItem* nativeItem);
		void DestroyNativeMenuItem(::Win32MenuItem* nativeItem);

	private:
		std::vector<SharedMenuItem> oldChildren;
		std::vector< ::Win32MenuShell* > nativeMenus;
	};
}
#endif
