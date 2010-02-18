/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#include "../ui_module.h"
namespace ti
{
	static void MenuCallback(GtkMenuItem* nativeItem, gpointer data)
	{
		GtkMenuItem* item = static_cast<GtkMenuItem*>(data);
		if (item->IsCheck())
		{
			g_signal_handlers_block_by_func(G_OBJECT(nativeItem),
				(void*) MenuCallback, item);
			gtk_check_menu_item_set_active(
				GTK_CHECK_MENU_ITEM(nativeItem), item->GetState());
			g_signal_handlers_unblock_by_func(G_OBJECT(nativeItem),
				(void*) MenuCallback, item);
		}
		item->HandleClickEvent(0);
	}

	GtkMenuItem::GtkMenuItem(MenuItemType type) :
		MenuItem(type),
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

		// Make a copy of the list of nativeItems, because we may modify it
		std::vector< ::GtkMenuItem*> nativeCopy = this->nativeItems;
		std::vector< ::GtkMenuItem*>::iterator i = nativeCopy.begin();
		while (i != nativeCopy.end())
		{
			::GtkMenuItem* nativeItem = *i++;
			::GtkMenuItem* newNativeItem = this->CreateNative(true);
			this->ReplaceNativeItem(nativeItem, newNativeItem);
		}
	}

	void GtkMenuItem::SetIconImpl(std::string newIconPath)
	{
		if (this->type == SEPARATOR || this->type == CHECK)
			return;

		// Make a copy of the list of nativeItems, because SetNativeItemIcon may modify it
		std::vector< ::GtkMenuItem*> nativeCopy = this->nativeItems;

		std::vector< ::GtkMenuItem*>::iterator i = nativeCopy.begin();
		while (i != nativeCopy.end())
		{
			::GtkMenuItem* nativeItem = *i++;
			this->SetNativeItemIcon(nativeItem, newIconPath);
		}
	}

	void GtkMenuItem::SetStateImpl(bool newState)
	{
		if (!this->IsCheck())
			return;

		std::vector< ::GtkMenuItem*>::iterator i = this->nativeItems.begin();
		while (i != this->nativeItems.end())
		{
			::GtkMenuItem* nativeItem = *i++;
			g_signal_handlers_block_by_func(G_OBJECT(nativeItem),
				(void*) MenuCallback, this);
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(nativeItem), newState);
			g_signal_handlers_unblock_by_func(G_OBJECT(nativeItem),
				(void*) MenuCallback, this);
		}
	}

	void GtkMenuItem::SetSubmenuImpl(AutoMenu newSubmenu)
	{
		if (this->IsSeparator())
			return;

		std::vector< ::GtkMenuItem*>::iterator i = this->nativeItems.begin();
		while (i != this->nativeItems.end())
		{
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
		while (i != this->nativeItems.end())
		{
			::GtkMenuItem* nativeItem = *i++;
			gtk_widget_set_sensitive(GTK_WIDGET(nativeItem), enabled);
		}
	}

	void GtkMenuItem::SetNativeItemIcon(::GtkMenuItem* nativeItem, std::string& newIconPath)
	{
		// If we are clearing the icon and this is already an image item or we are
		// not an image item and we are trying to set an image, then remake the native
		// item.
		if ((newIconPath.empty() &&
				G_TYPE_FROM_INSTANCE(nativeItem) == GTK_TYPE_IMAGE_MENU_ITEM) ||
			(G_TYPE_FROM_INSTANCE(nativeItem) != GTK_TYPE_IMAGE_MENU_ITEM))
		{
			::GtkMenuItem* newNativeItem = this->CreateNative(true);
			this->ReplaceNativeItem(nativeItem, newNativeItem);
		}
		else
		{
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
			if (w == nativeItem)
			{
				gtk_container_remove(GTK_CONTAINER(nativeMenu), GTK_WIDGET(w));
				gtk_menu_shell_insert(nativeMenu, GTK_WIDGET(newNativeItem), i);
				gtk_widget_show(GTK_WIDGET(newNativeItem));
				this->DestroyNative(nativeItem);
				return;
			}
		}
	}

	void GtkMenuItem::SetNativeItemSubmenu(::GtkMenuItem* nativeItem, AutoMenu newSubmenu)
	{
		::GtkMenuShell* oldNativeMenu = GTK_MENU_SHELL(gtk_menu_item_get_submenu(nativeItem));
		if (oldNativeMenu && !this->oldSubmenu.isNull())
		{
			this->oldSubmenu->DestroyNative(oldNativeMenu);
		}

		AutoPtr<GtkMenu> newGtkSubmenu = newSubmenu.cast<GtkMenu>();
		::GtkMenuShell* newNativeMenu = 0;
		if (!newGtkSubmenu.isNull())
		{
			newNativeMenu = newGtkSubmenu->CreateNative(true);
		}
		gtk_menu_item_set_submenu(nativeItem, GTK_WIDGET(newNativeMenu));
	}

	::GtkMenuItem* GtkMenuItem::CreateNative(bool registerNative)
	{
		::GtkMenuItem* newNativeItem;
		if (this->IsSeparator())
		{
			return (::GtkMenuItem*) gtk_separator_menu_item_new();
		}
		else if (this->IsCheck())
		{
			newNativeItem = (::GtkMenuItem*) gtk_check_menu_item_new_with_label(label.c_str());
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(newNativeItem), this->state);
		}
		else if (this->iconPath.empty())
		{
			newNativeItem = (::GtkMenuItem*) gtk_menu_item_new_with_label(label.c_str());
		}
		else
		{
			newNativeItem = (::GtkMenuItem*) gtk_image_menu_item_new_with_label(label.c_str());
			GtkWidget* image = gtk_image_new_from_file(this->iconPath.c_str());
			gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(newNativeItem), image);
		}

		gtk_widget_set_sensitive(GTK_WIDGET(newNativeItem), this->enabled);
		this->SetNativeItemSubmenu(newNativeItem, this->submenu);
		g_signal_connect(G_OBJECT(newNativeItem),
			"activate", G_CALLBACK(MenuCallback), this);

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
			if (item == nativeItem)
				i = this->nativeItems.erase(i);
			else
				i++;
		}

		this->SetNativeItemSubmenu(nativeItem, 0);
	}
}


