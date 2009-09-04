/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#include "../ui_module.h"
#include "../url/url.h"
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/scrnsaver.h>
#include <gdk/gdkx.h>
#include <Poco/Thread.h>
#include <libsoup/soup.h>
#include <libsoup/soup-gnome.h>

namespace ti
{
	GtkUIBinding::GtkUIBinding(Host *host) :
		UIBinding(host),
		menu(0),
		contextMenu(0),
		iconPath("")
	{
		// Prepare the custom URL handlers
		webkit_titanium_set_normalize_url_cb(NormalizeURLCallback);
		webkit_titanium_set_url_to_file_url_cb(URLToFileURLCallback);
		webkit_titanium_set_can_preprocess_cb(CanPreprocessURLCallback);
		webkit_titanium_set_preprocess_cb(PreprocessURLCallback);

		// Setup libsoup proxy support
		SoupSession* session = webkit_get_default_session();
		soup_session_add_feature_by_type(session, SOUP_TYPE_PROXY_RESOLVER_GNOME);

		char buf[256];
		snprintf(buf, 256, "%s/%s", PRODUCT_NAME, STRING(PRODUCT_VERSION));
		g_set_prgname(buf);

		std::string webInspectorPath = host->GetRuntimePath();
		webInspectorPath = FileUtils::Join(webInspectorPath.c_str(), "webinspector", NULL);
		webkit_titanium_set_inspector_url(webInspectorPath.c_str());

		// Tell Titanium what WebKit is using for a user-agent
		SharedKObject global = host->GetGlobalObject();
		const gchar* user_agent = webkit_titanium_get_user_agent();
		global->Set("userAgent", Value::NewString(user_agent));
	}

	AutoUserWindow GtkUIBinding::CreateWindow(
		WindowConfig* config,
		AutoUserWindow& parent)
	{
		UserWindow* w = new GtkUserWindow(config, parent);
		return w->GetAutoPtr();
	}

	void GtkUIBinding::ErrorDialog(std::string msg)
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
		UIBinding::ErrorDialog(msg);
	}

	AutoMenu GtkUIBinding::CreateMenu()
	{
		return new GtkMenu();
	}

	AutoMenuItem GtkUIBinding::CreateMenuItem()
	{
		return new GtkMenuItem(MenuItem::NORMAL);
	}

	AutoMenuItem GtkUIBinding::CreateSeparatorMenuItem()
	{
		return new GtkMenuItem(MenuItem::SEPARATOR);
	}

	AutoMenuItem GtkUIBinding::CreateCheckMenuItem()
	{
		return new GtkMenuItem(MenuItem::CHECK);
	}

	void GtkUIBinding::SetMenu(AutoMenu newMenu)
	{
		this->menu = newMenu.cast<GtkMenu>();
	}

	void GtkUIBinding::SetContextMenu(AutoMenu newMenu)
	{
		this->contextMenu = newMenu.cast<GtkMenu>();
	}

	void GtkUIBinding::SetIcon(std::string& iconPath)
	{
		this->iconPath = iconPath;
	}

	AutoTrayItem GtkUIBinding::AddTray(std::string& iconPath, SharedKMethod cb)
	{
		AutoTrayItem item = new GtkTrayItem(iconPath, cb);
		return item;
	}

	long GtkUIBinding::GetIdleTime()
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

	AutoMenu GtkUIBinding::GetMenu()
	{
		return this->menu;
	}

	AutoMenu GtkUIBinding::GetContextMenu()
	{
		return this->contextMenu;
	}

	std::string& GtkUIBinding::GetIcon()
	{
		return this->iconPath;
	}

}
