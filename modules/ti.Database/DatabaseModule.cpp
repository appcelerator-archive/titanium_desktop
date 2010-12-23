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

#include "DatabaseModule.h"

#include <Poco/Data/SQLite/Connector.h>
#include <Poco/Data/SQLite/SQLiteException.h>

#include "Database.h"

namespace Titanium {

KROLL_MODULE(DatabaseModule, STRING(MODULE_NAME), STRING(MODULE_VERSION));

void DatabaseModule::Initialize()
{
	Poco::Data::SQLite::Connector::registerConnector();

	/**
	 * @tiapi Opens a WebKit database, given a database name. This database
	 * @tiapi will be opened with the security origin of the application's
	 * @tiapi app:// url.
	 * @tiarg[String, name] The name of the database to open.
	 * @tiresult[Database.DB] The new database object.
	 */
	this->SetMethod("open", &DatabaseModule::Open);

	/**
	 * @tiapi(method=True,name=Database.openFile,since=0.8)
	 * @tiapi Opens a database, given a path to an sqlite file.
	 * @tiarg[String, path] Path to an SQLite file to store the database
	 * @tiarg in. If the file does not exist, it will be created.
	 * @tiresult[Database.DB] The new database object.
	 */
	this->SetMethod("openFile", &DatabaseModule::OpenFile);

	KObjectRef autoThis(this, true);
	host->GetGlobalObject()->SetObject("Database", autoThis);
}

void DatabaseModule::Stop()
{
	host->GetGlobalObject()->SetUndefined("Database");
}

void DatabaseModule::Open(const ValueList& args, KValueRef result)
{
	args.VerifyException("open", "?s");
	std::string name(args.GetString(0, "unnamed"));
	result->SetObject(new Database(name, true));
}

void DatabaseModule::OpenFile(const ValueList& args, KValueRef result)
{
	args.VerifyException("openFile", "s|o");
	std::string name;
	if (args.at(0)->IsString())
	{
		name = args.GetString(0);
	}
	else
	{
		const char* error = "openFile requires an object with a "
			"toString method returning a String";

		KObjectRef o(args.GetObject(0));
		if (!o->Get("toString")->IsMethod())
			throw ValueException::FromString(error);

		KValueRef v(o->Get("toString")->ToMethod()->Call());
		if (!v->IsString())
			throw ValueException::FromString(error);

		name = v->ToString();
	}

	result->SetObject(new Database(name, false));
}

} // namespace Titanium
