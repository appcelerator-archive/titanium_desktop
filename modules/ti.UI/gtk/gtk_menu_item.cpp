/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#include "../ui_module.h"
namespace ti
{
	namespace GtkMenus
	{
		void MenuCallback(gpointer data)
		{
			GtkMenuItem* item = static_cast<GtkMenuItem*>(data);
			item->HandleClickEvent(0);
		}
	}

	GtkMenuItem::GtkMenuItem(
		MenuItemType type, std::string label,
		SharedKMethod callback, std::string iconURL) :
			MenuItem(type, label, callback, iconURL),
			oldSubmenu(0)
	{
	}

	GtkMenuItem::~GtkMenuItem()
	{
	}

	void GtkMenuItem::SetLabelImpl(std::string newLabel)
	{
		if (this->type == SEPARATOR)
			return;

		std::vector< ::GtkMenuItem*>::iterator i = this->nativeItems.begin();
		while (i != this->nativeItems.end()) {
			::GtkMenuItem* nativeItem = *i++;
			gtk_menu_item_set_label(nativeItem, newLabel.c_str());
		}
	}

	void GtkMenuItem::SetIconImpl(std::string newIconPath)
	{
		if (this->type == SEPARATOR || this->type == CHECK)
			return;

		// Make a copy of the list of nativeItems, because
		// SetNativeItemIcon may modify it
		std::vector< ::GtkMenuItem*> nativeCopy = this->nativeItems;

		std::vector< ::GtkMenuItem*>::iterator i = nativeCopy.begin();
		while (i != nativeCopy.end()) {
			::GtkMenuItem* nativeItem = *i++;
			this->SetNativeItemIcon(nativeItem, newIconPath);
		}
	}

	void GtkMenuItem::SetStateImpl(bool newState)
	{
		if (this->type != CHECK)
			return;

		std::vector< ::GtkMenuItem*>::iterator i = this->nativeItems.begin();
		while (i != this->nativeItems.end()) {
			::GtkMenuItem* nativeItem = *i++;
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(nativeItem), newState);
		}
	}

	void GtkMenuItem::SetSubmenuImpl(SharedMenu newSubmenu)
	{
		if (this->type == SEPARATOR)
			return;

		std::vector< ::GtkMenuItem*>::iterator i = this->nativeItems.begin();
		while (i != this->nativeItems.end()) {
			::GtkMenuItem* nativeItem = *i++;
			this->SetNativeItemSubmenu(nativeItem, newSubmenu);
		}

		this->oldSubmenu = newSubmenu.cast<GtkMenu>();
	}

	void GtkMenuItem::SetEnabledImpl(bool enabled)
	{
		if (this->type == SEPARATOR)
			return;

		std::vector< ::GtkMenuItem*>::iterator i = this->nativeItems.begin();
		while (i != this->nativeItems.end()) {
			::GtkMenuItem* nativeItem = *i++;
			gtk_widget_set_sensitive(GTK_WIDGET(nativeItem), enabled);
		}
	}

	void GtkMenuItem::SetNativeItemIcon(::GtkMenuItem* nativeItem, std::string& newIconPath)
	{
		if ((newIconPath.empty() &&
				G_TYPE_FROM_INSTANCE(nativeItem) == GTK_TYPE_IMAGE_MENU_ITEM) ||
			(G_TYPE_FROM_INSTANCE(nativeItem) != GTK_TYPE_IMAGE_MENU_ITEM)) {
			::GtkMenuItem* newNativeItem = this->CreateNative(true);
			this->ReplaceNativeItem(nativeItem, newNativeItem);

		} else {
			GtkWidget* image = gtk_image_new_from_file(newIconPath.c_str());
			gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(nativeItem), image);
		}
	}

	void GtkMenuItem::ReplaceNativeItem(::GtkMenuItem* nativeItem, ::GtkMenuItem* newNativeItem)
	{
		GtkMenuShell* nativeMenu = GTK_MENU_SHELL(gtk_widget_get_parent(GTK_WIDGET(nativeItem)));
		GList* children = gtk_container_get_children(GTK_CONTAINER(nativeMenu));

		for (size_t i = 0; i < g_list_length(children); i++)
		{
			::GtkMenuItem* w = static_cast< ::GtkMenuItem*>(g_list_nth_data(children, i));
			if (w == nativeItem) {
				gtk_container_remove(GTK_CONTAINER(nativeMenu), GTK_WIDGET(w));
				gtk_menu_shell_insert(nativeMenu, GTK_WIDGET(newNativeItem), i);
				this->DestroyNative(nativeItem);
				return;
			}
		}
	}

	void GtkMenuItem::SetNativeItemSubmenu(::GtkMenuItem* nativeItem, SharedMenu newSubmenu)
	{
		::GtkMenuShell* oldNativeMenu = GTK_MENU_SHELL(gtk_menu_item_get_submenu(nativeItem));
		if (oldNativeMenu && !this->oldSubmenu.isNull()) {
			this->oldSubmenu->DestroyNative(oldNativeMenu);
		}

		SharedPtr<GtkMenu> newGtkSubmenu = newSubmenu.cast<GtkMenu>();
		::GtkMenuShell* newNativeMenu = 0;
		if (!newGtkSubmenu.isNull()) {
			newGtkSubmenu->CreateNative(true);
		}
		gtk_menu_item_set_submenu(nativeItem, GTK_WIDGET(newNativeMenu));
	}

	::GtkMenuItem* GtkMenuItem::CreateNative(bool registerNative)
	{
		::GtkMenuItem* newNativeItem;
		if (this->IsSeparator()) {
			return (::GtkMenuItem*) gtk_separator_menu_item_new();

		} else if (this->IsCheck()) {
			newNativeItem = (::GtkMenuItem*) gtk_menu_item_new();
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(newNativeItem), this->state);

		} else if (this->iconPath.empty()) {
			newNativeItem = (::GtkMenuItem*) gtk_menu_item_new();

		} else {
			newNativeItem = (::GtkMenuItem*) gtk_image_menu_item_new();
			GtkWidget* image = gtk_image_new_from_file(this->iconPath.c_str());
			gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(newNativeItem), image);
		}

		gtk_menu_item_set_label(newNativeItem, this->label.c_str());
		gtk_widget_set_sensitive(GTK_WIDGET(newNativeItem), this->enabled);
		this->SetNativeItemSubmenu(newNativeItem, this->submenu);
		g_signal_connect_swapped(
			G_OBJECT(newNativeItem), "activate", G_CALLBACK(GtkMenus::MenuCallback), this);

		if (registerNative)
				this->nativeItems.push_back(newNativeItem);

		return newNativeItem;
	}

	void GtkMenuItem::DestroyNative(::GtkMenuItem* nativeItem)
	{
		std::vector< ::GtkMenuItem*>::iterator i = this->nativeItems.begin();
		while (i != this->nativeItems.end())
		{
			::GtkMenuItem* item = *i;
			if (item == nativeItem) {
				i = this->nativeItems.erase(i);
			} else {
				i++;
			}
		}

		this->SetNativeItemSubmenu(nativeItem, 0);
	}
}


