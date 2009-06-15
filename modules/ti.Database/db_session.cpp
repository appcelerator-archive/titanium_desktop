/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#include "db_session.h"

namespace ti
{
	DBSession::DBSession(std::string location)
	{
		//TODO: make this static
		Poco::Data::SQLite::Connector::registerConnector();
		this->session = new Session("SQLite", location);
	}

	DBSession::~DBSession()
	{
		if (this->session)
		{
			this->session->close();
			delete this->session;
			this->session = NULL;
		}
	}
}
