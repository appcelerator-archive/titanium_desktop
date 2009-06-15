/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */

#include "../ui_module.h"
#include <iostream>
#include <Poco/Process.h>

namespace ti
{
	static void destroy_cb(
		GtkWidget* widget,
		gpointer data);
	static gboolean event_cb(
		GtkWidget* window,
		GdkEvent* event,
		gpointer user_data);
	static void window_object_cleared_cb(
		WebKitWebView*,
		WebKitWebFrame*,
		JSGlobalContextRef,
		JSObjectRef, gpointer);
	static void populate_popup_cb(
		WebKitWebView *webView,
		GtkMenu *menu,
		gpointer data);
	static gint navigation_requested_cb(
		WebKitWebView* webView,
		WebKitWebFrame* web_frame,
		WebKitNetworkRequest* request);
	static gint new_window_navigation_requested_cb(
		WebKitWebView* webView,
		WebKitWebFrame* web_frame,
		WebKitNetworkRequest* request,
		gchar* frame_name);
	static void load_finished_cb(
		WebKitWebView* view,
		WebKitWebFrame* frame,
		gpointer data);
	static WebKitWebView* create_inspector_cb(
		WebKitWebInspector* webInspector,
		WebKitWebView* page,
		gpointer data);
	static gboolean inspector_show_window_cb(
		WebKitWebInspector* inspector,
		gpointer data);
	static void hide_window_cb(GtkWidget *widget, gpointer data);
	
	GtkUserWindow::GtkUserWindow(WindowConfig* config, SharedUserWindow& parent) :
		UserWindow(config, parent),
		gdkWidth(-1),
		gdkHeight(-1),
		gdkX(-1),
		gdkY(-1),
		gdkMaximized(false),
		gdkMinimized(false),
		gtkWindow(NULL),
		vbox(NULL),
		webView(NULL),
		topmost(false),
		menu(NULL),
		menuInUse(NULL),
		menuBar(NULL),
		iconPath(NULL),
		context_menu(NULL),
		inspectorWindow(0)
	{
	}

	GtkUserWindow::~GtkUserWindow()
	{
		this->Close();
	}
	
	void GtkUserWindow::Open()
	{
		if (this->gtkWindow == NULL)
		{
			WebKitWebView* webView = WEBKIT_WEB_VIEW(webkit_web_view_new ());
	
			g_signal_connect(
				G_OBJECT(webView), "window-object-cleared",
				G_CALLBACK(window_object_cleared_cb), this);
			g_signal_connect(
				G_OBJECT(webView), "navigation-requested",
				G_CALLBACK(navigation_requested_cb), this);
			g_signal_connect(
				G_OBJECT(webView), "new-window-navigation-requested",
				G_CALLBACK(new_window_navigation_requested_cb), this);
			g_signal_connect(
				G_OBJECT(webView), "populate-popup",
				G_CALLBACK(populate_popup_cb), this);
			g_signal_connect(
				G_OBJECT(webView), "load-finished",
				G_CALLBACK(load_finished_cb), this);
	
			// Tell Titanium what WebKit is using for a user-agent
			SharedKObject global = host->GetGlobalObject();
			if (global->Get("userAgent")->IsUndefined())
			{
				gchar* user_agent = webkit_web_view_get_user_agent(webView);
				global->Set("userAgent", Value::NewString(user_agent));
				g_free(user_agent);
			}
	
			WebKitWebSettings* settings = webkit_web_settings_new();
			g_object_set(G_OBJECT(settings), "enable-developer-extras", TRUE, NULL);
			webkit_web_view_set_settings(WEBKIT_WEB_VIEW(webView), settings);

			WebKitWebInspector *inspector = webkit_web_view_get_inspector(webView);
			g_signal_connect(
				G_OBJECT(inspector), "inspect-web-view",
				G_CALLBACK(create_inspector_cb), this);
			g_signal_connect(
				G_OBJECT(inspector), "show-window",
				G_CALLBACK(inspector_show_window_cb), this);

			GtkWidget* view_container = NULL;
			if (this->IsUsingScrollbars())
			{
				/* web view scroller */
				GtkWidget* scrolledWindow = gtk_scrolled_window_new (NULL, NULL);
				gtk_scrolled_window_set_policy(
					GTK_SCROLLED_WINDOW(scrolledWindow),
					GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
				gtk_container_add(
					GTK_CONTAINER(scrolledWindow), GTK_WIDGET(webView));
				view_container = scrolledWindow;
			}
			else // No scrollin' fer ya.
			{
				view_container = GTK_WIDGET(webView);
			}
	
			/* main window vbox */
			this->vbox = gtk_vbox_new(FALSE, 0);
			gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(view_container), TRUE, TRUE, 0);
	
			/* main window */
			GtkWidget* window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
			gtk_widget_set_name(window, this->config->GetTitle().c_str());
			gtk_window_set_title(GTK_WINDOW(window), this->config->GetTitle().c_str());
	
			this->destroyCallbackId = g_signal_connect(
				G_OBJECT(window), "destroy", G_CALLBACK(destroy_cb), this);
			g_signal_connect(G_OBJECT(window), "event",
			                 G_CALLBACK(event_cb), this);
	
			gtk_container_add(GTK_CONTAINER(window), vbox);
	
			webkit_web_view_register_url_scheme_as_local("app");
			webkit_web_view_register_url_scheme_as_local("ti");
	
			this->gtkWindow = GTK_WINDOW(window);
			this->webView = webView;

			gtk_widget_realize(window);
			this->SetupDecorations();
			this->SetupSize();
			this->SetupSizeLimits();
			this->SetupPosition();
			this->SetupMenu();
			this->SetupIcon();
			this->SetTopMost(config->IsTopMost());
			this->SetCloseable(config->IsCloseable());
			// TI-62: Transparency currently causes bad crashes
			// this->SetupTransparency();

			gtk_widget_grab_focus(GTK_WIDGET(webView));
			webkit_web_view_open(webView, this->config->GetURL().c_str());
	
			if (this->IsVisible())
			{
				gtk_widget_show_all(window);
			}
	
			if (this->config->IsFullScreen())
			{
				gtk_window_fullscreen(this->gtkWindow);
			}
	
			if (this->config->IsMaximized())
			{
				this->Maximize();
			}
	
			if (this->config->IsMinimized())
			{
				this->Minimize();
			}
	
			UserWindow::Open();
			this->FireEvent(OPENED);
		}
		else
		{
			this->Show();
		}
	}
	
	// Notify this GtkUserWindow that the GTK bits are invalid
	void GtkUserWindow::Destroyed()
	{
		this->gtkWindow = NULL;
		this->webView = NULL;
	}
	
	static void destroy_cb(
		GtkWidget* widget,
		gpointer data)
	{
		GtkUserWindow* user_window = (GtkUserWindow*) data;
		user_window->Destroyed(); // Inform the GtkUserWindow we are done
		user_window->Close();
	}
	
	void GtkUserWindow::Close()
	{
		// Destroy the GTK bits, if we have them first, because
		// we need to assume the GTK window is gone for  everything
		// below (this method might be called by destroy_cb)
		if (this->gtkWindow != NULL)
		{
			// We don't want the destroy signal handler to fire after now.
			g_signal_handler_disconnect(this->gtkWindow, this->destroyCallbackId);
			gtk_widget_destroy(GTK_WIDGET(this->gtkWindow));
			this->Destroyed();
		}
		this->RemoveOldMenu(); // Cleanup old menu
	
		UserWindow::Close();
		this->FireEvent(CLOSED);
	}
	
	void GtkUserWindow::SetupTransparency()
	{
		if (this->gtkWindow != NULL)
		{
			GValue val = {0,};
			g_value_init(&val, G_TYPE_BOOLEAN);
			g_value_set_boolean(&val, 1);
			g_object_set_property(G_OBJECT(this->webView), "transparent", &val);
	
			GdkScreen* screen = gtk_widget_get_screen(GTK_WIDGET(this->gtkWindow));
			GdkColormap* colormap = gdk_screen_get_rgba_colormap(screen);
			if (!colormap)
			{
				std::cerr << "Could not use ARGB colormap. "
				          << "True transparency not available." << std::endl;
				colormap = gdk_screen_get_rgb_colormap(screen);
			}
			gtk_widget_set_colormap(GTK_WIDGET(this->gtkWindow), colormap);
		}
	}
	
	void GtkUserWindow::SetupDecorations()
	{
		if (this->gtkWindow != NULL)
		{
			GdkWindow* gdk_window = GTK_WIDGET(this->gtkWindow)->window;
			int d = 0;
	
			if (this->config->IsUsingChrome())
				d = d | GDK_DECOR_BORDER | GDK_DECOR_TITLE | GDK_DECOR_MENU;
	
			if (this->config->IsResizable())
				d = d | GDK_DECOR_RESIZEH;
	
			if (this->config->IsMinimizable())
				d = d | GDK_DECOR_MINIMIZE;
	
			if (this->config->IsMaximizable())
				d = d | GDK_DECOR_MAXIMIZE;
	
			this->SetTransparency(config->GetTransparency());
	
			gdk_window_set_decorations(gdk_window, (GdkWMDecoration) d);
		}
	}
	
	
	void GtkUserWindow::SetupSizeLimits()
	{
		if (this->gtkWindow != NULL)
		{
			GdkGeometry hints;
			int max_width = (int) this->config->GetMaxWidth();
			int min_width = (int) this->config->GetMinWidth();
			int max_height = (int) this->config->GetMaxHeight();
			int min_height = (int) this->config->GetMinHeight();

			if (max_width == -1)
			{
				hints.max_width = INT_MAX;
			}
			else
			{
				hints.max_width = max_width;
			}

			if (min_width == -1)
			{
				hints.min_width = 1;
			}
			else
			{
				hints.min_width = min_width;
			}

			if (max_height == -1)
			{
				hints.max_height = INT_MAX;
			}
			else
			{
				hints.max_height = max_height;
			}

			if (min_height == -1)
			{
				hints.min_height = 1;
			}
			else
			{
				hints.min_height = min_height;
			}
			GdkWindowHints mask = (GdkWindowHints) (GDK_HINT_MIN_SIZE | GDK_HINT_MAX_SIZE);
			gtk_window_set_geometry_hints(this->gtkWindow, NULL, &hints, mask);
		}
	}
	
	void GtkUserWindow::SetupPosition()
	{
		if (this->gtkWindow != NULL)
		{
			int x = this->config->GetX();
			int y = this->config->GetY();
	
			GdkScreen* screen = gdk_screen_get_default();
			if (x == UserWindow::CENTERED)
			{
				x = (gdk_screen_get_width(screen) - this->GetWidth()) / 2;
				this->config->SetX(x);
			}
			if (y == UserWindow::CENTERED)
			{
				y = (gdk_screen_get_height(screen) - this->GetHeight()) / 2;
				this->config->SetY(y);
			}
			gtk_window_move(this->gtkWindow, x, y);
	
			// Moving in GTK is asynchronous, so we prime the
			// values here in hopes that things will turn out okay.
			// Another alternative would be to block until a resize
			// is detected, but that might leave the application in
			// a funky state.
			this->gdkX = x;
			this->gdkY = y;
		}
	}
	
	void GtkUserWindow::SetupSize()
	{
		if (this->gtkWindow != NULL)
		{
			gtk_window_resize(this->gtkWindow,
				(int) this->config->GetWidth(),
				(int) this->config->GetHeight());
	
			// Resizing in GTK is asynchronous, so we prime the
			// values here in hopes that things will turn out okay.
			// Another alternative would be to block until a resize
			// is detected, but that might leave the application in
			// a funky state.
			this->gdkWidth = this->config->GetWidth();
			this->gdkHeight = this->config->GetHeight();
		}
	}
	
	void GtkUserWindow::SetupIcon()
	{
		if (this->gtkWindow == NULL)
			return;
	
		GdkPixbuf* icon = NULL; // NULL is an unset.
		SharedString iconPath = this->iconPath;
		if (iconPath.isNull() && !UIModule::GetIcon().isNull())
			iconPath = UIModule::GetIcon();
	
		if (!iconPath.isNull())
		{
			GError* error = NULL;
			icon = gdk_pixbuf_new_from_file(iconPath->c_str(), &error);
	
			if (icon == NULL && error != NULL)
			{
				std::cerr << "Could not load icon because: "
				          << error->message << std::endl;
				g_error_free(error);
			}
		}
		gtk_window_set_icon(this->gtkWindow, icon);
	}

	static gboolean event_cb(
		GtkWidget *w,
		GdkEvent *event,
		gpointer data)
	{
		GtkUserWindow* window = (GtkUserWindow*) data;
		if (event->type == GDK_FOCUS_CHANGE)
		{
			GdkEventFocus* f = (GdkEventFocus*) event;
			if (f->in)
			{
				window->FireEvent(FOCUSED);
			}
			else
			{
				window->FireEvent(UNFOCUSED);
			}
		}
		else if (event->type == GDK_WINDOW_STATE)
		{
			GdkEventWindowState* f = (GdkEventWindowState*) event;
			if ((f->changed_mask & GDK_WINDOW_STATE_WITHDRAWN)
				&& (f->new_window_state & GDK_WINDOW_STATE_WITHDRAWN))
			{
				window->FireEvent(HIDDEN);
			}

			if ((f->changed_mask & GDK_WINDOW_STATE_ICONIFIED)
				&& (f->new_window_state & GDK_WINDOW_STATE_ICONIFIED))
			{
				window->FireEvent(MINIMIZED);
			}

			if (((f->changed_mask & GDK_WINDOW_STATE_FULLSCREEN)
				&& (f->new_window_state & GDK_WINDOW_STATE_FULLSCREEN)))
			{
				window->FireEvent(FULLSCREENED);
			}

			if (f->changed_mask & GDK_WINDOW_STATE_FULLSCREEN)
			{
				window->FireEvent(UNFULLSCREENED);
			}

			if (((f->changed_mask & GDK_WINDOW_STATE_MAXIMIZED)
				&& (f->new_window_state & GDK_WINDOW_STATE_MAXIMIZED)))
			{
				window->FireEvent(MAXIMIZED);
			}

			window->gdkMinimized =
				f->new_window_state & GDK_WINDOW_STATE_ICONIFIED;

			window->gdkMaximized =
				f->new_window_state & GDK_WINDOW_STATE_MAXIMIZED;
		}
		else if (event->type == GDK_CONFIGURE)
		{
			GdkEventConfigure* c = (GdkEventConfigure*) event;
			if (c->x != window->gdkX || c->y != window->gdkY)
			{
				window->gdkX = c->x;
				window->gdkY = c->y;
				window->FireEvent(MOVED);
			}
	
			if (c->width != window->gdkWidth || c->height != window->gdkHeight)
			{
				window->gdkHeight = c->height;
				window->gdkWidth = c->width;
				window->FireEvent(RESIZED);
			}
		}
	
		return FALSE;
	}
	
	static gint navigation_requested_cb(
		WebKitWebView* webView,
		WebKitWebFrame* web_frame,
		WebKitNetworkRequest* request)
	{
		const gchar* uri = webkit_network_request_get_uri(request);
		std::string new_uri = AppConfig::Instance()->InsertAppIDIntoURL(uri);
		webkit_network_request_set_uri(request, new_uri.c_str());
		return WEBKIT_NAVIGATION_RESPONSE_ACCEPT;
	}
	
	static gint new_window_navigation_requested_cb(
		WebKitWebView* webView,
		WebKitWebFrame* web_frame,
		WebKitNetworkRequest* request,
		gchar* frame_name)
	{
		const char *sbrowser = "ti:systembrowser";
		gchar* frame_name_case = g_utf8_casefold(frame_name, g_utf8_strlen(frame_name, -1));
		gchar* system_browser = g_utf8_casefold(sbrowser, g_utf8_strlen(sbrowser, -1));
	
		if (g_utf8_collate(frame_name_case, system_browser) == 0)
		{
			gchar* url = strdup(webkit_network_request_get_uri(request));
			if (url[strlen(url)-1] == '/')
				url[strlen(url)-1] = '\0';
	
			std::vector<std::string> args;
			args.push_back(std::string(url));
			Poco::Process::launch("xdg-open", args);
			return WEBKIT_NAVIGATION_RESPONSE_IGNORE;
		}
		else
		{
			return WEBKIT_NAVIGATION_RESPONSE_ACCEPT;
		}
	}
	
	static void load_finished_cb(
		WebKitWebView* view,
		WebKitWebFrame* frame,
		gpointer data)
	{
		JSGlobalContextRef context = webkit_web_frame_get_global_context(frame);
		JSObjectRef global_object = JSContextGetGlobalObject(context);
		SharedKObject frame_global = new KKJSObject(context, global_object);
		std::string uri = webkit_web_frame_get_uri(frame);
	
		GtkUserWindow* user_window = static_cast<GtkUserWindow*>(data);
		user_window->PageLoaded(frame_global, uri, context);
	}
	
	static void window_object_cleared_cb(
		WebKitWebView* webView,
		WebKitWebFrame* web_frame,
		JSGlobalContextRef context,
		JSObjectRef window_object,
		gpointer data)
	{
	
		GtkUserWindow* user_window = (GtkUserWindow*) data;
		user_window->RegisterJSContext(context);
	}
	
	static void populate_popup_cb(
		WebKitWebView *webView,
		GtkMenu *menu,
		gpointer data)
	{
		GtkUserWindow* user_window = (GtkUserWindow*) data;
		SharedPtr<GtkMenuItemImpl> m =
			user_window->GetContextMenu().cast<GtkMenuItemImpl>();
	
		if (m.isNull())
			m = UIModule::GetContextMenu().cast<GtkMenuItemImpl>();
	
		// If we are not in debug mode, remove the default WebKit menu items
		if (!user_window->GetHost()->IsDebugMode())
		{
			GList* children = gtk_container_get_children(GTK_CONTAINER(menu));
			for (size_t i = 0; i < g_list_length(children); i++)
			{
				GtkWidget* w = (GtkWidget*) g_list_nth_data(children, i);
				gtk_container_remove(GTK_CONTAINER(menu), w);
			}
		}
	
		if (m.isNull())
			return;
	
		m->AddChildrenTo(GTK_WIDGET(menu));
	}
	

	static WebKitWebView* create_inspector_cb(
		WebKitWebInspector* webInspector,
		WebKitWebView* page,
		gpointer data)
	{
		GtkWidget* scrolledWindow;
		GtkWidget* newWebView;
		GtkUserWindow* userWindow = static_cast<GtkUserWindow*>(data);

		GtkWidget* inspectorWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
		g_signal_connect(
			G_OBJECT(inspectorWindow), "delete-event",
			G_CALLBACK(hide_window_cb), userWindow);

		gtk_window_set_title(GTK_WINDOW(inspectorWindow), "Inspector");
		gtk_window_set_default_size(GTK_WINDOW(inspectorWindow), 400, 300);
		gtk_widget_show(inspectorWindow);
		gtk_window_resize(GTK_WINDOW(inspectorWindow), 700, 500);

		scrolledWindow = gtk_scrolled_window_new(NULL, NULL);
		gtk_scrolled_window_set_policy(
			GTK_SCROLLED_WINDOW(scrolledWindow),
			GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
		gtk_container_add(GTK_CONTAINER(inspectorWindow), scrolledWindow);
		gtk_widget_show(scrolledWindow);

		newWebView = webkit_web_view_new();
		gtk_container_add(GTK_CONTAINER(scrolledWindow), newWebView);

		userWindow->SetInspectorWindow(inspectorWindow);
		return WEBKIT_WEB_VIEW(newWebView);
	}

	static gboolean inspector_show_window_cb(
		WebKitWebInspector* inspector,
		gpointer data)
	{
		GtkUserWindow* userWindow = static_cast<GtkUserWindow*>(data);
		GtkWidget* inspectorWindow = userWindow->GetInspectorWindow();
		if (inspectorWindow)
		{
			gtk_widget_show(inspectorWindow);
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}

	static void hide_window_cb(GtkWidget *widget, gpointer data)
	{
		GtkUserWindow* userWindow = static_cast<GtkUserWindow*>(data);
		GtkWidget* inspectorWindow = userWindow->GetInspectorWindow();
		if (inspectorWindow)
		{
			gtk_widget_hide(inspectorWindow);
		}
	}

	void GtkUserWindow::SetInspectorWindow(GtkWidget* inspectorWindow)
	{
		this->inspectorWindow = inspectorWindow;
	}

	GtkWidget* GtkUserWindow::GetInspectorWindow()
	{
		return this->inspectorWindow;
	}

	void GtkUserWindow::Hide()
	{
		if (this->gtkWindow != NULL)
		{
			gtk_widget_hide_all(GTK_WIDGET(this->gtkWindow));
		}
	}
	
	void GtkUserWindow::Show()
	{
		if (this->gtkWindow != NULL)
		{
			gtk_widget_show_all(GTK_WIDGET(this->gtkWindow));
		}
	}
	
	void GtkUserWindow::Minimize()
	{
		if (this->gtkWindow != NULL)
		{
			gtk_window_iconify(this->gtkWindow);
		}
		// Maximizing in GTK is asynchronous, so we prime the
		// values here in hopes that things will turn out okay.
		// Another alternative would be to block until a resize
		// is detected, but that might leave the application in
		// a funky state.
		this->gdkMinimized = true;
	}
	
	void GtkUserWindow::Unminimize()
	{
		if (this->gtkWindow != NULL)
		{
			gtk_window_deiconify(this->gtkWindow);
		}
		// Maximizing in GTK is asynchronous, so we prime the
		// values here in hopes that things will turn out okay.
		// Another alternative would be to block until a resize
		// is detected, but that might leave the application in
		// a funky state.
		this->gdkMinimized = false;
	}

	bool GtkUserWindow::IsMinimized()
	{
		if (this->gtkWindow != NULL)
		{
			return this->gdkMinimized;
		}
		else
		{
			return this->config->IsMinimized();
		}
	}

	void GtkUserWindow::Maximize()
	{
		if (this->gtkWindow != NULL)
		{
			gtk_window_maximize(this->gtkWindow);
		}

		// Maximizing in GTK is asynchronous, so we prime the
		// values here in hopes that things will turn out okay.
		// Another alternative would be to block until a resize
		// is detected, but that might leave the application in
		// a funky state.
		this->gdkMaximized = true;
	}

	void GtkUserWindow::Unmaximize()
	{
		if (this->gtkWindow != NULL)
		{
			gtk_window_unmaximize(this->gtkWindow);
		}

		// Maximizing in GTK is asynchronous, so we prime the
		// values here in hopes that things will turn out okay.
		// Another alternative would be to block until a resize
		// is detected, but that might leave the application in
		// a funky state.
		this->gdkMaximized = false;
	}

	bool GtkUserWindow::IsMaximized()
	{
		if (this->gtkWindow != NULL)
		{
			return this->gdkMaximized;
		}
		else
		{
			return this->config->IsMaximized();
		}
	}
	
	void GtkUserWindow::Focus()
	{
		if (this->gtkWindow != NULL)
			gtk_window_present(this->gtkWindow);
	}
	
	void GtkUserWindow::Unfocus(){
		if (gtk_window_has_toplevel_focus(this->gtkWindow))
		{
			gdk_window_focus(
				gdk_get_default_root_window(),
				gtk_get_current_event_time());
		}
	}
	
	bool GtkUserWindow::IsUsingScrollbars()
	{
		return this->config->IsUsingScrollbars();
	}
	
	bool GtkUserWindow::IsFullScreen()
	{
		return this->config->IsFullScreen();
	}
	
	void GtkUserWindow::SetFullScreen(bool fullscreen)
	{
		if (fullscreen && this->gtkWindow != NULL)
		{
			gtk_window_fullscreen(this->gtkWindow);
		}
		else if (this->gtkWindow != NULL)
		{
			gtk_window_unfullscreen(this->gtkWindow);
		}
	}
	
	
	std::string GtkUserWindow::GetId()
	{
		return this->config->GetID();
	}
	
	
	double GtkUserWindow::GetX()
	{
		return this->gdkX;
	}
	
	void GtkUserWindow::SetX(double x)
	{
		this->SetupPosition();
	}
	
	double GtkUserWindow::GetY()
	{
		return this->gdkY;
	}
	
	void GtkUserWindow::SetY(double y)
	{
		this->SetupPosition();
	}

	double GtkUserWindow::GetWidth()
	{
		return this->gdkWidth;
	}

	void GtkUserWindow::SetWidth(double width)
	{
		this->SetupSize();
	}

	double GtkUserWindow::GetMaxWidth()
	{
		return this->config->GetMaxWidth();
	}

	void GtkUserWindow::SetMaxWidth(double width)
	{
		this->SetupSizeLimits();
	}

	double GtkUserWindow::GetMinWidth()
	{
		return this->config->GetMinWidth();
	}

	void GtkUserWindow::SetMinWidth(double width)
	{
		this->SetupSizeLimits();
	}

	double GtkUserWindow::GetHeight()
	{
		return this->gdkHeight;
	}

	void GtkUserWindow::SetHeight(double height)
	{
		this->SetupSize();
	}

	double GtkUserWindow::GetMaxHeight()
	{
		return this->config->GetMaxHeight();
	}

	void GtkUserWindow::SetMaxHeight(double height)
	{
		this->SetupSizeLimits();
	}

	double GtkUserWindow::GetMinHeight()
	{
		return this->config->GetMinHeight();
	}

	void GtkUserWindow::SetMinHeight(double height)
	{
		this->SetupSizeLimits();
	}

	Bounds GtkUserWindow::GetBounds()
	{
		Bounds b;
		b.width = gdkWidth;
		b.height = gdkHeight;
		b.x = gdkX;
		b.y = gdkY;
		return b;
	}
	
	void GtkUserWindow::SetBounds(Bounds b)
	{
		this->SetupPosition();
		this->SetupSize();
	}
	
	std::string GtkUserWindow::GetTitle()
	{
		return this->config->GetTitle();
	}
	
	void GtkUserWindow::SetTitle(std::string& title)
	{
		if (this->gtkWindow != NULL)
		{
			std::string& ntitle = this->config->GetTitle();
			gtk_window_set_title(this->gtkWindow, ntitle.c_str());
		}
	}
	
	std::string GtkUserWindow::GetURL()
	{
		return this->config->GetURL();
	}
	
	void GtkUserWindow::SetURL(std::string& uri)
	{
		if (this->gtkWindow != NULL && this->webView != NULL)
			webkit_web_view_open(this->webView, uri.c_str());
	}
	
	bool GtkUserWindow::IsUsingChrome()
	{
		return this->config->IsUsingChrome();
	}
	
	void GtkUserWindow::SetUsingChrome(bool chrome)
	{
		if (this->gtkWindow != NULL)
			gtk_window_set_decorated(this->gtkWindow, chrome);
	}
	
	bool GtkUserWindow::IsResizable()
	{
		return this->config->IsResizable();
	}
	
	void GtkUserWindow::SetResizable(bool resizable)
	{
		if (this->gtkWindow != NULL)
			gtk_window_set_resizable(this->gtkWindow, resizable);
	}
	
	bool GtkUserWindow::IsMaximizable()
	{
		return this->config->IsMaximizable();
	}
	
	void GtkUserWindow::SetMaximizable(bool maximizable)
	{
		this->SetupDecorations();
	}
	
	bool GtkUserWindow::IsMinimizable()
	{
		return this->config->IsMinimizable();
	}
	
	void GtkUserWindow::SetMinimizable(bool minimizable)
	{
		this->SetupDecorations();
	}
	
	bool GtkUserWindow::IsCloseable()
	{
		return this->config->IsCloseable();
	}
	void GtkUserWindow::SetCloseable(bool closeable)
	{
		if (this->gtkWindow != NULL)
			gtk_window_set_deletable(this->gtkWindow, closeable);
	}
	
	bool GtkUserWindow::IsVisible()
	{
		return this->config->IsVisible();
	}
	
	double GtkUserWindow::GetTransparency()
	{
		return this->config->GetTransparency();
	}
	
	void GtkUserWindow::SetTransparency(double alpha)
	{
		if (this->gtkWindow != NULL)
			gtk_window_set_opacity(this->gtkWindow, alpha);
	}
	
	bool GtkUserWindow::IsTopMost()
	{
		return this->config->IsTopMost();
	}
	
	void GtkUserWindow::SetTopMost(bool topmost)
	{
		if (this->gtkWindow != NULL)
		{
			guint topmost_i = topmost ? TRUE : FALSE;
			gtk_window_set_keep_above(this->gtkWindow, topmost_i);
		}
	}
	
	void GtkUserWindow::SetMenu(SharedPtr<MenuItem> value)
	{
		SharedPtr<GtkMenuItemImpl> menu = value.cast<GtkMenuItemImpl>();
		this->menu = menu;
		this->SetupMenu();
	}
	
	SharedPtr<MenuItem> GtkUserWindow::GetMenu()
	{
		return this->menu;
	}
	
	void GtkUserWindow::SetContextMenu(SharedPtr<MenuItem> value)
	{
		SharedPtr<GtkMenuItemImpl> menu = value.cast<GtkMenuItemImpl>();
		this->context_menu = menu;
	}
	
	SharedPtr<MenuItem> GtkUserWindow::GetContextMenu()
	{
		return this->context_menu;
	}
	
	
	void GtkUserWindow::SetIcon(SharedString iconPath)
	{
		this->iconPath = iconPath;
		this->SetupIcon();
	}
	
	SharedString GtkUserWindow::GetIcon()
	{
		return this->iconPath;
	}
	
	void GtkUserWindow::RemoveOldMenu()
	{
	
		// Only clear a realization if we have one
		if (!this->menuInUse.isNull() && this->menuBar != NULL)
			this->menuInUse->ClearRealization(this->menuBar);
	
		// Only remove the old menu if we still have a window
		if (this->gtkWindow != NULL)
			gtk_container_remove(GTK_CONTAINER(this->vbox), this->menuBar);
	
		this->menuInUse = NULL;
		this->menuBar = NULL;
	}
	
	void GtkUserWindow::SetupMenu()
	{
		SharedPtr<GtkMenuItemImpl> menu = this->menu;
		SharedPtr<MenuItem> app_menu = UIModule::GetMenu();
	
		// No window menu, try to use the application menu.
		if (menu.isNull() && !app_menu.isNull())
		{
			menu = app_menu.cast<GtkMenuItemImpl>();
		}
	
		// Only do this if the menu is actually changing.
		if (menu == this->menuInUse)
			return;
	
		this->RemoveOldMenu();
	
		if (!menu.isNull() && this->gtkWindow != NULL)
		{
			GtkWidget* menuBar = menu->GetMenuBar();
			gtk_box_pack_start(GTK_BOX(this->vbox), menuBar,
			                   FALSE, FALSE, 2);
			gtk_box_reorder_child(GTK_BOX(this->vbox), menuBar, 0);
			gtk_widget_show(menuBar);
			this->menuBar = menuBar;
		}
	
		this->menuInUse = menu;
	
	}
	
	void GtkUserWindow::AppMenuChanged()
	{
		if (this->menu.isNull())
		{
			this->SetupMenu();
		}
	}
	
	void GtkUserWindow::AppIconChanged()
	{
		if (this->iconPath.isNull())
		{
			this->SetupIcon();
		}
	}
	
	namespace GtkUserWindowNS
	{
		std::string openFilesDirectory = "";
		SharedValue FileChooserWork(const ValueList& args)
		{
			void* data = args.at(0)->ToVoidPtr();
			FileChooserJob* job = static_cast<FileChooserJob*>(data);
			SharedKList results = new StaticBoundList();
	
			GtkFileChooserAction action;
			gchar* actionButton;
			if (job->mode == SELECT_FILE)
			{
				action = GTK_FILE_CHOOSER_ACTION_OPEN;
				actionButton = (gchar*) GTK_STOCK_OK;
			}
			else if (job->mode == SELECT_FOLDER)
			{
				action = GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER;
				actionButton = (gchar*) GTK_STOCK_OK;
			}
			else
			{
				action = GTK_FILE_CHOOSER_ACTION_SAVE;
				actionButton = (gchar*) GTK_STOCK_SAVE;
			}
		
			GtkWidget* chooser = gtk_file_chooser_dialog_new(
				job->title.c_str(),
				job->window,
				action,
				GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
				actionButton, GTK_RESPONSE_ACCEPT,
				NULL);
		
			std::string path = openFilesDirectory;
			if (!job->path.empty())
			{
				path = job->path;
			}
			if (!path.empty())
			{
				gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(chooser), path.c_str());
			}
		
			gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(chooser), job->multiple);
		
			if (job->types.size() > 0)
			{
				GtkFileFilter* f = gtk_file_filter_new();
				for (size_t fi = 0; fi < job->types.size(); fi++)
				{
					std::string filter = std::string("*.") + job->types.at(fi);
					gtk_file_filter_add_pattern(f, filter.c_str());
				}

				if (!job->typesDescription.empty())
				{
					gtk_file_filter_set_name(f, job->typesDescription.c_str());
				}
				gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(chooser), f);
			}
		
			int result = gtk_dialog_run(GTK_DIALOG(chooser));
			if (result == GTK_RESPONSE_ACCEPT && job->multiple)
			{
				GSList* files = gtk_file_chooser_get_filenames(GTK_FILE_CHOOSER(chooser));
				for (size_t i = 0; i < g_slist_length(files); i++)
				{
					char* f = (char*) g_slist_nth_data(files, i);
					results->Append(Value::NewString(f));
					g_free(f);
				}
				g_slist_free(files);
			}
			else if (result == GTK_RESPONSE_ACCEPT)
			{
				char *f = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(chooser));
				results->Append(Value::NewString(f));
				g_free(f);
			}
		
			openFilesDirectory =
				 gtk_file_chooser_get_current_folder(GTK_FILE_CHOOSER(chooser));
			gtk_widget_destroy(chooser);

			try
			{
				job->callback->Call(ValueList(Value::NewList(results)));
			}
			catch (ValueException &e)
			{
				SharedString ss = e.GetValue()->DisplayString();
				std::cerr << "openFiles callback failed: " << *ss << std::endl;
			}
			return Value::Undefined;
		}
	}
	
	using GtkUserWindowNS::FileChooserJob;
	using GtkUserWindowNS::FileChooserMode;
	void GtkUserWindow::ShowFileChooser(
		FileChooserMode mode,
		SharedKMethod callback,
		bool multiple,
		std::string& title,
		std::string& path,
		std::string& defaultName,
		std::vector<std::string>& types,
		std::string& typesDescription)
	{
		FileChooserJob* job = new FileChooserJob;
		job->window = this->gtkWindow;
		job->callback = callback;
		job->title = title;
		job->host = host;
		job->multiple = multiple;
		job->path = path;
		job->defaultName = defaultName;
		job->types = types;
		job->typesDescription = typesDescription;
		job->mode = mode;
	
		// Call this on the main thread -- so that it happens
		// at the appropriate time in the event loop.
		SharedKMethod meth =
			new kroll::KFunctionPtrMethod(&GtkUserWindowNS::FileChooserWork);
		ValueList args(Value::NewVoidPtr(job));
		job->host->InvokeMethodOnMainThread(meth, args, false);
	
	}

	void GtkUserWindow::OpenFileChooserDialog(
		SharedKMethod callback,
		bool multiple,
		std::string& title,
		std::string& path,
		std::string& defaultName,
		std::vector<std::string>& types,
		std::string& typesDescription)
	{
		this->ShowFileChooser(
			GtkUserWindowNS::SELECT_FILE,
			callback, multiple, title, path, defaultName, types, typesDescription);
	}
	
	void GtkUserWindow::OpenFolderChooserDialog(
		SharedKMethod callback,
		bool multiple,
		std::string& title,
		std::string& path,
		std::string& defaultName)
	{
		std::vector<std::string> types;
		std::string typesDescription;
		this->ShowFileChooser(
			GtkUserWindowNS::SELECT_FOLDER,
			callback, multiple, title, path, defaultName, types, typesDescription);
	}

	void GtkUserWindow::OpenSaveAsDialog(
		SharedKMethod callback,
		std::string& title,
		std::string& path,
		std::string& defaultName,
		std::vector<std::string>& types,
		std::string& typesDescription)
	{
		this->ShowFileChooser(
			GtkUserWindowNS::SAVE_FILE,
			callback, false, title, path, defaultName, types, typesDescription);
	}
}

