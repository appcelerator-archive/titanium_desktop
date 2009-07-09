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
	UIModule* UIModule::instance_ = 0;

	void UIModule::Initialize()
	{
		// We are keeping this object in a static variable, which means
		// that we should only ever have one copy of the UI module.
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
		uiBinding->ClearTray();
	}

	bool UIModule::IsResourceLocalFile(std::string string)
	{
		Poco::URI uri(string.c_str());
		std::string scheme = uri.getScheme();
		return (scheme == "app" || scheme == "ti" || scheme == "file");
	}
}
