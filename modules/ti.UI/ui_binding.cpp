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

	UIBinding::UIBinding(Host *host) : host(host)
	{
		instance = this;

		/**
		 * @tiapi(method=True,name=UI.createMenu,version=0.2) Creates a MenuItem object
		 * @tiresult(for=UI.createMenu,type=object) a MenuItem object
		 */
		this->SetMethod("createMenu", &UIBinding::_CreateMenu);
		/**
		 * @tiapi(method=True,name=UI.createTrayMenu,version=0.2) Creates a TrayItem object
		 * @tiresult(for=UI.createTrayMenu,type=object) a TrayItem object
		 */
		this->SetMethod("createTrayMenu", &UIBinding::_CreateTrayMenu);
		/**
		 * @tiapi(method=True,name=UI.setMenu,version=0.2) Sets a menu for the application
		 * @tiarg(for=UI.setMenu,type=object,name=menu) a MenuItem object or null to unset
		 */
		this->SetMethod("setMenu", &UIBinding::_SetMenu);
		/**
		 * @tiapi(method=True,name=UI.getMenu,version=0.2) Returns the application's main MenuItem
		 * @tiresult(for=UI.getMenu,type=object) the application's main MenuItem
		 */
		this->SetMethod("getMenu", &UIBinding::_GetMenu);
		/**
		 * @tiapi(method=True,name=UI.setContextMenu,version=0.2) Sets the application's context menu
		 * @tiarg(for=UI.setContextMenu,type=object,name=menu) a MenuItem object or null to unset
		 */
		this->SetMethod("setContextMenu", &UIBinding::_SetContextMenu);
		/**
		 * @tiapi(method=True,name=UI.getContextMenu,version=0.2) Returns the application context menu
		 * @tiresult(for=UI.getContextMenu,type=object) the application's context MenuItem object
		 */
		this->SetMethod("getContextMenu", &UIBinding::_GetContextMenu);
		/**
		 * @tiapi(method=True,name=UI.setIcon,version=0.2) Sets the application's icon
		 * @tiarg(for=UI.setIcon,type=object,name=menu) path to the icon
		 */
		this->SetMethod("setIcon", &UIBinding::_SetIcon);
		/**
		 * @tiapi(method=True,name=UI.addTray,version=0.2) Adds a tray menu
		 * @tiarg(for=UI.addTray,type=object,name=menu) a TrayItem to add
		 */
		this->SetMethod("addTray", &UIBinding::_AddTray);
		/**
		 * @tiapi(method=True,name=UI.clearTray,version=0.2) Removes a tray menu
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
		 * @tiapi(method=True,name=UI.getIdleTime,version=0.2) Returns the user's idle time (for the machine, not just the application)
		 * @tiresult(for=UI.getIdleTime,type=double) the idle time as a double
		 */
		this->SetMethod("getIdleTime", &UIBinding::_GetIdleTime);

		this->openWindowList = new StaticBoundList();
		/**
		 * @tiapi(method=True,name=UI.getOpenWindows,version=0.4) Returns the list of currently open windows
		 * @tiresult(for=UI.getOpenWindows,type=list) the list of open windows
		 */
		this->SetMethod("getOpenWindows", &UIBinding::_GetOpenWindows);

		/**
		 * @tiapi(property=True,name=UI.windows,version=0.2) Returns a list of open user created windows
		 * @tideprecated(for=UI.windows,version=0.4)
		 */
		this->Set("windows", Value::NewList(this->openWindowList));

		SharedKObject global = host->GetGlobalObject();
		SharedValue ui_binding_val = Value::NewObject(this);
		global->Set("UI", ui_binding_val);

	}

	void UIBinding::CreateMainWindow(WindowConfig* config)
	{
		SharedPtr<UserWindow> no_parent = NULL;
		SharedUserWindow main_window = this->CreateWindow(config, no_parent);

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
		SharedPtr<MenuItem> menu = this->CreateMenu(false);
		result->SetList(menu);
	}

	void UIBinding::_CreateTrayMenu(const ValueList& args, SharedValue result)
	{
		SharedPtr<MenuItem> menu = this->CreateMenu(true);
		result->SetList(menu);
	}

	void UIBinding::_SetMenu(const ValueList& args, SharedValue result)
	{
		SharedPtr<MenuItem> menu = NULL; // A NULL value is an unset
		if (args.size() > 0 && args.at(0)->IsList())
		{
			menu = args.at(0)->ToList().cast<MenuItem>();
		}
		UIModule::SetMenu(menu);
		this->SetMenu(menu);
	}

	void UIBinding::_GetMenu(const ValueList& args, SharedValue result)
	{
		SharedPtr<MenuItem> menu = UIModule::GetMenu();
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

	void UIBinding::_SetContextMenu(const ValueList& args, SharedValue result)
	{
		SharedPtr<MenuItem> menu = NULL; // A NULL value is an unset
		if (args.size() > 0 && args.at(0)->IsList())
		{
			menu = args.at(0)->ToList().cast<MenuItem>();
		}
		UIModule::SetContextMenu(menu);
		this->SetContextMenu(menu);
	}

	void UIBinding::_GetContextMenu(const ValueList& args, SharedValue result)
	{
		SharedPtr<MenuItem> menu = UIModule::GetContextMenu();
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
		SharedString icon_path = NULL; // a NULL value is an unset
		if (args.size() > 0 && args.at(0)->IsString())
		{
			const char *icon_url = args.at(0)->ToString();
			icon_path = UIModule::GetResourcePath(icon_url);
		}
		UIModule::SetIcon(icon_path);
		this->SetIcon(icon_path);
	}

	void UIBinding::_AddTray(const ValueList& args, SharedValue result)
	{
		const char *icon_url;
		GET_ARG_OR_RETURN(0, String, icon_url);
		SharedString icon_path = UIModule::GetResourcePath(icon_url);
		if (icon_path.isNull())
			return;

		SharedKMethod cb = SharedKMethod(NULL);
		GET_ARG(1, Method, cb);

		SharedPtr<TrayItem> item = this->AddTray(icon_path, cb);

		UIModule::AddTrayItem(item);
		result->SetObject(item);
	}

	void UIBinding::_ClearTray(const ValueList& args, SharedValue result)
	{
		UIModule::ClearTrayItems();
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
			menu = args.at(0)->ToList().cast<MenuItem>();
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

