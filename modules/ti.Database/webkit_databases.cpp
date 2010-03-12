/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#include <kroll/kroll.h>
#include <Poco/File.h>
#include "webkit_databases.h"

#include <Poco/Data/AbstractBinding.h>
#include <Poco/Data/RecordSet.h>
#include <Poco/Data/MetaColumn.h>
#include <Poco/Data/Statement.h>
#include <Poco/Data/SessionFactory.h>
#include <Poco/Data/SQLite/Connector.h>
#include <Poco/Data/SQLite/SQLiteException.h>

using Poco::Data::Session;
using Poco::Data::Statement;
using Poco::Data::RecordSet;
using Poco::Data::use;
using Poco::Data::into;
using Poco::Data::now;

//FIXME: add SessionPool

namespace ti
{
	static Logger* GetLogger()
	{
		static Logger* logger = Logger::Get("Database.DB");
		return logger;
	}

	static std::string& GetApplicationSecurityOrigin()
	{
		static std::string origin;
		if (origin.empty())
		{
			// This code is loosely based on:
			//http://www.opensource.apple.com/darwinsource/Current/WebCore-5525.18.1/platform/SecurityOrigin.cpp
			origin.append("app_"); // Protocol
			origin.append(Host::GetInstance()->GetApplication()->id);
			origin.append("_0"); // Port
		}
		return origin;
	}

	static std::string& GetDataPath()
	{
		static std::string dataDirectory(
			Host::GetInstance()->GetApplication()->GetDataPath().c_str());
		return dataDirectory;
	}

	static std::string& GetDataPath(std::string origin)
	{
		static std::string dataDirectory(FileUtils::Join(
			GetDataPath().c_str(), origin.c_str(), NULL));
		return dataDirectory;
	}

	WebKitDatabases::WebKitDatabases() :
		origin(GetApplicationSecurityOrigin()),
		originPath(GetDataPath(origin)),
		session(0)
	{
		std::string dbPath = FileUtils::Join(GetDataPath().c_str(), "Databases.db", NULL);
		GetLogger()->Debug("DB Path = %s", dbPath.c_str());
		this->session = new Session("SQLite", dbPath);

		Statement select(*this->session);
		GetLogger()->Debug("Creating table Origins");
		select << "CREATE TABLE IF NOT EXISTS Origins (origin TEXT UNIQUE ON "
			"CONFLICT REPLACE, quota INTEGER NOT NULL ON CONFLICT FAIL)", now;

		Statement select2(*this->session);
		GetLogger()->Debug("Creating table Databases");
		select2 << "CREATE TABLE IF NOT EXISTS Databases (guid INTEGER PRIMARY KEY "
			"AUTOINCREMENT, origin TEXT, name TEXT, displayName TEXT, estimatedSize "
			"INTEGER, path TEXT)", now;
	}

	WebKitDatabases::~WebKitDatabases()
	{
		if (this->session)
			delete session;
	}

	std::string WebKitDatabases::Create(std::string name)
	{
		Statement select(*this->session);
		Poco::UInt32 seq = 0;
		select << "SELECT seq FROM sqlite_sequence WHERE name='Databases'", into(seq);
		select.execute();

		++seq;

		std::string filename = Poco::format("%016u.db", (unsigned int) seq);
		GetLogger()->Debug("Creating new db: %s", filename.c_str());

		Statement select2(*this->session);
		select2 << "INSERT INTO Databases (origin, name, path) VALUES (:origin,:name,:path)",
			 use(this->origin), use(name), use(filename);
		select2.execute();

		Statement select5(*this->session);
		select5 << "SELECT origin from Origins where origin = :origin", use(this->origin);
		Poco::Int32 count = select5.execute();
		if (count == 0)
		{
			Statement select(*this->session);
			select << "INSERT INTO Origins (origin,quota) values (:origin,1720462881547374560)",
				use(this->origin), now;
		}

		// Create the path for this application's origin, if necessary.
		if (!FileUtils::IsDirectory(originPath))
		{
			GetLogger()->Debug("Creating new database directory: %s", originPath.c_str());
			FileUtils::CreateDirectory(originPath);
		}

		std::string filePath(FileUtils::Join(originPath.c_str(), filename.c_str(), NULL));
		GetLogger()->Debug("path to new database: %s", filePath.c_str());

		// Create the metadata table for WebKit
		Session fileSession("SQLite", filePath);
		Statement select3(fileSession);
		select3 << "CREATE TABLE __WebKitDatabaseInfoTable__ (key TEXT NOT NULL "
			"ON CONFLICT FAIL UNIQUE ON CONFLICT REPLACE,value TEXT NOT NULL ON "
			"CONFLICT FAIL)", now;

		Statement select4(fileSession);
		select4 << "insert into __WebKitDatabaseInfoTable__ values "
			"('WebKitDatabaseVersionKey','1.0')", now;

		return filePath;
	}

	void WebKitDatabases::Delete(std::string name)
	{
		if (Exists(name))
		{
			std::string path = Path(name);
			Statement select(*this->session);
			select << "DELETE FROM Databases WHERE origin=:origin AND name=:name", 
				use(this->origin), use(name), now;

			Poco::File f(path);
			f.remove(true);

			GetLogger()->Debug("deleted database file: %s", path.c_str());
		}
		else
		{
			GetLogger()->Debug("Delete called with origin:%s, name: %s - but this DB "
				"doesn't appear to exist", this->origin.c_str(), name.c_str());
		}
	}

	std::string WebKitDatabases::Path(std::string name)
	{
		if (!Exists(name))
			return Create(name);

		Statement select(*this->session);
		try
		{
			std::string path;
			select << "SELECT path FROM Databases WHERE origin=:origin AND name=:name",
				 use(this->origin), use(name), into(path);
			Poco::UInt32 count = select.execute();
			if (count > 0)
			{
				return FileUtils::Join(this->originPath.c_str(), path.c_str(), NULL);
			}
		}
		catch (Poco::Data::SQLite::InvalidSQLStatementException &se)
		{
			// NO DB
		}
		return "";
	}

	bool WebKitDatabases::Exists(std::string name)
	{
		Statement select(*this->session);
		try
		{
			select << "SELECT guid FROM Databases WHERE origin=:origin AND name=:name",
				use(this->origin), use(name);
			Poco::UInt32 count = select.execute();
			return count > 0;
		}
		catch (Poco::Data::SQLite::InvalidSQLStatementException &se)
		{
			// NO DB
			return false;
		}
	}
}
