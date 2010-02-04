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

	UIBinding::UIBinding(Host* host) :
		KAccessorObject("UI"),
		host(host)
	{
		instance = this;

		// @tiproperty[Number, UI.CENTERED, since=0.6] The CENTERED event constant
		this->Set("CENTERED", Value::NewInt(DEFAULT_POSITION));

		/**
		 * @tiapi(method=True,name=UI.createMenu,since=0.6)
		 * @tiapi Create a new menu
		 * @tiresult[UI.Menu] A new menu
		 */
		this->SetMethod("createMenu", &UIBinding::_CreateMenu);

		/**
		 * @tiapi(method=True,name=UI.createMenuItem,since=0.6)
		 * @tiapi Create a new menu item.
		 * @tiarg[String, label] The label for this menu item
		 * @tiarg[Function, eventListener, optional=True] An event listener for this menu item
		 * @tiarg[String, iconURL, optional=True] A URL to an icon to use for this menu item
		 * @tiresult[UI.MenuItem] A new menu item
		 */
		this->SetMethod("createMenuItem", &UIBinding::_CreateMenuItem);

		/**
		 * @tiapi(method=True,name=UI.createCheckMenuItem,since=0.6)
		 * @tiapi Create a new CheckMenuItem object.
		 * @tiarg[String, label] The label for this menu item
		 * @tiarg[Function, eventListener, optional=True] An event listener for this menu item
		 * @tiresult[UI.CheckMenuItem] The new CheckMenuItem object
		 */
		this->SetMethod("createCheckMenuItem", &UIBinding::_CreateCheckMenuItem);

		/**
		 * @tiapi(method=True,name=UI.createSeperatorMenuItem,since=0.6)
		 * @tiapi Create a new separator menu item.
		 * @tiresult[UI.SeparatorMenuItem] A new separator menu item
		 */
		this->SetMethod("createSeparatorMenuItem", &UIBinding::_CreateSeparatorMenuItem);

		/**
		 * @tiapi(method=True,name=UI.setMenu,since=0.2) Set a menu for the application
		 * @tiarg[UI.Menu|null, menu] A Menu object to use as the menu or null to unset the menu
		 */
		this->SetMethod("setMenu", &UIBinding::_SetMenu);

		/**
		 * @tiapi(method=True,name=UI.getMenu,since=0.2) Returns the application's main MenuItem
		 * @tiresult[UI.Menu|null] The application's main menu
		 */
		this->SetMethod("getMenu", &UIBinding::_GetMenu);

		/**
		 * @tiapi(method=True,name=UI.setContextMenu,since=0.2) Set the application's context menu
		 * @tiarg(for=UI.setContextMenu,type=UI.Menu|null,name=menu) a MenuItem object or null to unset
		 */
		this->SetMethod("setContextMenu", &UIBinding::_SetContextMenu);

		/**
		 * @tiapi(method=True,name=UI.getContextMenu,since=0.2) Returns the application context menu
		 * @tiresult(for=UI.getContextMenu,type=UI.Menu|null) the application's context MenuItem object
		 */
		this->SetMethod("getContextMenu", &UIBinding::_GetContextMenu);

		/**
		 * @tiapi(method=True,name=UI.setIcon,since=0.2) Set the application's icon
		 * @tiarg(for=UI.setIcon,type=String,name=menu) path to the icon
		 */
		this->SetMethod("setIcon", &UIBinding::_SetIcon);

		/**
		 * @tiapi(method=True,name=UI.addTray,since=0.2)
		 * @tiapi Create and add a tray icon
		 * @tiarg[String, iconURL] URL to the icon to use for this tray item
		 * @tiarg[Function, eventListener, optional=True] Event listener to add for this item
		 * @tiresult(for=UI.addTray,type=UI.Tray|null) the application's Tray icon object
		 */
		this->SetMethod("addTray", &UIBinding::_AddTray);

		/**
		 * @tiapi(method=True,name=UI.clearTray,since=0.2)
		 * @tiapi Empty the tray of all this application's tray items
		 */
		this->SetMethod("clearTray", &UIBinding::_ClearTray);

		/**
		 * @tiapi(method=True,name=UI.setDockIcon,since=0.2) Set the dock icon
		 * @tiarg(for=UI.setDockIcon,type=String,name=icon) path to the icon
		 */
		this->SetMethod("setDockIcon", &UIBinding::_SetDockIcon);

		/**
		 * @tiapi(method=True,name=UI.setDockMenu,since=0.2) Set the dock menu
		 * @tiarg(for=UI.setDockMenu,type=UI.Menu,name=menu) The new menu for the dock
		 */
		this->SetMethod("setDockMenu", &UIBinding::_SetDockMenu);

		/**
		 * @tiapi(method=True,name=UI.setBadge,since=0.2,platforms=osx)
		 * @tiapi Set the application icon's badge text.
		 * @tiarg[String, text] The new badge text.
		 */
		this->SetMethod("setBadge", &UIBinding::_SetBadge);

		/**
		 * @tiapi(method=True,name=UI.setBadgeImage,since=0.2,platforms=osx)
		 * @tiapi Set the application icon's badge image.
		 * @tiarg[String, imageURL] URL to the new badge image.
		 */
		this->SetMethod("setBadgeImage", &UIBinding::_SetBadgeImage);

		/**
		 * @tiapi(method=True,name=UI.getIdleTime,since=0.2)
		 * @tiapi Returns the user's idle time (for the desktop, not just the application)
		 * @tiresult(for=UI.getIdleTime,type=Number) Number of milliseconds of idle time.
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
		 * @tiapi(method=True,name=UI.getMainWindow,since=0.6)
		 * @tiapi Return the application's main window
		 * @tiresult[UI.UserWindow] The main window for this application
		 */
		this->SetMethod("getMainWindow", &UIBinding::_GetMainWindow);

		/**
		 * @tiapi(method=True,name=UI.createWindow,since=0.8.1)
		 * @tiapi Create a new top-level window or a child of the current window if called
		 * @tiapi from the context of an existing window.
		 * @tiarg[type=String|Object,options,optional=True]
		 * @tiarg A string containing a url of the new window or an object
		 * @tiarg containing properties for the new window
		 * @tiresult[UI.UserWindow] The new UserWindow object.
		 */
		this->SetMethod("createWindow", &UIBinding::_CreateWindow);

		this->SetObject("Clipboard", new Clipboard());
		Logger::AddLoggerCallback(&UIBinding::Log);
	}

	void UIBinding::CreateMainWindow(AutoPtr<WindowConfig> config)
	{
		this->mainWindow = UserWindow::CreateWindow(config, 0);
		this->mainWindow->Open();
	}

	void UIBinding::_CreateWindow(const ValueList& args, KValueRef result)
	{
		AutoPtr<WindowConfig> config(0);
		if (args.size() > 0 && args.at(0)->IsObject())
		{
			config = WindowConfig::FromProperties(args.GetObject(0));
		}
		else if (args.size() > 0 && args.at(0)->IsString())
		{
			std::string url(args.GetString(0));
			config = AppConfig::Instance()->GetWindowByURL(url);
			if (config.isNull())
			{
				config = WindowConfig::Default();
				config->SetURL(url);
			}
		}

		// If we still do not have a configuration, just use the default.
		if (config.isNull())
			config = WindowConfig::Default();

		result->SetObject(UserWindow::CreateWindow(config, 0));
	}

	void UIBinding::ErrorDialog(std::string msg)
	{
		std::cerr << msg << std::endl;
	}

	UIBinding::~UIBinding()
	{
		this->ClearTray();
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

	void UIBinding::_GetOpenWindows(const ValueList& args, KValueRef result)
	{
		KListRef list = new StaticBoundList();
		std::vector<AutoUserWindow>::iterator w = openWindows.begin();
		while (w != openWindows.end()) {
			list->Append(Value::NewObject(*w++));
		}
		result->SetList(list);
	}

	void UIBinding::_GetMainWindow(const ValueList& args, KValueRef result)
	{
		result->SetObject(this->mainWindow);
	}

	void UIBinding::_CreateMenu(const ValueList& args, KValueRef result)
	{
		result->SetObject(__CreateMenu(args));
	}

	AutoMenu UIBinding::__CreateMenu(const ValueList& args)
	{
		// call into the native code to retrieve the menu
		return this->CreateMenu();
	}

	void UIBinding::_CreateMenuItem(const ValueList& args, KValueRef result)
	{
		result->SetObject(__CreateMenuItem(args));
	}

	AutoMenuItem UIBinding::__CreateMenuItem(const ValueList& args)
	{
		args.VerifyException("createMenuItem", "?s m|0 s|0");
		std::string label = args.GetString(0, "");
		KMethodRef eventListener = args.GetMethod(1, NULL);
		std::string iconURL = args.GetString(2, "");

		AutoMenuItem item = this->CreateMenuItem();
		if (!label.empty())
			item->SetLabel(label);
		if (!iconURL.empty())
			item->SetIcon(iconURL);
		if (!eventListener.isNull())
			item->AddEventListener(Event::CLICKED, eventListener);

		return item;
	}


	void UIBinding::_CreateCheckMenuItem(const ValueList& args, KValueRef result)
	{
		result->SetObject(__CreateCheckMenuItem(args));
	}

	AutoMenuItem UIBinding::__CreateCheckMenuItem(const ValueList& args)
	{
		args.VerifyException("createCheckMenuItem", "?s m|0");
		std::string label = args.GetString(0, "");
		KMethodRef eventListener = args.GetMethod(1, NULL);

		AutoMenuItem item = this->CreateCheckMenuItem();
		if (!label.empty())
			item->SetLabel(label);
		if (!eventListener.isNull())
			item->AddEventListener(Event::CLICKED, eventListener);

		return item;
	}

	void UIBinding::_CreateSeparatorMenuItem(const ValueList& args, KValueRef result)
	{
		result->SetObject(__CreateSeparatorMenuItem(args));
	}

	AutoMenuItem UIBinding::__CreateSeparatorMenuItem(const ValueList& args)
	{
		return this->CreateSeparatorMenuItem();
	}

	void UIBinding::_SetMenu(const ValueList& args, KValueRef result)
	{
		args.VerifyException("setMenu", "o|0");
		KObjectRef argObj = args.GetObject(0, NULL);
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

	void UIBinding::_GetMenu(const ValueList& args, KValueRef result)
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

	void UIBinding::_SetContextMenu(const ValueList& args, KValueRef result)
	{
		args.VerifyException("setContextMenu", "o|0");
		KObjectRef argObj = args.GetObject(0, NULL);
		AutoMenu menu = NULL;

		if (!argObj.isNull())
		{
			menu = argObj.cast<Menu>();
		}
		this->SetContextMenu(menu);
	}

	void UIBinding::_GetContextMenu(const ValueList& args, KValueRef result)
	{
		AutoMenu menu = this->GetContextMenu();
		result->SetObject(menu);
	}

	void UIBinding::_SetIcon(const ValueList& args, KValueRef result)
	{
		args.VerifyException("setIcon", "s|0");

		std::string iconURL;
		if (args.size() > 0)
			iconURL = args.GetString(0);
		this->_SetIcon(iconURL);
	}

	void UIBinding::_SetIcon(std::string iconURL)
	{
		std::string iconPath;
		this->iconURL = iconURL;
		if (!iconURL.empty())
			iconPath = URLUtils::URLToPath(this->iconURL);

		this->SetIcon(iconPath); // platform-specific impl

		// Notify all windows that the app menu has changed.
		std::vector<AutoUserWindow>::iterator i = openWindows.begin();
		while (i != openWindows.end()) {
			(*i++)->AppIconChanged();
		}
	}

	void UIBinding::_AddTray(const ValueList& args, KValueRef result)
	{
		args.VerifyException("createTrayIcon", "s,?m");
		std::string iconURL = args.GetString(0);

		KMethodRef cbSingleClick = args.GetMethod(1, NULL);
		AutoTrayItem item = this->AddTray(iconURL, cbSingleClick);
		this->trayItems.push_back(item);
		result->SetObject(item);
	}

	void UIBinding::_ClearTray(const ValueList& args, KValueRef result)
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

	void UIBinding::_SetDockIcon(const ValueList& args, KValueRef result)
	{
		std::string iconPath;
		if (args.size() > 0) {
			std::string in = args.GetString(0);
			iconPath = URLUtils::URLToPath(in);
		}
		this->SetDockIcon(iconPath);
	}

	void UIBinding::_SetDockMenu(const ValueList& args, KValueRef result)
	{
		AutoPtr<Menu> menu = NULL; // A NULL value is an unset
		if (args.size() > 0 && args.at(0)->IsObject())
		{
			menu = args.at(0)->ToObject().cast<Menu>();
		}
		this->SetDockMenu(menu);
	}

	void UIBinding::_SetBadge(const ValueList& args, KValueRef result)
	{
		std::string badgeText;
		if (args.size() > 0) {
			badgeText = args.GetString(0);
		}

		this->SetBadge(badgeText);
	}

	void UIBinding::_SetBadgeImage(const ValueList& args, KValueRef result)
	{
		std::string iconPath;
		if (args.size() > 0) {
			std::string in = args.GetString(0);
			iconPath = URLUtils::URLToPath(in);
		}

		this->SetBadgeImage(iconPath);
	}

	void UIBinding::_GetIdleTime(
		const ValueList& args,
		KValueRef result)
	{
		result->SetDouble(this->GetIdleTime());
	}

	void UIBinding::Log(Logger::Level level, std::string& message)
	{
		if (level > Logger::LWARN)
			return;

		std::string methodName("warn");
		if (level < Logger::LWARN)
			methodName = "error";

		std::string origMethodName(methodName);
		origMethodName.append("_orig");

		std::vector<AutoUserWindow>& openWindows = UIBinding::GetInstance()->GetOpenWindows();
		for (size_t i = 0; i < openWindows.size(); i++)
		{
			KObjectRef domWindow = openWindows[i]->GetDOMWindow();
			if (domWindow.isNull())
				continue;

			KObjectRef console = domWindow->GetObject("console", 0);
			if (console.isNull())
				continue;

			KMethodRef method = console->GetMethod(origMethodName.c_str(), 0);
			if (method.isNull())
				method = console->GetMethod(methodName.c_str(), 0);

			RunOnMainThread(method, ValueList(Value::NewString(message)), false);
		}
	}
}

