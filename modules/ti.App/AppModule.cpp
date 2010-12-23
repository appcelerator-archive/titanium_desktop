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

#include "AppModule.h"

#include <Poco/File.h>

#include "ApplicationConfig.h"
#include "Application.h"
#include "Properties.h"

namespace Titanium {

KROLL_MODULE(AppModule, STRING(MODULE_NAME), STRING(MODULE_VERSION));

static Logger* GetLogger()
{
	static Logger* logger = Logger::Get("App");
	return logger;
}

void AppModule::Initialize()
{
	// Nudge the creation of the first app config instance.
	ApplicationConfig* config = ApplicationConfig::Instance();
	if (config)
		GetLogger()->Debug("Loaded config file successfully");

	KObjectRef global(host->GetGlobalObject());
	KObjectRef binding = new Application(host, host->GetGlobalObject());
	host->GetGlobalObject()->SetObject("App", binding);

	// Create the data directory for the app, if it doesn't exist.
	std::string dataPath(host->GetApplication()->GetDataPath());
	Poco::File dataPathFile(dataPath);
	if (!dataPathFile.exists())
		dataPathFile.createDirectories();

	std::string propFilename(FileUtils::Join(
		dataPath.c_str(), "application.properties", 0));

	// @tiapi(property=True,type=App.Properties,name=App.Properties,since=0.2)
	// @tiapi The application's private Properties object
	binding->SetObject("Properties", new Properties(propFilename));
}

void AppModule::Stop()
{
	host->GetGlobalObject()->SetNull("App");
}

} // namespace Titanium
