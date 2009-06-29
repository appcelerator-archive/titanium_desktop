/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _DATABASE_BINDING_H_
#define _DATABASE_BINDING_H_

#include <kroll/kroll.h>
#include "databases.h"
#include "db_session.h"

namespace ti
{
	class DatabaseBinding : public StaticBoundObject
	{
	public:
		DatabaseBinding(Host*);
	protected:
		virtual ~DatabaseBinding();
	private:
		Host* host;
		Databases *database;
		DBSession *session;
		std::string dbname;
		std::string origin;
		
		std::string GetSecurityOrigin(std::string &appid);
		void Convert(Statement &select, SharedValue arg, std::vector<SharedPtr <void*> >& mem);

		void Open(const ValueList& args, SharedValue result);
		void Execute(const ValueList& args, SharedValue result);
		void Close(const ValueList& args, SharedValue result);
		void Remove(const ValueList& args, SharedValue result);
		
		friend class DatabaseModule;
	};
}

#endif
