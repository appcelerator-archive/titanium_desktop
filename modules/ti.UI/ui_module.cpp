/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */

#include "ui_module.h"
#include <Poco/URI.h>

namespace ti
{
	KROLL_MODULE(UIModule, STRING(MODULE_NAME), STRING(MODULE_VERSION))


	SharedKObject UIModule::global = SharedKObject(NULL);
	SharedPtr<MenuItem> UIModule::app_menu = SharedPtr<MenuItem>(NULL);
	SharedPtr<MenuItem> UIModule::app_context_menu = SharedPtr<MenuItem>(NULL);
	SharedString UIModule::icon_path = SharedString(NULL);
	std::vector<SharedPtr<TrayItem> > UIModule::tray_items;
	UIModule* UIModule::instance_;

	void UIModule::Initialize()
	{
		// We are keeping this object in a static variable, which means
		// that we should only ever have one copy of the UI module.
		SharedKObject global = this->host->GetGlobalObject();
		UIModule::global = global;
		UIModule::instance_ = this;
	}

	void UIModule::Start()
	{
		SharedKMethod api = this->host->GetGlobalObject()->GetNS("API.fire")->ToMethod();
		api->Call("ti.UI.start", Value::Undefined);

#ifdef OS_WIN32
		this->uiBinding = new Win32UIBinding(this, host);
#elif OS_OSX
		this->uiBinding = new OSXUIBinding(host);
#elif OS_LINUX
		this->uiBinding = new GtkUIBinding(host);
#endif

		AppConfig *config = AppConfig::Instance();
		if (config == NULL)
		{
			std::string msg = "Error loading tiapp.xml. Your application "
			                  "is not properly configured or packaged.";
			this->uiBinding->ErrorDialog(msg);
			throw ValueException::FromString(msg.c_str());
			return;
		}
		WindowConfig *main_window_config = config->GetMainWindow();
		if (main_window_config == NULL)
		{
			std::string msg ="Error loading tiapp.xml. Your application "
			                 "window is not properly configured or packaged.";
			this->uiBinding->ErrorDialog(msg);
			throw ValueException::FromString(msg.c_str());
			return;
		}

		this->uiBinding->CreateMainWindow(main_window_config);
	}

	void UIModule::Exiting(int exitcode)
	{
		// send a stop notification - we need to do this before 
		// stop is called given that the API module is registered (and unregistered)
		// before our module and it will then be too late
		SharedKMethod api = this->host->GetGlobalObject()->GetNS("API.fire")->ToMethod();
		api->Call("ti.UI.stop", Value::Undefined);
	}

	void UIModule::Stop()
	{
		// Remove app tray icons
		UIModule::ClearTrayItems();

		// Only one copy of the UI module loaded hopefully,
		// otherwise we need to count instances and free
		// this variable when the last instance disappears
		UIModule::global = SharedKObject(NULL);
	}

	bool UIModule::IsResourceLocalFile(std::string string)
	{
		Poco::URI uri(string.c_str());
		std::string scheme = uri.getScheme();
		return (scheme == "app" || scheme == "ti" || scheme == "file");
	}

	SharedString UIModule::GetResourcePath(const char *URL)
	{
		if (URL == NULL || !strcmp(URL, ""))
			return new std::string("");

		Poco::URI uri(URL);
		std::string scheme = uri.getScheme();

		if (scheme == "app" || scheme == "ti")
		{
			SharedValue new_url = global->CallNS(
				"App.appURLToPath",
				Value::NewString(URL));

			if (new_url->IsString())
				return new std::string(new_url->ToString());
		}

		return new std::string(URL);
	}

	void UIModule::SetMenu(SharedPtr<MenuItem> menu)
	{
		UIModule::app_menu = menu;
	}

	SharedPtr<MenuItem> UIModule::GetMenu()
	{
		return UIModule::app_menu;
	}

	void UIModule::SetContextMenu(SharedPtr<MenuItem> menu)
	{
		UIModule::app_context_menu = menu;
	}

	SharedPtr<MenuItem> UIModule::GetContextMenu()
	{
		return UIModule::app_context_menu;
	}

	void UIModule::SetIcon(SharedString icon_path)
	{
		UIModule::icon_path = icon_path;
	}

	SharedString UIModule::GetIcon()
	{
		return UIModule::icon_path;
	}

	void UIModule::AddTrayItem(SharedPtr<TrayItem> item)
	{
		// One tray item at a time
		UIModule::ClearTrayItems();
		UIModule::tray_items.push_back(item);
	}

	void UIModule::ClearTrayItems()
	{
		std::vector<SharedPtr<TrayItem> >::iterator i = UIModule::tray_items.begin();
		while (i != UIModule::tray_items.end())
		{
			(*i++)->Remove();
		}
		UIModule::tray_items.clear();
	}

	void UIModule::UnregisterTrayItem(TrayItem* item)
	{
		std::vector<SharedPtr<TrayItem> >::iterator i = UIModule::tray_items.begin();
		while (i != UIModule::tray_items.end())
		{
			SharedPtr<TrayItem> c = *i;
			if (c.get() == item)
			{
				i = UIModule::tray_items.erase(i);
			}
			else
			{
				i++;
			}
		}
	}
}
