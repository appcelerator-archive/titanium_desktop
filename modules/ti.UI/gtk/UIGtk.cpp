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

#include "UIGtk.h"

#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/scrnsaver.h>
#include <gdk/gdkx.h>
#include <Poco/Thread.h>
#include <libsoup/soup.h>
#include <libsoup/soup-gnome.h>
#include <webkit/webkit.h>

#include "MenuGtk.h"
#include "MenuItemGtk.h"
#include "../MenuItem.h"
#include "TrayItemGtk.h"
#include "../url/URLUtils.h"

namespace Titanium {

UIGtk::UIGtk()
    : menu(0)
    , contextMenu(0)
    , iconPath("")
{
    // Setup libsoup proxy support
    SoupSession* session = webkit_get_default_session();
    soup_session_add_feature_by_type(session, SOUP_TYPE_PROXY_RESOLVER_GNOME);

    //webkit_set_cache_model(WEBKIT_CACHE_MODEL_DOCUMENT_VIEWER);
}

AutoPtr<Menu> UIGtk::CreateMenu()
{
    return new MenuGtk();
}

AutoPtr<MenuItem> UIGtk::CreateMenuItem()
{
    return new MenuItemGtk(MenuItem::NORMAL);
}

AutoPtr<MenuItem> UIGtk::CreateSeparatorMenuItem()
{
    return new MenuItemGtk(MenuItem::SEPARATOR);
}

AutoPtr<MenuItem> UIGtk::CreateCheckMenuItem()
{
    return new MenuItemGtk(MenuItem::CHECK);
}

void UIGtk::SetMenu(AutoPtr<Menu> newMenu)
{
    this->menu = newMenu.cast<MenuGtk>();
}

void UIGtk::SetContextMenu(AutoPtr<Menu> newMenu)
{
    this->contextMenu = newMenu.cast<MenuGtk>();
}

void UIGtk::SetIcon(std::string& iconPath)
{
    this->iconPath = iconPath;
}

AutoPtr<TrayItem> UIGtk::AddTray(std::string& iconPath, KMethodRef cb)
{
    AutoPtr<TrayItem> item = new TrayItemGtk(iconPath, cb);
    return item;
}

long UIGtk::GetIdleTime()
{
    Display *display = gdk_x11_get_default_xdisplay();
    if (display == NULL)
        return -1;
    int screen = gdk_x11_get_default_screen();

    XScreenSaverInfo *mit_info = XScreenSaverAllocInfo();
    XScreenSaverQueryInfo(display, RootWindow(display, screen), mit_info);
    long idle_time = mit_info->idle;
    XFree(mit_info);

    return idle_time;
}

AutoPtr<Menu> UIGtk::GetMenu()
{
    return this->menu;
}

AutoPtr<Menu> UIGtk::GetContextMenu()
{
    return this->contextMenu;
}

std::string& UIGtk::GetIcon()
{
    return this->iconPath;
}

/*static*/
void UIGtk::ErrorDialog(std::string msg)
{
    GtkWidget* dialog = gtk_message_dialog_new(
        NULL,
        GTK_DIALOG_MODAL,
        GTK_MESSAGE_ERROR,
        GTK_BUTTONS_OK,
        "%s",
        msg.c_str());
    gtk_dialog_run(GTK_DIALOG (dialog));
    gtk_widget_destroy(dialog);
    UI::ErrorDialog(msg);
}

} // namespace Titanium

