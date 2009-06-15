/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#ifndef TI_DBSESSION_H
#define TI_DBSESSION_H

#include <Poco/Data/SessionFactory.h>
#include <Poco/Data/Session.h>
#include <Poco/Data/RecordSet.h>
#include <Poco/Data/SQLite/Connector.h>
#include <Poco/Data/SQLite/SQLiteException.h>
#include <Poco/Data/SessionPool.h>
#include <iostream>

using namespace Poco::Data;

namespace ti
{
	/**
	 * DBSession is a simple holder for a Poco::Data::Session instance
	 */
	class DBSession
	{
	public:
		DBSession(std::string location);
		virtual ~DBSession();
		
		/**
		 * return the session reference
		 */
		Session& GetSession() const { return *session; }

	private:
		Session *session;
		
	};
}

#endif