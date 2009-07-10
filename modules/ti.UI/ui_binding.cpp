/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#include "ui_module.h"
#include <string>

namespace ti
{
	UIBinding* UIBinding::instance = NULL;

	// Module constants
	int UIBinding::CENTERED = WindowConfig::DEFAULT_POSITION;
	std::string UIBinding::FOCUSED = "focused";
	std::string UIBinding::UNFOCUSED = "unfocused";
	std::string UIBinding::OPEN = "open";
	std::string UIBinding::OPENED = "opened";
	std::string UIBinding::CLOSE = "close";
	std::string UIBinding::CLOSED = "closed";
	std::string UIBinding::HIDDEN = "hidden";
	std::string UIBinding::SHOWN = "shown";
	std::string UIBinding::FULLSCREENED = "fullscreened";
	std::string UIBinding::UNFULLSCREENED = "unfullscreened";
	std::string UIBinding::MAXIMIZED = "maximized";
	std::string UIBinding::MINIMIZED = "minimized";
	std::string UIBinding::RESIZED = "resized";
	std::string UIBinding::MOVED = "moved";
	std::string UIBinding::PAGE_INITIALIZED = "page.init";
	std::string UIBinding::PAGE_LOADED = "page.load";
	std::string UIBinding::CREATE = "create";
	std::string UIBinding::ACTIVATE = "activate";
	std::string UIBinding::CLICKED = "clicked";

	UIBinding::UIBinding(Host *host) :
		AccessorBoundObject("UI"),
		host(host)
	{
		instance = this;

		// @tiproperty[integer, UI.CENTERED, since=1.0] The CENTERED event constant
		this->Set("CENTERED", Value::NewInt(UIBinding::CENTERED));
		// @tiproperty[integer, UI.FOCUSED, since=1.0] The FOCUSED event constant
		this->Set("FOCUSED", Value::NewString(UIBinding::FOCUSED));
		// @tiproperty[integer, UI.UNFOCUSED, since=1.0] The UNFOCUSED event constant
		this->Set("UNFOCUSED", Value::NewString(UIBinding::UNFOCUSED));
		// @tiproperty[integer, UI.OPEN, since=1.0] The OPEN event constant
		this->Set("OPEN", Value::NewString(UIBinding::OPEN));
		// @tiproperty[integer, UI.OPENED, since=1.0] The OPENED event constant
		this->Set("OPENED", Value::NewString(UIBinding::OPENED));
		// @tiproperty[integer, UI.CLOSE, since=1.0] The CLOSE event constant
		this->Set("CLOSE", Value::NewString(UIBinding::CLOSE));
		// @tiproperty[integer, UI.CLOSED, since=1.0] The CLOSED event constant
		this->Set("CLOSED", Value::NewString(UIBinding::CLOSED));
		// @tiproperty[integer, UI.HIDDEN, since=1.0] The HIDDEN event constant
		this->Set("HIDDEN", Value::NewString(UIBinding::HIDDEN));
		// @tiproperty[integer, UI.SHOWN, since=1.0] The SHOWN event constant
		this->Set("SHOWN", Value::NewString(UIBinding::SHOWN));
		// @tiproperty[integer, UI.FULLSCREENED, since=1.0] The FULLSCREENED event constant
		this->Set("FULLSCREENED", Value::NewString(UIBinding::FULLSCREENED));
		// @tiproperty[integer, UI.UNFULLSCREENED, since=1.0] The UNFULLSCREENED event constant
		this->Set("UNFULLSCREENED", Value::NewString(UIBinding::UNFULLSCREENED));
		// @tiproperty[integer, UI.MAXIMIZED, since=1.0] The MAXIMIZED event constant
		this->Set("MAXIMIZED", Value::NewString(UIBinding::MAXIMIZED));
		// @tiproperty[integer, UI.MINIMIZED, since=1.0] The MINIMIZED event constant
		this->Set("MINIMIZED", Value::NewString(UIBinding::MINIMIZED));
		// @tiproperty[integer, UI.RESIZED, since=1.0] The RESIZED event constant
		this->Set("RESIZED", Value::NewString(UIBinding::RESIZED));
		// @tiproperty[integer, UI.MOVED, since=1.0] The MOVED event constant
		this->Set("MOVED", Value::NewString(UIBinding::MOVED));
		// @tiproperty[integer, UI.PAGE_INITIALIZED, since=1.0] The PAGE_INITIALIZED event constant
		this->Set("PAGE_INITIALIZED", Value::NewString(UIBinding::PAGE_INITIALIZED));
		// @tiproperty[integer, UI.PAGE_LOADED, since=1.0] The PAGE_LOADED event constant
		this->Set("PAGE_LOADED", Value::NewString(UIBinding::PAGE_LOADED));
		// @tiproperty[integer, UI.CREATE, since=1.0] The CREATE event constant
		this->Set("CREATE", Value::NewString(UIBinding::CREATE));

		/**
		 * @tiapi(method=True,name=UI.createMenu,version=1.0) Create an empty Menu object
		 * @tiresult(for=UI.createMenu,type=object) a Menu object
		 */
		this->SetMethod("createMenu", &UIBinding::_CreateMenu);

		/**
		 * @tiapi(method=True,name=UI.createMenuItem,version=1.0) Create a new MenuItem object
		 * @tiarg[String, label] The label for this menu itemi
		 * @tiarg[Function, eventListener, optional=True] An event listener for this menu item
		 * @tiarg[String, iconURL, optional=True] A URL to an icon to use for this menu item
		 * @tiresult[UI.MenuItem] The new MenuItem object
		 */
		this->SetMethod("createMenuItem", &UIBinding::_CreateMenuItem);

		/**
		 * @tiapi(method=True,name=UI.createCheckMenuItem,version=1.0)
		 * @tiapi Create a new CheckMenuItem object
		 * @tiarg[String, label] The label for this menu itemi
		 * @tiarg[Function, eventListener, optional=True] An event listener for this menu item
		 * @tiresult[UI.CheckMenuItem] The new CheckMenuItem object
		 */
		this->SetMethod("createCheckMenuItem", &UIBinding::_CreateCheckMenuItem);

		/**
		 * @tiapi(method=True,name=UI.createSeperatorMenuItem,version=1.0) Create a new SeparatorMenuItem object
		 * @tiresult[UI.SeparatorMenuItem] The new SeparatorMenuItem object
		 */
		this->SetMethod("createSeparatorMenuItem", &UIBinding::_CreateSeparatorMenuItem);

		/**
		 * @tiapi(method=True,name=UI.setMenu,version=0.2) Sets a menu for the application
		 * @tiarg[UI.Menu|null, menu] A Menu object to use as the menu or null to unset the menu
		 */
		this->SetMethod("setMenu", &UIBinding::_SetMenu);

		/**
		 * @tiapi(method=True,name=UI.getMenu,version=0.2) Returns the application's main MenuItem
		 * @tiresult[UI.Menu|null] The application's main menu
		 */
		this->SetMethod("getMenu", &UIBinding::_GetMenu);

		/**
		 * @tiapi(method=True,name=UI.setContextMenu,version=0.2) Sets the application's context menu
		 * @tiarg(for=UI.setContextMenu,type=UI.Menu|null,name=menu) a MenuItem object or null to unset
		 */
		this->SetMethod("setContextMenu", &UIBinding::_SetContextMenu);

		/**
		 * @tiapi(method=True,name=UI.getContextMenu,version=0.2) Returns the application context menu
		 * @tiresult(for=UI.getContextMenu,type=UI.Menu|null) the application's context MenuItem object
		 */
		this->SetMethod("getContextMenu", &UIBinding::_GetContextMenu);

		/**
		 * @tiapi(method=True,name=UI.setIcon,version=0.2) Sets the application's icon
		 * @tiarg(for=UI.setIcon,type=String,name=menu) path to the icon
		 */
		this->SetMethod("setIcon", &UIBinding::_SetIcon);

		/**
		 * @tiapi(method=True,name=UI.addTray,version=0.2,deprecated=True)
		 * @tiapi Create and add a tray icon
		 * @tiarg[String, iconURL] URL to the icon to use for this tray item
		 * @tiarg[Function, eventListener, optional=True] Event listener to add for this item
		 */
		this->SetMethod("addTray", &UIBinding::_AddTray);

		/**
		 * @tiapi(method=True,name=UI.clearTray,version=0.2)
		 * @tiapi Empty the tray of all this application's tray items
		 */
		this->SetMethod("clearTray", &UIBinding::_ClearTray);

		/**
		 * @tiapi(method=True,name=UI.setDockIcon,version=0.2) Sets the dock icon
		 * @tiarg(for=UI.setDockIcon,type=string,name=icon) path to the icon
		 */
		this->SetMethod("setDockIcon", &UIBinding::_SetDockIcon);

		/**
		 * @tiapi(method=True,name=UI.setDockMenu,version=0.2) Sets the dock menu
		 * @tiarg(for=UI.setDockMenu,type=object,name=menu) a MenuItem object
		 */
		this->SetMethod("setDockMenu", &UIBinding::_SetDockMenu);

		/**
		 * @tiapi(method=True,name=UI.setBadge,version=0.2) Sets the application's badge value
		 * @tiarg(for=UI.setBadge,type=string,name=badge) badge value
		 */
		this->SetMethod("setBadge", &UIBinding::_SetBadge);

		/**
		 * @tiapi(method=True,name=UI.setBadgeImage,version=0.2) Sets the application's badge image
		 * @tiarg(for=UI.setBadge,type=string,name=badge_image) path to badge image
		 */
		this->SetMethod("setBadgeImage", &UIBinding::_SetBadgeImage);

		/**
		 * @tiapi(method=True,name=UI.getIdleTime,version=0.2)
		 * @tiapi Returns the user's idle time (for the desktop, not just the application)
		 * @tiresult(for=UI.getIdleTime,type=double) the idle time as a double
		 */
		this->SetMethod("getIdleTime", &UIBinding::_GetIdleTime);

		/**
		 * @tiapi(method=True,name=UI.getOpenWindows,version=0.4) Returns the list of currently open windows
		 * @tiresult(for=UI.getOpenWindows,type=Array<UI.UserWindow>) the list of open windows
		 */
		/**
		 * @tiapi(method=True,name=UI.getWindows,version=0.4) Returns the list of currently open windows
		 * @tiresult(for=UI.getWindows,type=Array<UI.UserWindow>) the list of open windows
		 */
		this->SetMethod("getOpenWindows", &UIBinding::_GetOpenWindows);
		this->SetMethod("getWindows", &UIBinding::_GetOpenWindows);

		/**
		 * @tiapi(method=True,name=UI.getMainWindow,version=1.0)
		 * @tiapi Return the application's main window
		 * @tiresult[UI.UserWindow] The main window for this application
		 */
		this->SetMethod("getMainWindow", &UIBinding::_GetMainWindow);

		SharedKObject global = host->GetGlobalObject();
		SharedValue ui_binding_val = Value::NewObject(this);
		global->Set("UI", ui_binding_val);
	}

	void UIBinding::CreateMainWindow(WindowConfig* config)
	{
		AutoPtr<UserWindow> no_parent = NULL;
		this->mainWindow = this->CreateWindow(config, no_parent);
		this->mainWindow->Open();
	}

	void UIBinding::ErrorDialog(std::string msg)
	{
		std::cerr << msg << std::endl;
	}

	UIBinding::~UIBinding()
	{
	}

	Host* UIBinding::GetHost()
	{
		return host;
	}

	std::vector<AutoUserWindow>& UIBinding::GetOpenWindows()
	{
		return this->openWindows;
	}

	void UIBinding::AddToOpenWindows(AutoUserWindow window)
	{
		this->openWindows.push_back(window);
	}

	void UIBinding::RemoveFromOpenWindows(AutoUserWindow window)
	{
		static Logger* logger = Logger::Get("UI");
		std::vector<AutoUserWindow>::iterator w = openWindows.begin();
		while (w != openWindows.end())
		{
			if ((*w).get() == window.get())
			{
				w = this->openWindows.erase(w);
				return;
			}
			else
			{
				w++;
			}
		}
		logger->Warn("Tried to remove a non-existant window: %lx", (long int) window.get());
	}

	void UIBinding::_GetOpenWindows(const ValueList& args, SharedValue result)
	{
		SharedKList list = new StaticBoundList();
		std::vector<AutoUserWindow>::iterator w = openWindows.begin();
		while (w != openWindows.end()) {
			list->Append(Value::NewObject(*w++));
		}
		result->SetList(list);
	}

	void UIBinding::_GetMainWindow(const ValueList& args, SharedValue result)
	{
		result->SetObject(this->mainWindow);
	}

	void UIBinding::_CreateMenu(const ValueList& args, SharedValue result)
	{
		result->SetObject(__CreateMenu(args));
	}

	AutoMenu UIBinding::__CreateMenu(const ValueList& args)
	{
		return this->CreateMenu();
	}

	void UIBinding::_CreateMenuItem(const ValueList& args, SharedValue result)
	{
		result->SetObject(__CreateMenuItem(args));
	}

	AutoMenuItem UIBinding::__CreateMenuItem(const ValueList& args)
	{
		args.VerifyException("createMenuItem", "?s m|0 s|0");
		std::string label = args.GetString(0, "");
		SharedKMethod eventListener = args.GetMethod(1, NULL);
		std::string iconURL = args.GetString(2, "");

		AutoMenuItem item = this->CreateMenuItem();
		if (!label.empty())
			item->SetLabel(label);
		if (!iconURL.empty())
			item->SetIcon(iconURL);
		if (!eventListener.isNull())
			item->AddEventListener(eventListener);

		return item;
	}


	void UIBinding::_CreateCheckMenuItem(const ValueList& args, SharedValue result)
	{
		result->SetObject(__CreateCheckMenuItem(args));
	}

	AutoMenuItem UIBinding::__CreateCheckMenuItem(const ValueList& args)
	{
		args.VerifyException("createCheckMenuItem", "?s m|0");
		std::string label = args.GetString(0, "");
		SharedKMethod eventListener = args.GetMethod(1, NULL);

		AutoMenuItem item = this->CreateCheckMenuItem();
		if (!label.empty())
			item->SetLabel(label);
		if (!eventListener.isNull())
			item->AddEventListener(eventListener);
		return item;
	}

	void UIBinding::_CreateSeparatorMenuItem(const ValueList& args, SharedValue result)
	{
		result->SetObject(__CreateSeparatorMenuItem(args));
	}

	AutoMenuItem UIBinding::__CreateSeparatorMenuItem(const ValueList& args)
	{
		return this->CreateSeparatorMenuItem();
	}

	void UIBinding::_SetMenu(const ValueList& args, SharedValue result)
	{
		args.VerifyException("setMenu", "o|0");
		SharedKObject argObj = args.GetObject(0, NULL);
		AutoMenu menu = NULL;

		if (!argObj.isNull())
		{
			menu = argObj.cast<Menu>();
		}

		this->SetMenu(menu); // platform-specific impl

		// Notify all windows that the app menu has changed.
		std::vector<AutoUserWindow>::iterator i = openWindows.begin();
		while (i != openWindows.end()) {
			(*i++)->AppMenuChanged();
		}
	}

	void UIBinding::_GetMenu(const ValueList& args, SharedValue result)
	{
		AutoMenu menu = this->GetMenu();
		if (menu.isNull())
		{
			result->SetNull();
		}
		else
		{
			result->SetObject(menu);
		}
	}

	void UIBinding::_SetContextMenu(const ValueList& args, SharedValue result)
	{
		args.VerifyException("setContextMenu", "o|0");
		SharedKObject argObj = args.GetObject(0, NULL);
		AutoMenu menu = NULL;

		if (!argObj.isNull())
		{
			menu = argObj.cast<Menu>();
		}
		this->SetContextMenu(menu);
	}

	void UIBinding::_GetContextMenu(const ValueList& args, SharedValue result)
	{
		AutoMenu menu = this->GetContextMenu();
		result->SetObject(menu);
	}

	void UIBinding::_SetIcon(const ValueList& args, SharedValue result)
	{
		args.VerifyException("setIcon", "s|0");
		std::string iconPath = this->iconURL = "";
		if (args.size() > 0) {
			this->iconURL = args.GetString(0);
			iconPath = URLToPathOrURL(this->iconURL);
		}

		this->SetIcon(iconPath); // platform-specific impl

		// Notify all windows that the app menu has changed.
		std::vector<AutoUserWindow>::iterator i = openWindows.begin();
		while (i != openWindows.end()) {
			(*i++)->AppIconChanged();
		}
	}

	void UIBinding::_AddTray(const ValueList& args, SharedValue result)
	{
		args.VerifyException("createTrayIcon", "s,?m");

		std::string iconPath = args.GetString(0);
		iconPath = URLToPathOrURL(iconPath);

		SharedKMethod cb = args.GetMethod(1, NULL);
		AutoTrayItem item = this->AddTray(iconPath, cb);
		this->trayItems.push_back(item);
		result->SetObject(item);
	}

	void UIBinding::_ClearTray(const ValueList& args, SharedValue result)
	{
		this->ClearTray();
	}

	void UIBinding::ClearTray()
	{
		std::vector<AutoTrayItem>::iterator i = this->trayItems.begin();
		while (i != this->trayItems.end())
		{
			(*i++)->Remove();
		}
		this->trayItems.clear();
	}

	void UIBinding::UnregisterTrayItem(TrayItem* item)
	{
		std::vector<AutoTrayItem>::iterator i = this->trayItems.begin();
		while (i != this->trayItems.end())
		{
			AutoTrayItem c = *i;
			if (c.get() == item)
			{
				i = this->trayItems.erase(i);
			}
			else
			{
				i++;
			}
		}
	}

	void UIBinding::_SetDockIcon(const ValueList& args, SharedValue result)
	{
		std::string iconPath;
		if (args.size() > 0) {
			std::string in = args.GetString(0);
			iconPath = URLToPathOrURL(in);
		}
		this->SetDockIcon(iconPath);
	}

	void UIBinding::_SetDockMenu(const ValueList& args, SharedValue result)
	{
		AutoPtr<Menu> menu = NULL; // A NULL value is an unset
		if (args.size() > 0 && args.at(0)->IsObject())
		{
			menu = args.at(0)->ToObject().cast<Menu>();
		}
		this->SetDockMenu(menu);
	}

	void UIBinding::_SetBadge(const ValueList& args, SharedValue result)
	{
		std::string badgeText;
		if (args.size() > 0) {
			badgeText = args.GetString(0);
		}

		this->SetBadge(badgeText);
	}

	void UIBinding::_SetBadgeImage(const ValueList& args, SharedValue result)
	{
		std::string iconPath;
		if (args.size() > 0) {
			std::string in = args.GetString(0);
			iconPath = URLToPathOrURL(in);
		}

		this->SetBadgeImage(iconPath);
	}

	void UIBinding::_GetIdleTime(
		const ValueList& args,
		SharedValue result)
	{
		result->SetDouble(this->GetIdleTime());
	}

	/*static*/
	void UIBinding::SendEventToListeners(
		std::vector<SharedKMethod> eventListeners,
		std::string eventType,
		SharedKObject eventSource,
		SharedKObject event)
	{
		event->SetObject("source", eventSource);
		event->SetString("type", eventType);

		std::vector<SharedKMethod>::iterator i = eventListeners.begin();
		while (i != eventListeners.end())
		{
			UIBinding::SendEventToListener((*i++), event);
		}
	}

	/*static*/
	void UIBinding::SendEventToListener(
		SharedKMethod listener, SharedKObject event)
	{
		if (listener.isNull())
			return;

		try {
			listener->Call(ValueList(Value::NewObject(event)));

		} catch (ValueException& e) {
			Logger* logger = Logger::Get("UI.UIBinding");
			SharedString exceptionSS = e.DisplayString();
			SharedString eventTypeSS = event->Get("type")->DisplayString();
			logger->Error(
				"Event listener for %s event failed with exception: %s",
				eventTypeSS->c_str(),
				exceptionSS->c_str());
		}
	}

}

