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

	GtkMenu::GtkMenu() :
		Menu()
	{
	}

	GtkMenu::~GtkMenu()
	{
		std::vector< ::GtkMenuShell* >::iterator i = this->nativeMenus.begin();
		while (i != this->nativeMenus.end()) {
			this->ClearNativeMenu((*i++));
		}
		nativeMenus.clear();
	}

	void GtkMenu::AppendItemImpl(AutoMenuItem item)
	{
		AutoPtr<GtkMenuItem> gtkItem = item.cast<GtkMenuItem>();
		std::vector<GtkMenuShell*>::iterator i = this->nativeMenus.begin();
		while (i != this->nativeMenus.end())
		{
			::GtkMenuShell* nativeMenu = (*i++);
			::GtkMenuItem* nativeItem = gtkItem->CreateNative(true);
			gtk_menu_shell_append(GTK_MENU_SHELL(nativeMenu), GTK_WIDGET(nativeItem));
			gtk_widget_show(GTK_WIDGET(nativeItem));
		}

		this->oldChildren = this->children;
	}

	void GtkMenu::InsertItemAtImpl(AutoMenuItem item, unsigned int index)
	{
		AutoPtr<GtkMenuItem> gtkItem = item.cast<GtkMenuItem>();
		std::vector<GtkMenuShell*>::iterator i = this->nativeMenus.begin();
		while (i != this->nativeMenus.end())
		{
			::GtkMenuShell* nativeMenu = (*i++);
			::GtkMenuItem* nativeItem = gtkItem->CreateNative(true);
			gtk_menu_shell_insert(GTK_MENU_SHELL(nativeMenu), GTK_WIDGET(nativeItem), index);
			gtk_widget_show(GTK_WIDGET(nativeItem));
		}

		this->oldChildren = this->children;
	}

	void GtkMenu::RemoveItemAtImpl(unsigned int index)
	{
		std::vector<GtkMenuShell*>::iterator i = this->nativeMenus.begin();
		while (i != this->nativeMenus.end())
		{
			::GtkMenuShell* nativeMenu = (*i++);
			this->RemoveItemAtFromNativeMenu(nativeMenu, index);
		}

		this->oldChildren = this->children;
	}

	void GtkMenu::ClearImpl()
	{
		std::vector<GtkMenuShell*>::iterator i = this->nativeMenus.begin();
		while (i != this->nativeMenus.end())
		{
			::GtkMenuShell* nativeMenu = (*i++);
			this->ClearNativeMenu(nativeMenu);
		}

		this->oldChildren = this->children;
	}

	void GtkMenu::ClearNativeMenu(::GtkMenuShell* nativeMenu)
	{
		GList* children = gtk_container_get_children(GTK_CONTAINER(nativeMenu));
		for (size_t i = 0; i < g_list_length(children); i++)
		{
			::GtkMenuItem* w = static_cast< ::GtkMenuItem*>(g_list_nth_data(children, i));

			AutoMenuItem item = this->oldChildren.at(i);
			AutoPtr<GtkMenuItem> gtkItem = item.cast<GtkMenuItem>();
			gtkItem->DestroyNative(w);

			gtk_container_remove(GTK_CONTAINER(nativeMenu), GTK_WIDGET(w));
		}
	}

	void GtkMenu::RemoveItemAtFromNativeMenu(::GtkMenuShell* nativeMenu, unsigned int index)
	{
		GList* children = gtk_container_get_children(GTK_CONTAINER(nativeMenu));
		::GtkMenuItem* w = (::GtkMenuItem*) g_list_nth_data(children, index);

		AutoMenuItem item = this->oldChildren.at(index);
		AutoPtr<GtkMenuItem> gtkItem = item.cast<GtkMenuItem>();
		gtkItem->DestroyNative(w);

		gtk_container_remove(GTK_CONTAINER(nativeMenu), GTK_WIDGET(w));
	}

	void GtkMenu::DestroyNative(::GtkMenuShell* nativeMenu)
	{
		// Remove the native menu from our list of known native menus
		std::vector< ::GtkMenuShell* >::iterator i = this->nativeMenus.begin();
		while (i != this->nativeMenus.end()) {
			if (*i == nativeMenu)
				i = this->nativeMenus.erase(i);
			else
				i++;
		}

		// Clear the native menu and release, so that children will be freed
		this->ClearNativeMenu(nativeMenu);
	}

	GtkMenuShell* GtkMenu::CreateNativeBar(bool registerNative)
	{
		GtkMenuShell* nativeMenu = GTK_MENU_SHELL(gtk_menu_bar_new());
		this->AddChildrenToNativeMenu(nativeMenu, registerNative);
		if (registerNative)
			this->nativeMenus.push_back(nativeMenu);
		return nativeMenu;
	}

	::GtkMenuShell* GtkMenu::CreateNative(bool registerNative)
	{
		::GtkMenuShell* nativeMenu = GTK_MENU_SHELL(gtk_menu_new());
		this->AddChildrenToNativeMenu(nativeMenu, registerNative);
		if (registerNative)
			this->nativeMenus.push_back(nativeMenu);
		return nativeMenu;
	}

	void GtkMenu::AddChildrenToNativeMenu(::GtkMenuShell* nativeMenu, bool registerNative)
	{
		vector<AutoMenuItem>::iterator i = this->children.begin();
		while (i != this->children.end()) {
			AutoMenuItem item = *i++;
			AutoPtr<GtkMenuItem> gtkItem = item.cast<GtkMenuItem>();
			::GtkMenuItem* nativeItem = gtkItem->CreateNative(registerNative);
			gtk_menu_shell_append(GTK_MENU_SHELL(nativeMenu), GTK_WIDGET(nativeItem));
			gtk_widget_show_all(GTK_WIDGET(nativeItem));
		}
	}
}
