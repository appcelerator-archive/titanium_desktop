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
#ifdef OS_WIN32
		this->uiBinding = new Win32UIBinding(this, host);
#elif OS_OSX
		this->uiBinding = new OSXUIBinding(host);
#elif OS_LINUX
		this->uiBinding = new GtkUIBinding(host);
#endif
		host->GetGlobalObject()->SetObject("UI", this->uiBinding);
		host->GetGlobalObject()->SetObject("Notification", this->uiBinding);

		ScriptEvaluator::Initialize();
		AppConfig* config = AppConfig::Instance();
		if (!config)
		{
			std::string msg = "Error loading tiapp.xml. Your application "
				"is not properly configured or packaged.";
			this->uiBinding->ErrorDialog(msg);
			throw ValueException::FromString(msg.c_str());
			return;
		}

		// If there is no main window configuration, this just
		// AppConfig::GetMainWindow returns a default configuration.
		this->uiBinding->CreateMainWindow(config->GetMainWindow());

		try
		{
			std::string& appIcon = host->GetApplication()->image;
			if (!appIcon.empty())
				this->uiBinding->_SetIcon(appIcon);
		}
		catch (ValueException& e)
		{
			SharedString ss = e.DisplayString();
			Logger::Get("UI")->Error("Could not set default icon: %s", ss->c_str());
		}
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
