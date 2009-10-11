/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license. 
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */	
#include <kroll/kroll.h>
#include "database_binding.h"
#include "resultset_binding.h"
#include <Poco/Data/AbstractBinding.h>

namespace ti
{
	/**
	 * this is a class that manages Value to Data bindings
	 * and manages the memory that needs to be created during the
	 * lifecycle of the Statement and then frees the memory once
	 * this binding is destructed
	 */
	class ValueBinding
	{
	public:
		void convert (Statement &select, SharedValue arg)
		{
			if (arg->IsString())
			{
				std::string *s = new std::string(arg->ToString()); 
				select , use(*s);
				strings.push_back(s);
			}
			else if (arg->IsInt())
			{
				int *i = new int(arg->ToInt());
				select , use(*i);
				ints.push_back(i);
			}
			else if (arg->IsDouble())
			{
				double *d = new double(arg->ToDouble());
				select , use(*d);
				doubles.push_back(d);
			}
			else if (arg->IsBool())
			{
				bool *b = new bool(arg->ToBool());
				select , use(*b);
				bools.push_back(b);
			}
			else if (arg->IsNull() || arg->IsUndefined())
			{
				// in this case, we bind a null string (dequoted)
				std::string *s = new std::string("null");
				select , use(s);
				strings.push_back(s);
			}
			else
			{
				throw ValueException::FromFormat("Unsupport type for argument: %s",
					arg->GetType().c_str());
			}
		} 
		ValueBinding() {}
		~ValueBinding()
		{
			if (strings.size()>0)
			{
				std::vector<std::string*>::iterator i = strings.begin();
				while(i!=strings.end())
				{
					std::string *s = (*i);
					delete s;
					i++;
				}
			}
			if (ints.size()>0)
			{
				std::vector<int*>::iterator i = ints.begin();
				while(i!=ints.end())
				{
					int *s = (*i);
					delete s;
					i++;
				}
			}
			if (doubles.size()>0)
			{
				std::vector<double*>::iterator i = doubles.begin();
				while(i!=doubles.end())
				{
					double *s = (*i);
					delete s;
					i++;
				}
			}
			if (bools.size()>0)
			{
				std::vector<bool*>::iterator i = bools.begin();
				while(i!=bools.end())
				{
					bool *s = (*i);
					delete s;
					i++;
				}
			}
		}
	private:
		std::vector<std::string*> strings;
		std::vector<int*> ints;
		std::vector<double*> doubles;
		std::vector<bool*> bools;
	};
	
	DatabaseBinding::DatabaseBinding(Host *host) : StaticBoundObject("DB"), host(host), database(NULL), session(NULL)
	{
		/**
		 * @tiapi(method=True,name=Database.DB.execute,since=0.4) Executes an SQL query on the database.
		 * @tiarg(for=Database.DB.execute,name=sql,type=String) the SQL query to execute.
		 * @tiresult(for=Database.DB.execute,type=Database.ResultSet) returns a Database.ResultSet
		 */
		this->SetMethod("execute",&DatabaseBinding::Execute);
		/**
		 * @tiapi(method=True,name=Database.DB.close,since=0.4) Closes an open database
		 */
		this->SetMethod("close",&DatabaseBinding::Close);
		/**
		 * @tiapi(method=True,name=Database.DB.remove,since=0.4) Removes a database
		 */
		this->SetMethod("remove",&DatabaseBinding::Remove);

		/**
		 * @tiapi(property=True,name=Database.DB.lastInsertRowId) The row id of the last insert operation.
		 */
		this->SetInt("lastInsertRowId", 0);
		
		/**
		 * @tiapi(property=True,name=Database.DB.rowsAffected) The number of rows affected by the last execute
		 */
		this->SetInt("rowsAffected", 0);
	}
	DatabaseBinding::~DatabaseBinding()
	{
		if (database)
		{
			delete database;
			database=NULL;
		}
		if (session)
		{
			delete session;
			session = NULL;
		}
	}
	std::string DatabaseBinding::GetSecurityOrigin(std::string &appid)
	{
		//this code is loosely based on:
		//http://www.opensource.apple.com/darwinsource/Current/WebCore-5525.18.1/platform/SecurityOrigin.cpp
		std::string origin = "app_"; // protocol which is app
		origin+=appid; // host which is the appid
		origin+="_0"; // port which is always 0
		return origin;
	}
	void DatabaseBinding::Open(const ValueList& args, SharedValue result)
	{
		if (database)
		{
			delete database;
			database = NULL;
		}
		if (session)
		{
			delete session;
			session = NULL;
		}
		std::string appid = host->GetApplicationID();
		std::string dbdir = FileUtils::GetApplicationDataDirectory(appid);
		dbname = args.GetString(0, "unnamed");
		origin = GetSecurityOrigin(appid);

		static Logger* logger = Logger::Get("Database");
		logger->Debug("appid=%s,dir=%s,dbname=%s,origin=%s",appid.c_str(),dbdir.c_str(),dbname.c_str(),origin.c_str());

		database = new Databases(dbdir);
		std::string path;
		if (!database->Exists(origin,dbname))
		{
			path = database->Create(origin,dbname);
		}
		else
		{
			path = database->Path(origin,dbname);
		}
		session = new DBSession(path);
	}
	void DatabaseBinding::Convert(Statement &select, SharedValue arg, std::vector<SharedPtr <void*> >& mem)
	{
		if (arg->IsString())
		{
			std::string *s = new std::string(arg->ToString()); 
			select , use(*s);
		}
		else if (arg->IsInt())
		{
			int *i = new int(arg->ToInt());
			select , use(*i);
		}
		else if (arg->IsDouble())
		{
			double *d = new double(arg->ToDouble());
			select , use(*d);
		}
		else if (arg->IsBool())
		{
			bool *b = new bool(arg->ToBool());
			select , use(*b);
		}
		else
		{
			std::string msg = "Unsupported type for argument (";
			msg.append(arg->GetType());
			msg.append(")");
			throw ValueException::FromString(msg);
		}
	}
	void DatabaseBinding::Execute(const ValueList& args, SharedValue result)
	{
		if (database == NULL)
		{
			throw ValueException::FromString("no database opened");
		}
		std::string sql = args.at(0)->ToString();

		static Logger* logger = Logger::Get("Database");
		logger->Debug("Execute called with %s",sql.c_str());
		
		Statement select(session->GetSession());
		
		try
		{
			ValueBinding binding;
			
			select << sql;
			
			if (args.size()>1)
			{
				
				for (size_t c=1;c<args.size();c++)
				{
					SharedValue anarg = args.at(c);
					if (anarg->IsList())
					{
						SharedKList list = anarg->ToList();
						for (size_t a=0;a<list->Size();a++)
						{
							SharedValue arg = list->At(a);
							binding.convert(select,arg);
						}
					}
					else
					{
						binding.convert(select,anarg);
					}
				}
			}
			Poco::UInt32 count = select.execute();

			logger->Debug("sql returned: %d rows for result",count);

			this->SetInt("rowsAffected",count);

			// get the row insert id
			Statement ss(session->GetSession());
			ss << "select last_insert_rowid()", now;
			RecordSet rr(ss);
			Poco::DynamicAny value = rr.value(0);
			int i;
			value.convert(i);
			this->SetInt("lastInsertRowId",i);

			
			if (count > 0)
			{
				RecordSet rs(select);
				SharedKObject r = new ResultSetBinding(rs);
				result->SetObject(r);
			}
			else
			{
				SharedKObject r = new ResultSetBinding();
				result->SetObject(r);
			}
		}
		catch (Poco::Data::DataException &e)
		{
			logger->Error("Exception executing: %s, Error was: %s",sql.c_str(),e.what());
			throw ValueException::FromString(e.what());
		}
	}
	void DatabaseBinding::Close(const ValueList& args, SharedValue result)
	{
		static Logger* logger = Logger::Get("Database");
		logger->Debug("Close database: %s",dbname.c_str());
		if (session)
		{
			delete session;
			session = NULL;
		}
		if (database)
		{
			delete database;
			database = NULL;
		}
	}
	void DatabaseBinding::Remove(const ValueList& args, SharedValue result)
	{
		static Logger* logger = Logger::Get("Database");
		logger->Debug("Remove database: %s",dbname.c_str());
		if (session)
		{
			delete session;
			session = NULL;
		}
		if (database)
		{
			database->Delete(origin,dbname);
			delete database;
			database = NULL;
		}
	}
}
