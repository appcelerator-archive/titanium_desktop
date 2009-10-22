/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */

#include "../ui_module.h"
#include <iostream>
#include <Poco/Process.h>
#define G_OBJECT_USER_WINDOW_KEY "gtk-user-window"
#define TRANSPARENCY_MAJOR_VERSION 2
#define TRANSPARENCY_MINOR_VERSION 16
extern const guint gtk_major_version;
extern const guint gtk_minor_version;

namespace ti
{
	enum FileChooserMode
	{
		SELECT_FILE,
		SELECT_FOLDER,
		SAVE_FILE
	};

	enum DialogType
	{
		ALERT,
		CONFIRM,
		PROMPT
	};

	static gboolean DeleteCallback(GtkWidget*, GdkEvent*, gpointer);
	static gboolean EventCallback(GtkWidget*, GdkEvent*, gpointer);
	static void WindowObjectClearedCallback(WebKitWebView*,
		WebKitWebFrame*, JSGlobalContextRef, JSObjectRef, gpointer);
	static void PopulatePopupCallback(WebKitWebView*, GtkMenu*, gpointer);
	static WebKitWebView* CreateWebViewCallback(
		WebKitWebView*, WebKitWebFrame*, gpointer);
	static gint NewWindowPolicyDecisionCallback(WebKitWebView*,
		WebKitWebFrame*, WebKitNetworkRequest*, WebKitWebNavigationAction*,
		WebKitWebPolicyDecision *);
	static void LoadFinishedCallback(WebKitWebView*, WebKitWebFrame*,
		gpointer);
	static void TitleChangedCallback(WebKitWebView*, WebKitWebFrame*,
		gchar*, gpointer);
	static void FeaturesChangedCallback(WebKitWebView* view, GParamSpec *pspec,
		 gpointer data);
	static WebKitWebView* InspectWebViewCallback(WebKitWebInspector*,
		WebKitWebView*, gpointer);
	static gboolean InspectorShowWindowCallback(WebKitWebInspector*, gpointer);
	static gboolean ScriptAlertCallback(WebKitWebView* webView,
		WebKitWebFrame *frame, gchar* message, gpointer data);
	static gboolean ScriptConfirmCallback(WebKitWebView* webView,
		WebKitWebFrame* frame, gchar* message, gboolean* confirmed,
		gpointer data);
	static gboolean ScriptPromptCallback(WebKitWebView* webView,
		WebKitWebFrame* frame, gchar* message, gchar* defaultPromptValue,
		gchar** value, gpointer data);
	static bool MakeScriptDialog(DialogType type, GtkWindow* window,
		const gchar* message, const gchar* defaultPromptResponse,
		char** promptResponse);
	static inline bool GtkVersionSupportsWebViewTransparency();

	GtkUserWindow::GtkUserWindow(WindowConfig* config, AutoUserWindow& parent) :
		UserWindow(config, parent),
		targetWidth(-1),
		targetHeight(-1),
		targetX(-1),
		targetY(-1),
		targetMaximized(false),
		targetMinimized(false),
		gtkWindow(0),
		vbox(0),
		webView(0),
		topmost(false),
		menu(0),
		activeMenu(0),
		contextMenu(0),
		nativeMenu(0),
		iconPath(""),
		inspectorWindow(0)
	{
	}

	GtkUserWindow::~GtkUserWindow()
	{
		if (this->active)
		{
			this->Close();
		}
	}

	void GtkUserWindow::Open()
	{
		if (this->gtkWindow == NULL)
		{
			this->webView = WEBKIT_WEB_VIEW(webkit_web_view_new());
			g_object_set_data(G_OBJECT(this->webView), G_OBJECT_USER_WINDOW_KEY, this);

			g_object_connect(G_OBJECT(webView),
				"signal::window-object-cleared",
				G_CALLBACK(WindowObjectClearedCallback), this,
				"signal::new-window-policy-decision-requested",
				G_CALLBACK(NewWindowPolicyDecisionCallback), this,
				"signal::populate-popup",
				G_CALLBACK(PopulatePopupCallback), this,
				"signal::load-finished",
				G_CALLBACK(LoadFinishedCallback), this,
				"signal::title-changed",
				G_CALLBACK(TitleChangedCallback), this,
				"signal::notify::window-features",
				G_CALLBACK(FeaturesChangedCallback), this,
				"signal::create-web-view",
				G_CALLBACK(CreateWebViewCallback), this,
				"signal::script-alert",
				G_CALLBACK(ScriptAlertCallback), this->gtkWindow,
				"signal::script-confirm",
				G_CALLBACK(ScriptConfirmCallback), this->gtkWindow,
				"signal::script-prompt",
				G_CALLBACK(ScriptPromptCallback), this->gtkWindow,
				NULL);

			WebKitWebSettings* settings = webkit_web_settings_new();
			g_object_set(G_OBJECT(settings), 
				"enable-developer-extras", TRUE,
				"enable-universal-access-from-file-uris", TRUE,
				"javascript-can-open-windows-automatically", TRUE,
				NULL);
			webkit_web_view_set_settings(WEBKIT_WEB_VIEW(webView), settings);

			// Get the default user agent, append the product name and version and
			// then record the new user agent in the global object.
			static std::string userAgent;
			if (userAgent.empty())
			{
				const char* cUserAgent = 0;
				g_object_get(G_OBJECT(settings), "user-agent", &cUserAgent, NULL);
				userAgent.append(cUserAgent);

				// Force the inclusion of a version string. WebKit GTK does not do
				// this by default and some misbehaving JavaScript code relies on it.
				// https://appcelerator.lighthouseapp.com/projects/25719/tickets/
				// 149-windownavigator-is-undefined-when-running-on-linux
				userAgent.append(" Version/4.0 ");
				userAgent.append(PRODUCT_NAME);
				userAgent.append("/");
				userAgent.append(STRING(PRODUCT_VERSION));
				host->GetGlobalObject()->Set("userAgent", Value::NewString(userAgent));
			}
			g_object_set(G_OBJECT(settings), "user-agent", userAgent.c_str(), NULL);

			WebKitWebInspector *inspector = webkit_web_view_get_inspector(webView);
			g_signal_connect(
				G_OBJECT(inspector), "inspect-web-view",
				G_CALLBACK(InspectWebViewCallback), this);
			g_signal_connect(
				G_OBJECT(inspector), "show-window",
				G_CALLBACK(InspectorShowWindowCallback), this);

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

			if (this->IsToolWindow())
			{
				gtk_window_set_skip_taskbar_hint(GTK_WINDOW(window), TRUE);
				gtk_window_set_skip_pager_hint(GTK_WINDOW(window), TRUE);
			}

			gtk_widget_set_name(window, this->config->GetTitle().c_str());
			gtk_window_set_title(GTK_WINDOW(window), this->config->GetTitle().c_str());

			this->deleteCallbackId = g_signal_connect(
				G_OBJECT(window), "delete-event", G_CALLBACK(DeleteCallback), this);
			g_signal_connect(
				G_OBJECT(window), "event", G_CALLBACK(EventCallback), this);

			gtk_container_add(GTK_CONTAINER(window), vbox);
	
			this->gtkWindow = GTK_WINDOW(window);
			this->SetupTransparency();

			gtk_widget_realize(window);
			this->SetupDecorations();
			this->SetupSize();
			this->SetupSizeLimits();
			this->SetupPosition();
			this->SetupMenu();
			this->SetupIcon();
			this->SetTopMost(config->IsTopMost());
			this->SetCloseable(config->IsCloseable());
			this->SetResizable(config->IsResizable());

			gtk_widget_grab_focus(GTK_WIDGET(webView));
			webkit_web_view_open(webView, this->config->GetURL().c_str());

			if (this->IsVisible())
			{
				gtk_widget_show_all(window);
			}
	
			if (this->config->IsFullscreen())
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
			this->FireEvent(Event::OPENED);
		}
		else
		{
			this->Show();
		}
	}

	static gboolean DeleteCallback(GtkWidget* widget, GdkEvent* event, gpointer data)
	{
		// Let the close handler actually destroy this window,
		// as we want things to happen in a very particular order.
		GtkUserWindow* userWindow = (GtkUserWindow*) data;
		return !userWindow->Close();
	}

	bool GtkUserWindow::Close()
	{
		if (!this->active)
			return false;

		UserWindow::Close();

		// If the window is still active at this point, it
		// indicates an event listener has cancelled this close event.
		if (!this->active)
		{
			// Destroy the GTK bits, if we have them first, because
			// we need to assume the GTK window is gone for  everything
			// below (this method might be called by DeleteCallback)
			if (this->gtkWindow != NULL)
			{
				// We don't want the destroy signal handler to fire after now.
				g_signal_handler_disconnect(this->gtkWindow, this->deleteCallbackId);
				gtk_widget_destroy(GTK_WIDGET(this->gtkWindow));

				this->gtkWindow = NULL;
				this->webView = NULL;
			}
			this->RemoveOldMenu(); // Cleanup old menu

			this->Closed();
		}

		return !this->active;
	}
	
	void GtkUserWindow::SetupTransparency()
	{
		if (this->gtkWindow && GtkVersionSupportsWebViewTransparency())
		{
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

			if (!config->IsResizable())
			{
				hints.max_width = this->config->GetWidth();
				hints.max_height = this->config->GetHeight();
				hints.min_width = this->config->GetWidth();
				hints.min_height = this->config->GetHeight();
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
			if (x == UIBinding::CENTERED)
			{
				x = (gdk_screen_get_width(screen) - this->GetWidth()) / 2;
				this->config->SetX(x);
			}
			if (y == UIBinding::CENTERED)
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
			this->targetX = x;
			this->targetY = y;
		}
	}
	
	void GtkUserWindow::SetupSize()
	{
		if (this->gtkWindow != NULL)
		{
			if (this->IsResizable())
				gtk_window_resize(this->gtkWindow,
					(int) this->config->GetWidth(),
					(int) this->config->GetHeight());
			else
				this->SetupSizeLimits();
	
			// Resizing in GTK is asynchronous, so we prime the
			// values here in hopes that things will turn out okay.
			// Another alternative would be to block until a resize
			// is detected, but that might leave the application in
			// a funky state.
			this->targetWidth = this->config->GetWidth();
			this->targetHeight = this->config->GetHeight();
		}
	}
	
	void GtkUserWindow::SetupIcon()
	{
		if (this->gtkWindow == NULL)
			return;
	
		GdkPixbuf* icon = NULL; // NULL is an unset.
		std::string iconPath = this->iconPath;

		if (iconPath.empty()) {
			GtkUIBinding* b = static_cast<GtkUIBinding*>(UIBinding::GetInstance());
			iconPath = b->GetIcon();
		}
	
		if (!iconPath.empty())
		{
			GError* error = NULL;
			icon = gdk_pixbuf_new_from_file(iconPath.c_str(), &error);
	
			if (icon == NULL && error != NULL) {
				logger->Error("Failed to load icon: %s\n", error->message);
				g_error_free(error);
			}
		}
		gtk_window_set_icon(this->gtkWindow, icon);
	}

	static gboolean EventCallback(
		GtkWidget *w,
		GdkEvent *event,
		gpointer data)
	{
		static int oldWidth = -1;
		static int oldHeight = -1;
		static int oldX = -1;
		static int oldY = -1;

		GtkUserWindow* window = (GtkUserWindow*) data;
		if (event->type == GDK_FOCUS_CHANGE)
		{
			GdkEventFocus* f = (GdkEventFocus*) event;
			if (f->in)
			{
				window->FireEvent(Event::FOCUSED);
			}
			else
			{
				window->FireEvent(Event::UNFOCUSED);
			}
		}
		else if (event->type == GDK_WINDOW_STATE)
		{
			GdkEventWindowState* f = (GdkEventWindowState*) event;
			if ((f->changed_mask & GDK_WINDOW_STATE_WITHDRAWN)
				&& (f->new_window_state & GDK_WINDOW_STATE_WITHDRAWN))
			{
				window->FireEvent(Event::HIDDEN);
			}

			if ((f->changed_mask & GDK_WINDOW_STATE_ICONIFIED)
				&& (f->new_window_state & GDK_WINDOW_STATE_ICONIFIED))
			{
				window->FireEvent(Event::MINIMIZED);
			}

			if (((f->changed_mask & GDK_WINDOW_STATE_FULLSCREEN)
				&& (f->new_window_state & GDK_WINDOW_STATE_FULLSCREEN)))
			{
				window->FireEvent(Event::FULLSCREENED);
			}

			if (f->changed_mask & GDK_WINDOW_STATE_FULLSCREEN)
			{
				window->FireEvent(Event::UNFULLSCREENED);
			}

			if (((f->changed_mask & GDK_WINDOW_STATE_MAXIMIZED)
				&& (f->new_window_state & GDK_WINDOW_STATE_MAXIMIZED)))
			{
				window->FireEvent(Event::MAXIMIZED);
			}

			window->targetMinimized =
				f->new_window_state & GDK_WINDOW_STATE_ICONIFIED;

			window->targetMaximized =
				f->new_window_state & GDK_WINDOW_STATE_MAXIMIZED;
		}
		else if (event->type == GDK_CONFIGURE)
		{
			GdkEventConfigure* c = (GdkEventConfigure*) event;
			if ((oldX != -1 && c->x != oldX) || 
				(oldY != -1 && c->y != oldY))
			{
				window->targetX = c->x;
				window->targetY = c->y;
				window->FireEvent(Event::MOVED);
			}

			if ((oldWidth != -1 && c->width != oldWidth) ||
				(oldHeight != -1 && c->height != oldHeight))
			{
				window->targetHeight = c->height;
				window->targetWidth = c->width;
				window->FireEvent(Event::RESIZED);
			}

			oldHeight = c->height;
			oldWidth = c->width;
			oldX = c->x;
			oldY = c->y;
		}
	
		return FALSE;
	}

	WebKitWebView* CreateWebViewCallback(WebKitWebView* webView,
		WebKitWebFrame* frame, gpointer data)
	{
		GtkUserWindow* userWindow = static_cast<GtkUserWindow*>(data);
		AutoUserWindow newWindow = userWindow->CreateWindow(new WindowConfig());
		newWindow->Open();

		AutoPtr<GtkUserWindow> gtkNewWindow = newWindow.cast<GtkUserWindow>();
		if (!gtkNewWindow.isNull())
		{
			// We aren't saving a reference to this new window, but we don't
			// want it to disappear either. We'll release this once the window
			// is shown.
			return gtkNewWindow->GetWebView();
		}
		else
		{
			// Don't you dare ever happen.
			return 0;
		}
	}

	static gint NewWindowPolicyDecisionCallback(WebKitWebView* webView,
		WebKitWebFrame* frame, WebKitNetworkRequest* request,
		WebKitWebNavigationAction* navigationAction,
		WebKitWebPolicyDecision *policyDecision)
	{
		const char* frameName = webkit_web_navigation_action_get_target_frame(navigationAction);
		gchar* frame_name_case = g_utf8_casefold(frameName, g_utf8_strlen(frameName, -1));
		if (g_utf8_collate(frame_name_case, "ti:systembrowser") == 0 ||
			g_utf8_collate(frame_name_case, "_blank") == 0)
		{
			gchar* url = strdup(webkit_network_request_get_uri(request));
			if (url[strlen(url)-1] == '/')
				url[strlen(url)-1] = '\0';
	
			std::vector<std::string> args;
			args.push_back(std::string(url));
			Poco::Process::launch("xdg-open", args);
			webkit_web_policy_decision_ignore(policyDecision);
		}
		else
		{
			webkit_web_policy_decision_use(policyDecision);
		}

		return TRUE;
	}
	
	static void LoadFinishedCallback(WebKitWebView* view, WebKitWebFrame* frame,
		gpointer data)
	{
		JSGlobalContextRef context = webkit_web_frame_get_global_context(frame);
		JSObjectRef global_object = JSContextGetGlobalObject(context);
		KObjectRef frame_global = new KKJSObject(context, global_object);

		// If uri is NULL, then likely this is the result of a cancel,
		// so don't report it as a PageLoad
		const gchar* uri = webkit_web_frame_get_uri(frame);
		if (uri) {
			std::string uriString = uri;
			GtkUserWindow* userWindow = static_cast<GtkUserWindow*>(data);
			userWindow->PageLoaded(frame_global, uriString, context);
		}
	}

	static void TitleChangedCallback(WebKitWebView* view, WebKitWebFrame* frame,
		gchar* newTitle, gpointer data)
	{
		GtkUserWindow* userWindow = (GtkUserWindow*) data;
		if (NULL == webkit_web_frame_get_parent(frame))
		{
			std::string newTitleString = newTitle;
			userWindow->SetTitle(newTitleString);
		}
	}

	static void FeaturesChangedCallback(WebKitWebView* view, GParamSpec *pspec, gpointer data)
	{
		GtkUserWindow* userWindow = (GtkUserWindow*) data;
		WebKitWebWindowFeatures* features = webkit_web_view_get_window_features(view);

		gint width, height, x, y;
		g_object_get(features,
			"width", &width,
			"height", &height,
			"x", &x,
			"y", &y,
			NULL);

		if (width != -1)
			userWindow->_SetWidth((double) width);
		if (height != -1)
			userWindow->_SetHeight((double) height);
		if (x != -1)
			userWindow->_SetX((double) x);
		if (y != -1)
			userWindow->_SetY((double) y);
	}

	static void WindowObjectClearedCallback(
		WebKitWebView* webView,
		WebKitWebFrame* web_frame,
		JSGlobalContextRef context,
		JSObjectRef window_object,
		gpointer data)
	{
	
		GtkUserWindow* userWindow = (GtkUserWindow*) data;
		userWindow->RegisterJSContext(context);
	}
	
	static void PopulatePopupCallback(
		WebKitWebView *webView,
		GtkMenu *menu,
		gpointer data)
	{
		GtkUserWindow* userWindow = static_cast<GtkUserWindow*>(data);
		AutoPtr<GtkMenu> m = userWindow->GetContextMenu().cast<GtkMenu>();
	
		if (m.isNull()) {
			GtkUIBinding* b = static_cast<GtkUIBinding*>(UIBinding::GetInstance());
			m = b->GetContextMenu().cast<GtkMenu>();
		}
	
		// Remove existing unused context menu items
		GList* children = gtk_container_get_children(GTK_CONTAINER(menu));
		size_t extent = g_list_length(children);

		// If we are in debug mode, leave the last two --
		// a separator and the web inspector
		if (userWindow->GetHost()->IsDebugMode())
			extent = extent - 2;

		// If we are not in debug mode, remove the default WebKit menu items
		for (size_t i = 0; i < extent; i++)
		{
			GtkWidget* w = (GtkWidget*) g_list_nth_data(children, i);
			gtk_container_remove(GTK_CONTAINER(menu), w);
		}

		if (!m.isNull()) {
			m->AddChildrenToNativeMenu(GTK_MENU_SHELL(menu), false);
		}
	}
	

	static WebKitWebView* InspectWebViewCallback(
		WebKitWebInspector* webInspector,
		WebKitWebView* page,
		gpointer data)
	{
		GtkWidget* scrolledWindow;
		GtkWidget* newWebView;
		GtkUserWindow* userWindow = static_cast<GtkUserWindow*>(data);
		GtkWidget* inspectorWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);

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

	static gboolean InspectorShowWindowCallback(
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

	static inline bool GtkVersionSupportsWebViewTransparency()
	{
		// This is disabled until issues with
		// WebKit and RGBA colormaps are solved.
		return false;

		//return gtk_major_version >= TRANSPARENCY_MAJOR_VERSION &&
		//	gtk_minor_version >= TRANSPARENCY_MINOR_VERSION;
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

			// There is no GDK event to detect when the window is shown,
			// so we force the firing of this event here.
			this->FireEvent(Event::SHOWN);
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
		this->targetMinimized = true;
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
		this->targetMinimized = false;
	}

	bool GtkUserWindow::IsMinimized()
	{
		if (this->gtkWindow != NULL)
		{
			return this->targetMinimized;
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
		this->targetMaximized = true;
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
		this->targetMaximized = false;
	}

	bool GtkUserWindow::IsMaximized()
	{
		if (this->gtkWindow != NULL)
		{
			return this->targetMaximized;
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
	
	void GtkUserWindow::Unfocus()
	{
		if (gtk_window_has_toplevel_focus(this->gtkWindow))
		{
			gdk_window_focus(
				gdk_get_default_root_window(),
				gtk_get_current_event_time());

			// FIXME: GDK unfocus events seem to fire very spottily, so we
			// force the event to fire here. This means that the UNFOCUSED event
			// may fire twice.
			FireEvent(Event::UNFOCUSED);
		}
	}
	
	bool GtkUserWindow::IsUsingScrollbars()
	{
		return this->config->IsUsingScrollbars();
	}
	
	bool GtkUserWindow::IsFullscreen()
	{
		return this->config->IsFullscreen();
	}
	
	void GtkUserWindow::SetFullscreen(bool fullscreen)
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
		return this->targetX;
	}
	
	void GtkUserWindow::SetX(double x)
	{
		this->SetupPosition();
	}
	
	double GtkUserWindow::GetY()
	{
		return this->targetY;
	}
	
	void GtkUserWindow::SetY(double y)
	{
		this->SetupPosition();
	}

	double GtkUserWindow::GetWidth()
	{
		return this->targetWidth;
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
		return this->targetHeight;
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
		b.width = targetWidth;
		b.height = targetHeight;
		b.x = targetX;
		b.y = targetY;
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
	
	void GtkUserWindow::SetTitleImpl(std::string& title)
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
		{
			gtk_window_set_resizable(this->gtkWindow, resizable ? TRUE : FALSE);
			this->SetupSizeLimits();
		}
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
		{
			gtk_window_set_opacity(this->gtkWindow, alpha);
			if (GtkVersionSupportsWebViewTransparency())
				webkit_web_view_set_transparent(this->webView, alpha < 1.0);
		}
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
	
	void GtkUserWindow::SetMenu(AutoMenu value)
	{
		AutoPtr<GtkMenu> menu = value.cast<GtkMenu>();
		this->menu = menu;
		this->SetupMenu();
	}

	AutoMenu GtkUserWindow::GetMenu()
	{
		return this->menu;
	}

	void GtkUserWindow::SetContextMenu(AutoMenu value)
	{
		AutoPtr<GtkMenu> menu = value.cast<GtkMenu>();
		this->contextMenu = menu;
	}

	AutoMenu GtkUserWindow::GetContextMenu()
	{
		return this->contextMenu;
	}

	void GtkUserWindow::SetIcon(std::string& iconPath)
	{
		this->iconPath = iconPath;
		this->SetupIcon();
	}

	std::string& GtkUserWindow::GetIcon()
	{
		return this->iconPath;
	}

	void GtkUserWindow::RemoveOldMenu()
	{
		if (!this->activeMenu.isNull() && this->nativeMenu) {
			this->activeMenu->DestroyNative(GTK_MENU_SHELL(this->nativeMenu));
		}

		if (this->gtkWindow != NULL && this->nativeMenu) {
			gtk_container_remove(GTK_CONTAINER(this->vbox), GTK_WIDGET(this->nativeMenu));
		}

		this->activeMenu = 0;
		this->nativeMenu = 0;
	}

	void GtkUserWindow::SetupMenu()
	{
		AutoPtr<GtkMenu> menu = this->menu;
	
		// No window menu, try to use the application menu.
		if (menu.isNull())
		{
			GtkUIBinding* b = static_cast<GtkUIBinding*>(UIBinding::GetInstance());
			menu = b->GetMenu().cast<GtkMenu>();
		}
	
		// Only do this if the menu is actually changing.
		if (menu.get() == this->activeMenu.get())
			return

		this->RemoveOldMenu();
		if (!menu.isNull() && this->gtkWindow)
		{
			GtkMenuBar* newNativeMenu = GTK_MENU_BAR(menu->CreateNativeBar(true));
			gtk_box_pack_start(GTK_BOX(this->vbox), GTK_WIDGET(newNativeMenu), FALSE, FALSE, 2);
			gtk_box_reorder_child(GTK_BOX(this->vbox), GTK_WIDGET(newNativeMenu), 0);
			gtk_widget_show_all(GTK_WIDGET(newNativeMenu));
			this->nativeMenu = newNativeMenu;
		}
		this->activeMenu = menu;
	}

	void GtkUserWindow::AppMenuChanged()
	{
		if (this->menu.isNull())
			this->SetupMenu();
	}

	void GtkUserWindow::AppIconChanged()
	{
		if (this->iconPath.empty())
			this->SetupIcon();
	}

	struct FileChooserJob
	{
		GtkWindow* window;
		KMethodRef callback;
		FileChooserMode mode;
		bool multiple;
		std::string title;
		std::string path;
		std::string defaultName;
		std::vector<std::string> types;
		std::string typesDescription;
	};

	static KValueRef FileChooserWork(const ValueList& args)
	{
		void* data = args.at(0)->ToVoidPtr();
		FileChooserJob* job = static_cast<FileChooserJob*>(data);
		KListRef results = new StaticBoundList();
		static std::string openFilesDirectory("");
	
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
	
		std::string path(openFilesDirectory);
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
			Logger* logger = Logger::Get("UI.GtkUserWindow");
			logger->Error("openFiles callback failed: %s", e.ToString().c_str());
		}

		delete job;
		return Value::Undefined;
	}
	

	void GtkUserWindow::OpenFileChooserDialog(KMethodRef callback,
		bool multiple, std::string& title, std::string& path,
		std::string& defaultName, std::vector<std::string>& types,
		std::string& typesDescription)
	{
		FileChooserJob* job = new FileChooserJob;
		job->window = this->gtkWindow;
		job->callback = callback;
		job->title = title;
		job->multiple = multiple;
		job->path = path;
		job->defaultName = defaultName;
		job->types = types;
		job->typesDescription = typesDescription;
		job->mode = SELECT_FILE;

		KMethodRef work(new kroll::KFunctionPtrMethod(&FileChooserWork));
		ValueList args(Value::NewVoidPtr(job));
		Host::GetInstance()->InvokeMethodOnMainThread(work, args, false);
	}

	void GtkUserWindow::OpenFolderChooserDialog(KMethodRef callback,
		bool multiple, std::string& title, std::string& path,
		std::string& defaultName)
	{
		std::vector<std::string> types;
		std::string typesDescription;

		FileChooserJob* job = new FileChooserJob;
		job->window = this->gtkWindow;
		job->callback = callback;
		job->title = title;
		job->multiple = multiple;
		job->path = path;
		job->defaultName = defaultName;
		job->types = types;
		job->typesDescription = typesDescription;
		job->mode = SELECT_FOLDER;

		KMethodRef work(new kroll::KFunctionPtrMethod(&FileChooserWork));
		ValueList args(Value::NewVoidPtr(job));
		Host::GetInstance()->InvokeMethodOnMainThread(work, args, false);
	}

	void GtkUserWindow::OpenSaveAsDialog(KMethodRef callback,
		std::string& title, std::string& path, std::string& defaultName,
		std::vector<std::string>& types, std::string& typesDescription)
	{
		FileChooserJob* job = new FileChooserJob;
		job->window = this->gtkWindow;
		job->callback = callback;
		job->title = title;
		job->multiple = false;
		job->path = path;
		job->defaultName = defaultName;
		job->types = types;
		job->typesDescription = typesDescription;
		job->mode = SAVE_FILE;

		KMethodRef work(new kroll::KFunctionPtrMethod(&FileChooserWork));
		ValueList args(Value::NewVoidPtr(job));
		Host::GetInstance()->InvokeMethodOnMainThread(work, args, false);
	}

	void GtkUserWindow::ShowInspector(bool console)
	{
		WebKitWebInspector *inspector = webkit_web_view_get_inspector(webView);
		
		// TODO we need to expose showConsole ala win32/osx
		if (!GetInspectorWindow())
		{
			// calling these callbacks directly probably isn't the best way to do this?
			InspectWebViewCallback(inspector, webView, this);
		}
		gtk_widget_show(GetInspectorWindow());
	}

	static gboolean ScriptAlertCallback(WebKitWebView* webView,
		WebKitWebFrame *frame, gchar* message, gpointer data)
	{
		GtkWindow* window = reinterpret_cast<GtkWindow*>(data);
		MakeScriptDialog(ALERT, window, message, NULL, NULL);
		return TRUE;
	}

	static gboolean ScriptConfirmCallback(WebKitWebView* webView,
		WebKitWebFrame* frame, gchar* message, gboolean* confirmed,
		gpointer data)
	{
		GtkWindow* window = reinterpret_cast<GtkWindow*>(data);
		*confirmed = MakeScriptDialog(CONFIRM, window, message, NULL, NULL);
		return TRUE;
	}

	static gboolean ScriptPromptCallback(WebKitWebView* webView,
		WebKitWebFrame* frame, gchar* message, gchar* defaultPromptValue,
		gchar** value, gpointer data)
	{
		GtkWindow* window = reinterpret_cast<GtkWindow*>(data);
		if (!MakeScriptDialog(PROMPT, window, message, defaultPromptValue, value))
			*value = NULL;
		return TRUE;
	}

	static bool MakeScriptDialog(DialogType type, GtkWindow* window,
		const gchar* message, const gchar* defaultPromptResponse,
		char** promptResponse)
	{

		GtkWidget* dialog = NULL;
		GtkWidget* field = 0;

		if (type == ALERT)
		{
			dialog = gtk_message_dialog_new(window, GTK_DIALOG_DESTROY_WITH_PARENT,
				GTK_MESSAGE_WARNING, GTK_BUTTONS_CLOSE, "%s", message);
			gtk_dialog_set_default_response(GTK_DIALOG(dialog), GTK_RESPONSE_CLOSE);
		}
		else if (type == CONFIRM)
		{
			dialog = gtk_message_dialog_new(window, GTK_DIALOG_DESTROY_WITH_PARENT,
				GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO, "%s", message);
			gtk_dialog_set_default_response(GTK_DIALOG(dialog), GTK_RESPONSE_YES);
		}
		else if (type == PROMPT)
		{
			dialog = gtk_message_dialog_new(window, GTK_DIALOG_DESTROY_WITH_PARENT,
				GTK_MESSAGE_QUESTION, GTK_BUTTONS_OK_CANCEL, "%s", message);
			gtk_dialog_set_default_response(GTK_DIALOG(dialog), GTK_RESPONSE_OK);

			field = gtk_entry_new();
			gtk_entry_set_text(GTK_ENTRY(field), defaultPromptResponse);
			gtk_container_add(GTK_CONTAINER(GTK_DIALOG(dialog)->vbox), field);
			gtk_entry_set_activates_default(GTK_ENTRY(field), TRUE);
			gtk_widget_show(field);
		}

		gtk_window_set_title(GTK_WINDOW(dialog),
			Host::GetInstance()->GetApplication()->name.c_str());

		gint response = gtk_dialog_run(GTK_DIALOG(dialog));
		bool toReturn = ((type == PROMPT && response == GTK_RESPONSE_OK) ||
			(type == CONFIRM && response == GTK_RESPONSE_YES));

		if (toReturn && field)
			*promptResponse = g_strdup(gtk_entry_get_text(GTK_ENTRY(field)));

		gtk_widget_destroy(dialog);
		return toReturn;
	}
}

