/*
 * Copyright (c) 2009-2010 Appcelerator, Inc. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "UI.h"

#include "Clipboard.h"
#include "Menu.h"
#include "MenuItem.h"
#include "Notification.h"
#include "TrayItem.h"
#include "UserWindow.h"
#include "../ti.App/ApplicationConfig.h"

namespace Titanium {

UI* UI::instance = NULL;

UI::UI()
	: KAccessorObject("UI")
{
	instance = this;

	this->Set("CENTERED", Value::NewInt(DEFAULT_POSITION));

	this->SetMethod("createNotification", &UI::_CreateNotification);
	this->SetMethod("createMenu", &UI::_CreateMenu);
	this->SetMethod("createMenuItem", &UI::_CreateMenuItem);
	this->SetMethod("createCheckMenuItem", &UI::_CreateCheckMenuItem);
	this->SetMethod("createSeparatorMenuItem", &UI::_CreateSeparatorMenuItem);
	this->SetMethod("setMenu", &UI::_SetMenu);
	this->SetMethod("getMenu", &UI::_GetMenu);
	this->SetMethod("setContextMenu", &UI::_SetContextMenu);
	this->SetMethod("getContextMenu", &UI::_GetContextMenu);
	this->SetMethod("setIcon", &UI::_SetIcon);
	this->SetMethod("addTray", &UI::_AddTray);
	this->SetMethod("clearTray", &UI::_ClearTray);
	this->SetMethod("setDockIcon", &UI::_SetDockIcon);
	this->SetMethod("setDockMenu", &UI::_SetDockMenu);
	this->SetMethod("setBadge", &UI::_SetBadge);
	this->SetMethod("setBadgeImage", &UI::_SetBadgeImage);
	this->SetMethod("getIdleTime", &UI::_GetIdleTime);
	this->SetMethod("getOpenWindows", &UI::_GetOpenWindows);
	this->SetMethod("getWindows", &UI::_GetOpenWindows);
	this->SetMethod("getMainWindow", &UI::_GetMainWindow);
	this->SetMethod("createWindow", &UI::_CreateWindow);

	// Initialize notifications
	this->SetBool("nativeNotifications", Notification::InitializeImpl());

	this->SetObject("Clipboard", new Clipboard());
	Logger::AddLoggerCallback(&UI::Log);
}

void UI::CreateMainWindow(AutoPtr<WindowConfig> config)
{
	this->mainWindow = UserWindow::CreateWindow(config, 0);
	this->mainWindow->Open();
}

AutoPtr<UserWindow> UI::GetMainWindow()
{
	return this->mainWindow;
}

void UI::_CreateWindow(const ValueList& args, KValueRef result)
{
	AutoPtr<WindowConfig> config(0);
	if (args.size() > 0 && args.at(0)->IsObject())
	{
		config = WindowConfig::FromProperties(args.GetObject(0));
	}
	else if (args.size() > 0 && args.at(0)->IsString())
	{
		std::string url(args.GetString(0));
		config = ApplicationConfig::Instance()->GetWindowByURL(url);
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

void UI::ErrorDialog(std::string msg)
{
	std::cerr << msg << std::endl;
}

UI::~UI()
{
	this->ClearTray();

	// Shutdown notifications
	Notification::ShutdownImpl();
}

std::vector<AutoPtr<UserWindow> >& UI::GetOpenWindows()
{
	return this->openWindows;
}

void UI::AddToOpenWindows(AutoPtr<UserWindow> window)
{
	this->openWindows.push_back(window);
}

void UI::RemoveFromOpenWindows(AutoPtr<UserWindow> window)
{
	static Logger* logger = Logger::Get("UI");
	std::vector<AutoPtr<UserWindow> >::iterator w = openWindows.begin();
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

void UI::_GetOpenWindows(const ValueList& args, KValueRef result)
{
	KListRef list = new StaticBoundList();
	std::vector<AutoPtr<UserWindow> >::iterator w = openWindows.begin();
	while (w != openWindows.end()) {
		list->Append(Value::NewObject(*w++));
	}
	result->SetList(list);
}

void UI::_GetMainWindow(const ValueList& args, KValueRef result)
{
	result->SetObject(this->mainWindow);
}

void UI::_CreateNotification(const ValueList& args, KValueRef result)
{
	args.VerifyException("createNotification", "?o");
	AutoPtr<Notification> n(new Notification());

	if (args.GetValue(0)->IsObject())
		n->Configure(args.GetObject(0));

	result->SetObject(n);
}

void UI::_CreateMenu(const ValueList& args, KValueRef result)
{
	result->SetObject(__CreateMenu(args));
}

AutoPtr<Menu> UI::__CreateMenu(const ValueList& args)
{
	// call into the native code to retrieve the menu
	return this->CreateMenu();
}

void UI::_CreateMenuItem(const ValueList& args, KValueRef result)
{
	result->SetObject(__CreateMenuItem(args));
}

AutoPtr<MenuItem> UI::__CreateMenuItem(const ValueList& args)
{
	args.VerifyException("createMenuItem", "?s m|0 s|0");
	std::string label = args.GetString(0, "");
	KMethodRef eventListener = args.GetMethod(1, NULL);
	std::string iconURL = args.GetString(2, "");

	AutoPtr<MenuItem> item = this->CreateMenuItem();
	if (!label.empty())
		item->SetLabel(label);
	if (!iconURL.empty())
		item->SetIcon(iconURL);
	if (!eventListener.isNull())
		item->AddEventListener(Event::CLICKED, eventListener);

	return item;
}


void UI::_CreateCheckMenuItem(const ValueList& args, KValueRef result)
{
	result->SetObject(__CreateCheckMenuItem(args));
}

AutoPtr<MenuItem> UI::__CreateCheckMenuItem(const ValueList& args)
{
	args.VerifyException("createCheckMenuItem", "?s m|0");
	std::string label = args.GetString(0, "");
	KMethodRef eventListener = args.GetMethod(1, NULL);

	AutoPtr<MenuItem> item = this->CreateCheckMenuItem();
	if (!label.empty())
		item->SetLabel(label);
	if (!eventListener.isNull())
		item->AddEventListener(Event::CLICKED, eventListener);

	return item;
}

void UI::_CreateSeparatorMenuItem(const ValueList& args, KValueRef result)
{
	result->SetObject(__CreateSeparatorMenuItem(args));
}

AutoPtr<MenuItem> UI::__CreateSeparatorMenuItem(const ValueList& args)
{
	return this->CreateSeparatorMenuItem();
}

void UI::_SetMenu(const ValueList& args, KValueRef result)
{
	args.VerifyException("setMenu", "o|0");
	AutoPtr<Menu> menu(args.GetObject(0, 0).cast<Menu>());

	this->SetMenu(menu); // platform-specific impl

	// Notify all windows that the app menu has changed.
	std::vector<AutoPtr<UserWindow> >::iterator i = openWindows.begin();
	while (i != openWindows.end()) {
		(*i++)->AppMenuChanged();
	}
}

void UI::_GetMenu(const ValueList& args, KValueRef result)
{
	AutoPtr<Menu> menu = this->GetMenu();
	if (menu.isNull())
	{
		result->SetNull();
	}
	else
	{
		result->SetObject(menu);
	}
}

void UI::_SetContextMenu(const ValueList& args, KValueRef result)
{
	args.VerifyException("setContextMenu", "o|0");
	AutoPtr<Menu> menu(args.GetObject(0, 0).cast<Menu>());
	this->SetContextMenu(menu);
}

void UI::_GetContextMenu(const ValueList& args, KValueRef result)
{
	AutoPtr<Menu> menu = this->GetContextMenu();
	result->SetObject(menu);
}

void UI::_SetIcon(const ValueList& args, KValueRef result)
{
	args.VerifyException("setIcon", "s|0");

	std::string iconURL;
	if (args.size() > 0)
		iconURL = args.GetString(0);
	this->_SetIcon(iconURL);
}

void UI::_SetIcon(std::string iconURL)
{
	std::string iconPath;
	this->iconURL = iconURL;
	if (!iconURL.empty())
		iconPath = URLUtils::URLToPath(this->iconURL);

	this->SetIcon(iconPath); // platform-specific impl

	// Notify all windows that the app menu has changed.
	std::vector<AutoPtr<UserWindow> >::iterator i = openWindows.begin();
	while (i != openWindows.end()) {
		(*i++)->AppIconChanged();
	}
}

void UI::_AddTray(const ValueList& args, KValueRef result)
{
	args.VerifyException("createTrayIcon", "s,?m");
	std::string iconURL = args.GetString(0);

	KMethodRef cbSingleClick = args.GetMethod(1, NULL);
	AutoPtr<TrayItem> item = this->AddTray(iconURL, cbSingleClick);
	this->trayItems.push_back(item);
	result->SetObject(item);
}

void UI::_ClearTray(const ValueList& args, KValueRef result)
{
	this->ClearTray();
}

void UI::ClearTray()
{
	std::vector<AutoPtr<TrayItem> >::iterator i = this->trayItems.begin();
	while (i != this->trayItems.end())
	{
		(*i++)->Remove();
	}
	this->trayItems.clear();
}

void UI::UnregisterTrayItem(TrayItem* item)
{
	std::vector<AutoPtr<TrayItem> >::iterator i = this->trayItems.begin();
	while (i != this->trayItems.end())
	{
		AutoPtr<TrayItem> c = *i;
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

void UI::_SetDockIcon(const ValueList& args, KValueRef result)
{
#if defined(OS_OSX)
	std::string iconPath;
	if (args.size() > 0) {
		std::string in = args.GetString(0);
		iconPath = URLUtils::URLToPath(in);
	}
	this->SetDockIcon(iconPath);
#endif
}

void UI::_SetDockMenu(const ValueList& args, KValueRef result)
{
#if defined(OS_OSX)
	AutoPtr<Menu> menu(args.GetObject(0, 0).cast<Menu>());
	this->SetDockMenu(menu);
#endif
}

void UI::_SetBadge(const ValueList& args, KValueRef result)
{
#if defined(OS_OSX)
	std::string badgeText;
	if (args.size() > 0) {
		badgeText = args.GetString(0);
	}

	this->SetBadge(badgeText);
#endif
}

void UI::_SetBadgeImage(const ValueList& args, KValueRef result)
{
#if defined(OS_OSX)
	std::string iconPath;
	if (args.size() > 0) {
		std::string in = args.GetString(0);
		iconPath = URLUtils::URLToPath(in);
	}

	this->SetBadgeImage(iconPath);
#endif
}

void UI::_GetIdleTime(
	const ValueList& args,
	KValueRef result)
{
	result->SetDouble(this->GetIdleTime());
}

void UI::Log(Logger::Level level, std::string& message)
{
	if (level > Logger::LWARN)
		return;

	std::string methodName("warn");
	if (level < Logger::LWARN)
		methodName = "error";

	std::string origMethodName(methodName);
	origMethodName.append("_orig");

	std::vector<AutoPtr<UserWindow> >& openWindows = UI::GetInstance()->GetOpenWindows();
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

} // namespace Titanium
