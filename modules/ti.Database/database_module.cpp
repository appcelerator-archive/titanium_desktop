/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license. 
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#include <kroll/kroll.h>
#include "database_module.h"
#include "database_binding.h"

#include <Poco/Data/SQLite/Connector.h>
#include <Poco/Data/SQLite/SQLiteException.h>

using namespace kroll;
using namespace ti;

namespace ti
{
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
		result->SetObject(new DatabaseBinding(name, true));
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

		result->SetObject(new DatabaseBinding(name, false));
	}
}
