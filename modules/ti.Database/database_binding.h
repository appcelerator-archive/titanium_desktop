/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _DATABASE_BINDING_H_
#define _DATABASE_BINDING_H_

#include <kroll/kroll.h>
#include "webkit_databases.h"
#include <Poco/Data/Session.h>
#include <Poco/Data/Statement.h>

namespace ti
{
	class DatabaseBinding : public KAccessorObject
	{
	public:
		DatabaseBinding(std::string& name, bool isWebKitDatabase);

	protected:
		virtual ~DatabaseBinding();
		void Open(const ValueList& args, KValueRef result);
		void Execute(const ValueList& args, KValueRef result);
		void Close(const ValueList& args, KValueRef result);
		void Remove(const ValueList& args, KValueRef result);
		void GetPath(const ValueList& args, KValueRef result);
		void Close();

		Poco::Data::Session *session;
		std::string name;
		std::string path;
		bool isWebKitDatabase;
	};
}

#endif
