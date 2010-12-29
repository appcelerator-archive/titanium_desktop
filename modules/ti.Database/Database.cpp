/*
 * Copyright (c) 2009-2010 Appcelerator, Inc. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "Database.h"

#include <kroll/kroll.h>
#include <Poco/Data/AbstractBinding.h>
#include <Poco/Data/RecordSet.h>
#include <Poco/Data/MetaColumn.h>
#include <Poco/Data/Statement.h>
#include <Poco/Data/SessionFactory.h>
#include <Poco/Data/SQLite/Connector.h>
#include <Poco/Data/SQLite/SQLiteException.h>
#include <Poco/File.h>

#include "ResultSet.h"
#include "WebKitDatabases.h"

using Poco::Data::Session;
using Poco::Data::Statement;
using Poco::Data::RecordSet;
using Poco::Data::use;
using Poco::Data::into;
using Poco::Data::now;

namespace Titanium {

static Logger* GetLogger()
{
    static Logger* logger = Logger::Get("Database.DB");
    return logger;
}

/**
 * this is a class that manages Value to Data bindings
 * and manages the memory that needs to be created during the
 * lifecycle of the Statement and then frees the memory once
 * this binding is destructed
 */
class ValueBinding
{
public:
    void convert (Statement &select, KValueRef arg)
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

static WebKitDatabases* GetWebKitDatabases()
{
    static WebKitDatabases* databases = new WebKitDatabases();
    return databases;
}

Database::Database(std::string& name, bool isWebKitDatabase) :
    KAccessorObject("Database.DB"),
    session(0),
    name(name),
    path(name),
    isWebKitDatabase(isWebKitDatabase)
{
    /**
     * @tiapi(method=True,name=Database.DB.execute,since=0.4) Executes an SQL query on the database.
     * @tiarg(for=Database.DB.execute,name=sql,type=String) the SQL query to execute.
     * @tiresult(for=Database.DB.execute,type=Database.ResultSet) returns a Database.ResultSet
     */
    this->SetMethod("execute", &Database::Execute);

    /**
     * @tiapi(method=True,name=Database.DB.close,since=0.4) Closes an open database
     */
    this->SetMethod("close", &Database::Close);

    /**
     * @tiapi(method=True,name=Database.DB.remove,since=0.4) Removes a database
     */
    this->SetMethod("remove", &Database::Remove);

    /**
     * @tiapi(method=True,name=Database.DB.getPath,since=0.8)
     * @tiapi Get the full filesystem path to the database.
     * @tiresult[String] the full fileystem path to the database.
     */
    this->SetMethod("getPath", &Database::GetPath);

    /**
     * @tiapi(property=True,name=Database.DB.lastInsertRowId) The row id of the last insert operation.
     */
    this->SetInt("lastInsertRowId", 0);

    /**
     * @tiapi(property=True,name=Database.DB.rowsAffected) The number of rows affected by the last execute
     */
    this->SetInt("rowsAffected", 0);

    if (isWebKitDatabase)
        this->path = GetWebKitDatabases()->Path(name);

    session = new Session("SQLite", path);
}

Database::~Database()
{
    if (session)
        delete session;
}

void Database::Execute(const ValueList& args, KValueRef result)
{
    args.VerifyException("execute", "s");

    if (!session)
        throw ValueException::FromString("Tried to call execute, but database was closed.");

    std::string sql(args.GetString(0));
    GetLogger()->Debug("Execute called with %s", sql.c_str());
    
    Statement select(*this->session);
    
    try
    {
        ValueBinding binding;
        
        select << sql;
        
        if (args.size()>1)
        {
            
            for (size_t c=1;c<args.size();c++)
            {
                KValueRef anarg = args.at(c);
                if (anarg->IsList())
                {
                    KListRef list = anarg->ToList();
                    for (size_t a=0;a<list->Size();a++)
                    {
                        KValueRef arg = list->At(a);
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

        GetLogger()->Debug("sql returned: %d rows for result",count);

        this->SetInt("rowsAffected",count);

        // get the row insert id
        Statement ss(*this->session);
        ss << "select last_insert_rowid()", now;
        RecordSet rr(ss);
        Poco::DynamicAny value = rr.value(0);
        int i;
        value.convert(i);
        this->SetInt("lastInsertRowId",i);

        
        if (count > 0)
        {
            RecordSet rs(select);
            KObjectRef r = new ResultSet(rs);
            result->SetObject(r);
        }
        else
        {
            KObjectRef r = new ResultSet();
            result->SetObject(r);
        }
    }
    catch (Poco::Data::DataException &e)
    {
        GetLogger()->Error("Exception executing: %s, Error was: %s", sql.c_str(),
            e.what());
        throw ValueException::FromString(e.what());
    }
}

void Database::Close(const ValueList& args, KValueRef result)
{
    GetLogger()->Debug("Closing database: %s", name.c_str());
    this->Close();
}

void Database::Close()
{
    if (session)
    {
        delete session;
        session = 0;
    }
}

void Database::GetPath(const ValueList& args, KValueRef result)
{
    result->SetString(this->path);
}

void Database::Remove(const ValueList& args, KValueRef result)
{
    this->Close();

    if (isWebKitDatabase)
    {
        GetWebKitDatabases()->Delete(name);
    }
    else
    {
        GetLogger()->Debug("Removing database file: %s", path.c_str());
        Poco::File file(path);
        if (file.canWrite())
            file.remove();
    }
}

} // namespace Titanium
