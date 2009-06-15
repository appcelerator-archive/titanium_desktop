/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#ifndef TI_DATABASES_H
#define TI_DATABASES_H

#include "db_session.h"

namespace ti
{
	/**
	 * Databases is a convenience class for handling WebKit 
	 * compatible databases
	 */
	class Databases
	{
	public:
		Databases(std::string datadir);
		virtual ~Databases();
		
		/**
		 * returns true if database for origin exists, false if not found
		 */
		bool Exists(std::string origin, std::string name);
		
		/**
		 * return the path to an existing database with origin and name
		 */
		std::string Path(std::string origin, std::string name);
		
		/**
		 * create a new database with name for origin
		 */
		std::string Create(std::string origin, std::string name);
		
		/**
		 * delete an existing database for name and origin
		 */
		void Delete (std::string origin, std::string name);
		
		/**
		 * return the session reference
		 */
		Session& GetSession() const { return session->GetSession(); }
		
	private:
		std::string datadir;
		DBSession *session;
	};
}

#endif