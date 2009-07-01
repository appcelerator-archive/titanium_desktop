/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#include "ui_module.h"
#include <string>

#define GET_ARG_OR_RETURN(INDEX, TYPE, VAR) \
	if ((int) args.size() < INDEX - 1 || !args.at(INDEX)->Is##TYPE()) \
		return; \
	VAR = args.at(INDEX)->To##TYPE();

#define GET_ARG(INDEX, TYPE, VAR) \
	if ((int) args.size() > INDEX && args.at(INDEX)->Is##TYPE()) \
		VAR = args.at(INDEX)->To##TYPE();


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

	UIBinding::UIBinding(Host *host) : host(host)
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
		this->SetMethod("createMenuItem", &UIBinding::_CreateMenuItem);

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
		 * @tiapi(method=True,name=UI.createTrayItem,version=1.0)
		 * @tiapi Create and add a tray icon
		 * @tiarg[String, iconURL] URL to the icon to use for this tray item
		 * @tiarg[Function, eventListener, optional=True] Event listener to add for this item
		 */
		this->SetMethod("createTrayItem", &UIBinding::_AddTray);

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
		 * @tiresult(for=UI.getOpenWindows,type=list) the list of open windows
		 */
		this->openWindowList = new StaticBoundList();
		this->SetMethod("getOpenWindows", &UIBinding::_GetOpenWindows);

		/**
		 * @tiapi(property=True,name=UI.windows,version=0.2,deprecated=True)
		 * @tiapi  Returns a list of open user created windows
		 * @tideprecated(for=UI.windows,version=0.4)
		 */
		this->Set("windows", Value::NewList(this->openWindowList));

		SharedKObject global = host->GetGlobalObject();
		SharedValue ui_binding_val = Value::NewObject(this);
		global->Set("UI", ui_binding_val);
	}

	void UIBinding::CreateMainWindow(WindowConfig* config)
	{
		SharedPtr<UserWindow> noParent = NULL;
		SharedUserWindow main_window = this->CreateWindow(config, noParent);

		SharedKObject global = host->GetGlobalObject();
		/**
		 * @tiapi(property=True,name=UI.mainWindow) Returns the main window 
		 */
		global->SetNS("UI.mainWindow", Value::NewObject(main_window));

		main_window->Open();
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

	std::vector<SharedUserWindow>& UIBinding::GetOpenWindows()
	{
		return this->openWindows;
	}

	void UIBinding::AddToOpenWindows(SharedUserWindow window)
	{
		this->openWindowList->Append(Value::NewObject(window));
		this->openWindows.push_back(window);
	}

	void UIBinding::RemoveFromOpenWindows(SharedUserWindow window)
	{
		static Logger* logger = Logger::Get("UI");
		std::vector<SharedUserWindow>::iterator w = openWindows.begin();
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
		logger->Warn("Tried to remove a non-existant window: 0x%lx", (long int) window.get());
	}

	void UIBinding::_GetOpenWindows(const ValueList& args, SharedValue result)
	{
		result->SetList(this->openWindowList);
	}

	void UIBinding::_CreateMenu(const ValueList& args, SharedValue result)
	{
		SharedMenu menu = this->CreateMenu();
		result->SetObject(menu);
	}

	void UIBinding::_CreateMenuItem(const ValueList& args, SharedValue result)
	{
		args.VerifyException("createMenuItem", "?s m|0 s|0");
		std::string label = args.GetString(0, "");
		SharedKMethod eventListener = args.GetString(1, NULL);
		std::string iconURL = args.GetString(1, "");

		SharedMenuItem item = this->CreateMenuItem(label, callback, iconURL);
		result->SetObject(item);
	}

	void UIBinding::_CreateCheckMenuItem(const ValueList& args, SharedValue result)
	{
		args.VerifyException("createCheckMenuItem", "?s m|0");
		std::string label = args.GetString(0, "");
		SharedKMethod eventListener = args.GetString(1, NULL);

		SharedMenuItem item = this->CreateCheckMenuItem(label, callback);
		result->SetObject(item);
	}

	void UIBinding::_CreateSeparatorItem(const ValueList& args, SharedValue result)
	{
		SharedMenuItem item = this->CreateSeparatorMenuItem();
		result->SetObject(item);
	}

	void UIBinding::_SetMenu(const ValueList& args, SharedValue result)
	{
		args.VerifyException("setMenu", "o|0");
		SharedKObject argObj = args.GetObject(0, NULL);
		SharedMenu menu = NULL;

		if (!argObj.isNull())
		{
			menu = argObj.cast<MenuItem>();
		}
		this->SetMenu(menu);
	}

	void UIBinding::_GetMenu(const ValueList& args, SharedValue result)
	{
		SharedMenu menu = this->GetMenu();
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
		SharedMenu menu = NULL;

		if (!argObj.isNull())
		{
			menu = argObj.cast<MenuItem>();
		}
		this->SetContextMenu(menu);
	}

	void UIBinding::_GetContextMenu(const ValueList& args, SharedValue result)
	{
		SharedMenu menu = this->GetContextMenu();
		if (menu.get() != NULL)
		{
			SharedKList list = menu;
			result->SetList(list);
		}
		else
		{
			result->SetUndefined();
		}
	}

	void UIBinding::_SetIcon(const ValueList& args, SharedValue result)
	{
		SharedString iconPath = NULL; // a NULL value is an unset
		if (args.size() > 0 && args.at(0)->IsString())
		{
			const char *iconURL = args.at(0)->ToString();
			iconPath = UIModule::GetResourcePath(iconURL);
		}
		this->iconPath = iconPath;
		this->SetIcon(iconPath);
	}

	void UIBinding::_AddTray(const ValueList& args, SharedValue result)
	{
		args.VerifyException("createTrayIcon", "s,?m");
		std::string iconURL = args.GetString(0);
		SharedString iconPath = UIModule::GetResourcePath(icon_url);
		if (iconPath.isNull())
		{
			throw ValueException::FromString("Could not add tray icon with icon URL: " + iconURL);
		}

		SharedKMethod cb = args.GetMethod(1, NULL);
		SharedTrayItem item = this->AddTray(icon_path, cb);
		this->trayItems.push_back(item);

		result->SetObject(item);
	}

	void UIBinding::_ClearTray(const ValueList& args, SharedValue result)
	{
		this->ClearTray();
	}

	void UIBinding::ClearTray()
	{
		std::vector<SharedTrayItem>::iterator i = this->trayItems.begin();
		while (i != this->trayItems.end())
		{
			(*i++)->Remove();
		}
		this->trayItems.clear();
	}

	void UIBinding::UnregisterTrayItem(TrayItem* item)
	{
		std::vector<SharedTrayItem>::iterator i = this->trayItems.begin();
		while (i != this->trayItems.end())
		{
			SharedTrayItem c = *i;
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
		SharedString icon_path = NULL; // a NULL value is an unset
		if (args.size() > 0 && args.at(0)->IsString())
		{
			const char *icon_url = args.at(0)->ToString();
			icon_path = UIModule::GetResourcePath(icon_url);
		}
		this->SetDockIcon(icon_path);
	}

	void UIBinding::_SetDockMenu(const ValueList& args, SharedValue result)
	{
		SharedPtr<MenuItem> menu = NULL; // A NULL value is an unset
		if (args.size() > 0 && args.at(0)->IsList())
		{
			SharedKList list = args.at(0)->ToList();
			menu = list.cast<MenuItem>();
		}
		this->SetDockMenu(menu);
	}

	void UIBinding::_SetBadge(const ValueList& args, SharedValue result)
	{
		// badges are just labels right now
		// we might want to support custom images too
		SharedString badge_path = NULL; // a NULL value is an unset
		if (args.size() > 0 && args.at(0)->IsString())
		{
			const char *badge_url = args.at(0)->ToString();
			if (badge_url!=NULL)
			{
				badge_path = SharedString(new std::string(badge_url));
			}
		}
		this->SetBadge(badge_path);
	}

	void UIBinding::_SetBadgeImage(const ValueList& args, SharedValue result)
	{
		SharedString image_path = NULL; // a NULL value is an unset
		if (args.size() > 0 && args.at(0)->IsString())
		{
			const char *image_url = args.at(0)->ToString();
			if (image_url!=NULL)
			{
				image_path = UIModule::GetResourcePath(image_url);
			}
		}

		this->SetBadgeImage(image_path);
	}

	void UIBinding::_GetIdleTime(
		const ValueList& args,
		SharedValue result)
	{
		result->SetDouble(this->GetIdleTime());
	}

}

