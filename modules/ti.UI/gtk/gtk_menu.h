/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#ifndef _GTK_MENU_H_
#define _GTK_MENU_H_
namespace ti
{
	class GtkMenu : public Menu
	{
	public:
		GtkMenu();
		~GtkMenu();

		void AppendItemImpl(SharedMenuItem item);
		void InsertItemAtImpl(SharedMenuItem item, unsigned int index);
		void RemoveItemAtImpl(unsigned int index);
		void ClearImpl();

		void ClearNativeMenu(::GtkMenuShell* nativeMenu);
		void RemoveItemAtFromNativeMenu(::GtkMenuShell* nativeMenu, unsigned int index);
		void DestroyNative(::GtkMenuShell* nativeMenu);
		GtkMenuShell* CreateNativeBar(bool registerNative);
		::GtkMenuShell* CreateNative(bool registerNative);
		void AddChildrenToNativeMenu(::GtkMenuShell* nativeMenu, bool registerNative);
		void RegisterNativeMenuItem(SharedMenuItem item, ::GtkMenuItem* nativeItem);
		void DestroyNativeMenuItem(::GtkMenuItem* nativeItem);

	private:
		std::vector<SharedMenuItem> oldChildren;
		std::vector< ::GtkMenuShell* > nativeMenus;
	};
}
#endif
