/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#include "../ui_module.h"
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/scrnsaver.h>
#include <gdk/gdkx.h>
#include <Poco/Thread.h>

namespace ti
{
	GtkUIBinding::GtkUIBinding(Host *host) : UIBinding(host)
	{
		/* Prepare the custom curl URL handler */
		curl_register_local_handler(&Titanium_app_url_handler);

		/* Register the script evaluator */
		evaluator = new ScriptEvaluator();
		addScriptEvaluator(evaluator);

		char buf[256];
		snprintf(buf, 256, "%s/%s", PRODUCT_NAME, STRING(PRODUCT_VERSION));
		g_set_prgname(buf);

		webkit_titanium_set_inspector_path(host->GetRuntimePath().c_str());
	}

	SharedUserWindow GtkUIBinding::CreateWindow(
		WindowConfig* config,
		SharedUserWindow& parent)
	{
		UserWindow* w = new GtkUserWindow(config, parent);
		return w->GetSharedPtr();
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

	SharedPtr<MenuItem> GtkUIBinding::CreateMenu(bool trayMenu)
	{
		SharedPtr<MenuItem> menu = new GtkMenuItemImpl();
		return menu;
	}

	void GtkUIBinding::SetMenu(SharedPtr<MenuItem> new_menu)
	{
		// Notify all windows that the app menu has changed.
		std::vector<SharedUserWindow>& windows = this->GetOpenWindows();
		std::vector<SharedUserWindow>::iterator i = windows.begin();
		while (i != windows.end())
		{
			SharedPtr<GtkUserWindow> guw = (*i).cast<GtkUserWindow>();
			if (!guw.isNull())
				guw->AppMenuChanged();
			i++;
		}
	}

	void GtkUIBinding::SetContextMenu(SharedPtr<MenuItem> new_menu)
	{
	}

	void GtkUIBinding::SetIcon(SharedString icon_path)
	{
		// Notify all windows that the app icon has changed.
		std::vector<SharedUserWindow>& windows = this->GetOpenWindows();
		std::vector<SharedUserWindow>::iterator i = windows.begin();
		while (i != windows.end())
		{
			SharedPtr<GtkUserWindow> guw = (*i).cast<GtkUserWindow>();
			if (!guw.isNull())
				guw->AppIconChanged();
			i++;
		}
	}

	SharedPtr<TrayItem> GtkUIBinding::AddTray(
		SharedString icon_path,
		SharedKMethod cb)
	{
		SharedPtr<TrayItem> item = new GtkTrayItem(icon_path, cb);
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

}
