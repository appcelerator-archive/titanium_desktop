/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#include <kroll/kroll.h>
#include <Poco/File.h>
#include "databases.h"

//FIXME: add SessionPool

namespace ti
{
	Databases::Databases(std::string datadir) : datadir(datadir)
	{
		std::string dbpath = FileUtils::Join(datadir.c_str(),"Databases.db",NULL);

		static Logger* logger = Logger::Get("Database.Databases");
		logger->Debug("DB Path = %s",dbpath.c_str());

		session = new DBSession(dbpath);

		Statement select(this->session->GetSession());
		logger->Debug("Creating table Origins");
		select << "CREATE TABLE IF NOT EXISTS Origins (origin TEXT UNIQUE ON CONFLICT REPLACE, quota INTEGER NOT NULL ON CONFLICT FAIL)", now;

		Statement select2(this->session->GetSession());
		logger->Debug("Creating table Databases");
		select2 << "CREATE TABLE IF NOT EXISTS Databases (guid INTEGER PRIMARY KEY AUTOINCREMENT, origin TEXT, name TEXT, displayName TEXT, estimatedSize INTEGER, path TEXT)", now;
	}


	Databases::~Databases()
	{
		if (session)
		{
			delete session;
		}
	}

	std::string Databases::Create(std::string origin, std::string name)
	{
		if (Exists(origin, name))
		{
			return Path(origin,name);
		}
		static Logger* logger = Logger::Get("Database.Databases");

		Statement select(this->session->GetSession());
		Poco::UInt32 seq = 0;
		select << "SELECT seq FROM sqlite_sequence WHERE name='Databases'", into(seq);
		select.execute();

		++seq;

		std::string filename = Poco::format("%016u.db",(unsigned int)seq);
		logger->Debug("creating new db: %s",filename.c_str());

		Statement select2(this->session->GetSession());
		select2 << "INSERT INTO Databases (origin, name, path) VALUES (:origin,:name,:path)", use(origin), use(name), use(filename);
		select2.execute();

		Statement select5(this->session->GetSession());
		select5 << "SELECT origin from Origins where origin = :origin", use(origin);
		Poco::Int32 count = select5.execute();
		if (count == 0)
		{
			Statement select(this->session->GetSession());
			select << "INSERT INTO Origins (origin,quota) values (:origin,1720462881547374560)", use(origin), now;
		}

		// create the DB file
		std::string dbdir = FileUtils::Join(datadir.c_str(),origin.c_str(),NULL);
		if (!FileUtils::IsDirectory(dbdir))
		{
			logger->Debug("creating new db dir: %s",dbdir.c_str());
			FileUtils::CreateDirectory(dbdir);
		}
		std::string fullpath = FileUtils::Join(dbdir.c_str(),filename.c_str(),NULL);
		logger->Debug("path to new db : %s",fullpath.c_str());

		DBSession s(fullpath);

		// create the metadata table for WebKit
		Statement select3(s.GetSession());
		select3 << "CREATE TABLE __WebKitDatabaseInfoTable__ (key TEXT NOT NULL ON CONFLICT FAIL UNIQUE ON CONFLICT REPLACE,value TEXT NOT NULL ON CONFLICT FAIL)", now;

		Statement select4(s.GetSession());
		select4 << "insert into __WebKitDatabaseInfoTable__ values ('WebKitDatabaseVersionKey','1.0')", now;

		return fullpath;
	}

	void Databases::Delete (std::string origin, std::string name)
	{
		static Logger* logger = Logger::Get("Database.Databases");
		if (Exists(origin, name))
		{
			std::string path = Path(origin,name);

			Statement select(this->session->GetSession());
			select << "DELETE FROM Databases WHERE origin=:origin AND name=:name", use(origin), use(name), now;

			Poco::File f(path);
			f.remove(true);

			logger->Debug("deleted database file: %s",path.c_str());
		}
		else
		{
			logger->Debug("delete called with origin:%s, name: %s - but this DB doesn't appear to exist",origin.c_str(),name.c_str());
		}
	}

	std::string Databases::Path(std::string origin, std::string name)
	{
		Statement select(this->session->GetSession());
		try
		{
			std::string path;
			select << "SELECT path FROM Databases WHERE origin=:origin AND name=:name", use(origin), use(name), into(path);
			Poco::UInt32 count = select.execute();
			if (count > 0)
			{
				return FileUtils::Join(datadir.c_str(),origin.c_str(),path.c_str(),NULL);
			}
		}
		catch(Poco::Data::SQLite::InvalidSQLStatementException &se)
		{
			// NO DB
		}
		return "";
	}

	bool Databases::Exists(std::string origin, std::string name)
	{
		Statement select(this->session->GetSession());
		try
		{
			select << "SELECT guid FROM Databases WHERE origin=:origin AND name=:name", use(origin), use(name);
			Poco::UInt32 count = select.execute();
			return count > 0;
		}
		catch(Poco::Data::SQLite::InvalidSQLStatementException &se)
		{
			// NO DB
			return false;
		}
	}
}
