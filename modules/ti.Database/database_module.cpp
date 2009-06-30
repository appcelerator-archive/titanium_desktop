/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license. 
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#include <kroll/kroll.h>
#include "database_module.h"
#include "database_binding.h"

using namespace kroll;
using namespace ti;

namespace ti
{
	KROLL_MODULE(DatabaseModule, STRING(MODULE_NAME), STRING(MODULE_VERSION));
	
	void DatabaseModule::Initialize()
	{
		/**
		 * @tiapi(method=True,name=Database.open,since=0.4) Opens a database
		 * @tiarg(for=Database.open,name=name,type=string) database name
		 * @tiresult(for=Database.open,type=Database.DB) returns a Database.DB
		 */
		this->SetMethod("open",&DatabaseModule::Open);

		// set our ti.Database
		SharedValue value = Value::NewObject(this);
		host->GetGlobalObject()->Set("Database", value);
	}

	void DatabaseModule::Stop()
	{
	}
	
	void DatabaseModule::Open(const ValueList& args, SharedValue result)
	{
		DatabaseBinding *db = new DatabaseBinding(host);
		db->Open(args,result);
		SharedKObject kdb = db;
		result->SetObject(kdb);
	}
}
