/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */
#ifndef TI_APP_CONFIG_H_
#define TI_APP_CONFIG_H_

#include <string>
#include <cstring>
#include <vector>
#include <algorithm>
#include <sstream>

#include "app_api.h"
#include "properties_binding.h"

#define nodeNameEquals(n,s) (xmlStrcmp(n->name, s) == 0)

namespace ti
{

class WindowConfig;
class PropertiesBinding;

class TITANIUM_APP_API AppConfig
{
public:
	~AppConfig();

	WindowConfig* GetWindow(std::string &id);
	WindowConfig* GetWindowByURL(std::string url);
	WindowConfig* GetMainWindow();

	std::string& GetAppName() { return appName; }
	std::string& GetAppID() { return appID; }
	std::string& GetDescription() { return description; }
	std::string& GetCopyright() { return copyright; }
	std::string& GetURL() { return url; }
	std::string& GetVersion() { return version; }
	std::string& GetPublisher() { return publisher; }
	AutoPtr<PropertiesBinding> GetSystemProperties() { return systemProperties; }
	std::vector<WindowConfig*>& GetWindows() { return windows; }
	std::string& GetIcon() { return icon; }
	const char* GetError() { return error; }
	bool IsAnalyticsEnabled() { return analyticsEnabled; }

	static AppConfig* Instance();

private:
	AutoPtr<PropertiesBinding> systemProperties;
	std::vector<WindowConfig*> windows;
	const char* error;
	std::string appName;
	std::string appID;
	std::string description;
	std::string copyright;
	std::string url;
	std::string version;
	std::string publisher;
	std::string icon;
	bool analyticsEnabled;
	static AppConfig *instance_;

	AppConfig(std::string& xmlfile);
};

}
#endif
