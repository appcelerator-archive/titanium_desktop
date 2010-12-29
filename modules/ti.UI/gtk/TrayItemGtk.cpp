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

#include "TrayItemGtk.h"

#include "MenuGtk.h"

namespace Titanium {

void TrayClickedCallback(GtkStatusIcon*, gpointer);
void TrayMenuCallback(GtkStatusIcon*, guint, guint, gpointer);

TrayItemGtk::TrayItemGtk(std::string& iconURL, KMethodRef cb)
    : TrayItem(iconURL)
    , item(gtk_status_icon_new())
    , menu(0)
    , callback(cb)
    , active(true)
{
    g_signal_connect(
        G_OBJECT(this->item), "activate",
        G_CALLBACK(TrayClickedCallback), this);
    g_signal_connect(
        G_OBJECT(this->item), "popup-menu",
        G_CALLBACK(TrayMenuCallback), this);

    this->SetIcon(this->iconPath);
    gtk_status_icon_set_visible(this->item, TRUE);
}

TrayItemGtk::~TrayItemGtk()
{
}

void TrayItemGtk::SetIcon(std::string& iconPath)
{
    if (active) {
        if (iconPath.empty()) {
            gtk_status_icon_set_from_file(this->item, NULL);
        } else {
            gtk_status_icon_set_from_file(this->item, iconPath.c_str());
        }
    }
}

void TrayItemGtk::SetMenu(AutoPtr<Menu> menu)
{
    this->menu = menu.cast<MenuGtk>();
}

void TrayItemGtk::SetHint(std::string& hint)
{
    if (active) {
        if (hint.empty()) {
            gtk_status_icon_set_tooltip(this->item, NULL);
        } else {
            gtk_status_icon_set_tooltip(this->item, hint.c_str());
        }
    }
}

void TrayItemGtk::Remove()
{
    if (active) {
        this->active = false;
        g_object_unref(this->item);
    }
}

GtkStatusIcon* TrayItemGtk::GetWidget()
{
    if (active)
        return this->item;
    else
        return NULL;
}

AutoPtr<MenuGtk> TrayItemGtk::GetMenu()
{
    return this->menu;
}

KMethodRef TrayItemGtk::GetCallback()
{
    return this->callback;
}

void TrayClickedCallback(GtkStatusIcon *status_icon, gpointer data)
{
    TrayItemGtk* item = static_cast<TrayItemGtk*>(data);
    KMethodRef cb = item->GetCallback();

    if (cb.isNull())
        return;

    try {
        ValueList args;
        cb->Call(args);

    } catch (ValueException& e) {
        Logger* logger = Logger::Get("UI.TrayItemGtk");
        SharedString ss = e.DisplayString();
        logger->Error("Tray icon callback failed: %s", ss->c_str());
    }
}

void TrayMenuCallback(
    GtkStatusIcon *status_icon, guint button,
    guint activate_time, gpointer data)
{
    TrayItemGtk* item = static_cast<TrayItemGtk*>(data);
    GtkStatusIcon* trayWidget = item->GetWidget();
    AutoPtr<MenuGtk> menu = item->GetMenu();

    if (!menu.isNull()) {
        ::GtkMenu* nativeMenu = (::GtkMenu*) menu->CreateNative(false);
        gtk_menu_popup(
            nativeMenu, NULL, NULL,
            gtk_status_icon_position_menu,
            trayWidget, button, activate_time);
    }
}

} // namespace Titanium

