/*
 * Copyright (c) 2008-2010 Appcelerator, Inc. All Rights Reserved.
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

#ifndef ApplicationConfig_h
#define ApplicationConfig_h

#include <string>
#include <vector>

#include <kroll/kroll.h>

#include "WindowConfig.h"
#include "Properties.h"

namespace Titanium {

class EXPORT ApplicationConfig {
public:
	virtual ~ApplicationConfig() {}

	AutoPtr<WindowConfig> GetWindowByURL(const std::string& url);
	AutoPtr<WindowConfig> GetMainWindow();
	std::string& GetAppName() { return appName; }
	std::string& GetAppID() { return appID; }
	std::string& GetDescription() { return description; }
	std::string& GetCopyright() { return copyright; }
	std::string& GetURL() { return url; }
	std::string& GetVersion() { return version; }
	std::string& GetPublisher() { return publisher; }
	AutoPtr<Properties> GetSystemProperties() { return systemProperties; }
	std::string& GetIcon() { return icon; }
	bool IsAnalyticsEnabled() { return analyticsEnabled; }
	bool IsUpdateMonitorEnabled() { return updateMonitorEnabled; }
	static ApplicationConfig* Instance();

private:
	ApplicationConfig(std::string& xmlfile);

	AutoPtr<Properties> systemProperties;
	std::vector<AutoPtr<WindowConfig> > windows;
	std::string appName;
	std::string appID;
	std::string description;
	std::string copyright;
	std::string url;
	std::string version;
	std::string publisher;
	std::string icon;
	bool analyticsEnabled;
	bool updateMonitorEnabled;
	static ApplicationConfig* instance_;
};

} // namespace Titanium

#endif
