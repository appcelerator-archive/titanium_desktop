/*
 * Copyright (c) 2009-2010 Appcelerator, Inc. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "MenuGtk.h"

#include <vector>

#include "MenuItemGtk.h"

using namespace std;

namespace Titanium {

MenuGtk::MenuGtk()
	: Menu()
{
}

MenuGtk::~MenuGtk()
{
	vector<GtkMenuShell*>::iterator i = this->nativeMenus.begin();
	while (i != this->nativeMenus.end()) {
		this->ClearNativeMenu((*i++));
	}
	nativeMenus.clear();
}

void MenuGtk::AppendItemImpl(AutoPtr<MenuItem> item)
{
	AutoPtr<MenuItemGtk> gtkItem = item.cast<MenuItemGtk>();
	vector<GtkMenuShell*>::iterator i = this->nativeMenus.begin();
	while (i != this->nativeMenus.end())
	{
		GtkMenuShell* nativeMenu = (*i++);
		GtkMenuItem* nativeItem = gtkItem->CreateNative(true);
		gtk_menu_shell_append(GTK_MENU_SHELL(nativeMenu), GTK_WIDGET(nativeItem));
		gtk_widget_show(GTK_WIDGET(nativeItem));
	}

	this->oldChildren = this->children;
}

void MenuGtk::InsertItemAtImpl(AutoPtr<MenuItem> item, unsigned int index)
{
	AutoPtr<MenuItemGtk> gtkItem = item.cast<MenuItemGtk>();
	vector<GtkMenuShell*>::iterator i = this->nativeMenus.begin();
	while (i != this->nativeMenus.end())
	{
		GtkMenuShell* nativeMenu = (*i++);
		GtkMenuItem* nativeItem = gtkItem->CreateNative(true);
		gtk_menu_shell_insert(GTK_MENU_SHELL(nativeMenu), GTK_WIDGET(nativeItem), index);
		gtk_widget_show(GTK_WIDGET(nativeItem));
	}

	this->oldChildren = this->children;
}

void MenuGtk::RemoveItemAtImpl(unsigned int index)
{
	vector<GtkMenuShell*>::iterator i = this->nativeMenus.begin();
	while (i != this->nativeMenus.end())
	{
		GtkMenuShell* nativeMenu = (*i++);
		this->RemoveItemAtFromNativeMenu(nativeMenu, index);
	}

	this->oldChildren = this->children;
}

void MenuGtk::ClearImpl()
{
	vector<GtkMenuShell*>::iterator i = this->nativeMenus.begin();
	while (i != this->nativeMenus.end())
	{
		GtkMenuShell* nativeMenu = (*i++);
		this->ClearNativeMenu(nativeMenu);
	}

	this->oldChildren = this->children;
}

void MenuGtk::ClearNativeMenu(GtkMenuShell* nativeMenu)
{
	GList* children = gtk_container_get_children(GTK_CONTAINER(nativeMenu));
	for (size_t i = 0; i < g_list_length(children); i++)
	{
		GtkMenuItem* w = static_cast<GtkMenuItem*>(g_list_nth_data(children, i));

		AutoPtr<MenuItem> item = this->oldChildren.at(i);
		AutoPtr<MenuItemGtk> gtkItem = item.cast<MenuItemGtk>();
		gtkItem->DestroyNative(w);

		gtk_container_remove(GTK_CONTAINER(nativeMenu), GTK_WIDGET(w));
	}
}

void MenuGtk::RemoveItemAtFromNativeMenu(GtkMenuShell* nativeMenu, unsigned int index)
{
	GList* children = gtk_container_get_children(GTK_CONTAINER(nativeMenu));
	GtkMenuItem* w = (::GtkMenuItem*) g_list_nth_data(children, index);

	AutoPtr<MenuItem> item = this->oldChildren.at(index);
	AutoPtr<MenuItemGtk> gtkItem = item.cast<MenuItemGtk>();
	gtkItem->DestroyNative(w);

	gtk_container_remove(GTK_CONTAINER(nativeMenu), GTK_WIDGET(w));
}

void MenuGtk::DestroyNative(GtkMenuShell* nativeMenu)
{
	// Remove the native menu from our list of known native menus
	vector<GtkMenuShell*>::iterator i = this->nativeMenus.begin();
	while (i != this->nativeMenus.end()) {
		if (*i == nativeMenu)
			i = this->nativeMenus.erase(i);
		else
			i++;
	}

	// Clear the native menu and release, so that children will be freed
	this->ClearNativeMenu(nativeMenu);
}

GtkMenuShell* MenuGtk::CreateNativeBar(bool registerNative)
{
	GtkMenuShell* nativeMenu = GTK_MENU_SHELL(gtk_menu_bar_new());
	this->AddChildrenToNativeMenu(nativeMenu, registerNative);
	if (registerNative)
		this->nativeMenus.push_back(nativeMenu);
	return nativeMenu;
}

GtkMenuShell* MenuGtk::CreateNative(bool registerNative)
{
	GtkMenuShell* nativeMenu = GTK_MENU_SHELL(gtk_menu_new());
	this->AddChildrenToNativeMenu(nativeMenu, registerNative);
	if (registerNative)
		this->nativeMenus.push_back(nativeMenu);
	return nativeMenu;
}

void MenuGtk::AddChildrenToNativeMenu(GtkMenuShell* nativeMenu, bool registerNative)
{
	vector<AutoPtr<MenuItem> >::iterator i = this->children.begin();
	while (i != this->children.end()) {
		AutoPtr<MenuItem> item = *i++;
		AutoPtr<MenuItemGtk> gtkItem = item.cast<MenuItemGtk>();
		GtkMenuItem* nativeItem = gtkItem->CreateNative(registerNative);
		gtk_menu_shell_append(GTK_MENU_SHELL(nativeMenu), GTK_WIDGET(nativeItem));
		gtk_widget_show_all(GTK_WIDGET(nativeItem));
	}
}

} // namespace Titanium

