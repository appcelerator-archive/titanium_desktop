/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#include "../ui_module.h"

namespace ti
{

	void TrayClickedCallback(GtkStatusIcon*, gpointer);
	void TrayMenuCallback(GtkStatusIcon*, guint, guint, gpointer);

	GtkTrayItem::GtkTrayItem(SharedString icon_path, SharedKMethod cb) :
		TrayItem(),
		item(gtk_status_icon_new()),
		menu(0),
		callback(cb),
		active(true)
	{
		g_signal_connect(
			G_OBJECT(this->item), "activate",
			G_CALLBACK(TrayClickedCallback), this);
		g_signal_connect(
			G_OBJECT(this->item), "popup-menu",
			G_CALLBACK(TrayMenuCallback), this);

		this->SetIcon(icon_path);
		gtk_status_icon_set_visible(this->item, TRUE);
	}

	GtkTrayItem::~GtkTrayItem()
	{
	}

	void GtkTrayItem::SetIcon(SharedString icon_path)
	{
		if (!active) return;

		if (icon_path.isNull())
		{
			gtk_status_icon_set_from_file(this->item, NULL);
		}
		else
		{
			gtk_status_icon_set_from_file(
				this->item, icon_path->c_str());
		}
	}

	void GtkTrayItem::SetMenu(SharedMenu menu)
	{
		this->menu = menu.cast<GtkMenu>();
	}

	void GtkTrayItem::SetHint(SharedString hint)
	{
		if (!active) return;

		gtk_status_icon_set_tooltip(this->item, hint->c_str());
	}

	void GtkTrayItem::Remove()
	{
		if (active) {
			this->active = false;
			g_object_unref(this->item);
		}
	}

	GtkStatusIcon* GtkTrayItem::GetWidget()
	{
		if (active)
			return this->item;
		else
			return NULL;
	}

	SharedPtr<GtkMenu> GtkTrayItem::GetMenu()
	{
		return this->menu;
	}

	SharedKMethod GtkTrayItem::GetCallback()
	{
		return this->callback;
	}

	void TrayClickedCallback(GtkStatusIcon *status_icon, gpointer data)
	{
		GtkTrayItem* item = static_cast<GtkTrayItem*>(data);
		SharedKMethod cb = item->GetCallback();

		if (cb.isNull())
			return;

		try {
			ValueList args;
			cb->Call(args);

		} catch (ValueException& e) {
			Logger* logger = Logger::Get("UI.GtkTrayItem");
			SharedString ss = e.DisplayString();
			logger->Error("Tray icon callback failed: %s", ss->c_str());
		}
	}

	void TrayMenuCallback(
		GtkStatusIcon *status_icon, guint button,
		guint activate_time, gpointer data)
	{
		GtkTrayItem* item = static_cast<GtkTrayItem*>(data);

		GtkStatusIcon* trayWidget = item->GetWidget();
		SharedPtr<GtkMenu> menu = item->GetMenu();

		if (!menu.isNull()) {
			::GtkMenu* nativeMenu = (::GtkMenu*) menu->CreateNative(false);
			gtk_menu_popup(
				nativeMenu, NULL, NULL,
				gtk_status_icon_position_menu,
				trayWidget, button, activate_time);
		}
	}


}
