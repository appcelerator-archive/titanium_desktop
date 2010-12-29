/*
 * Copyright (c) 2008-2010 Appcelerator, Inc. All Rights Reserved.
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

#include "Application.h"

#include <Poco/Environment.h>

#include "ApplicationConfig.h"
#include "Properties.h"

namespace Titanium {

Application::Application(Host* host, KObjectRef global) :
    KAccessorObject("App"),
    host(host),
    global(global)
{
    /**
     * @tiapi(method=True,name=App.getID,since=0.2)
     * @tiapi Return the application id.
     * @tiresult[String] The application id.
     */
    this->SetMethod("getID", &Application::GetID);

    /**
     * @tiapi(method=True,name=App.getName,since=0.2)
     * @tiapi Return the application name.
     * @tiresult[String] The application name.
     */
    this->SetMethod("getName", &Application::GetName);

    /**
     * @tiapi(method=True,name=App.getVersion,since=0.2)
     * @tiapi Return the application version.
     * @tiresult[String] The application version.
     */
    this->SetMethod("getVersion", &Application::GetVersion);

    /**
     * @tiapi(method=True,name=App.getPublisher,since=0.4)
     * @tiapi Return the application publisher.
     * @tiresult[String] The application publisher.
     */
    this->SetMethod("getPublisher", &Application::GetPublisher);

    /**
     * @tiapi(method=True,name=App.getURL,since=0.4)
     * @tiapi Return the application URL.
     * @tiresult[String] The application URL.
     */
    this->SetMethod("getURL", &Application::GetURL);

    /**
     * @tiapi(method=True,name=App.getDescription,since=0.4)
     * @tiapi Return the application description.
     * @tiresult[String] The application description.
     */
    this->SetMethod("getDescription", &Application::GetDescription);

    /**
     * @tiapi(method=True,name=App.getCopyright,since=0.4)
     * @tiapi Return the application's copyright information
     * @tiresult[String] The application copyright.
     */
    this->SetMethod("getCopyright", &Application::GetCopyright);

    /**
     * @tiapi(method=True,name=App.getGUID,since=0.2)
     * @tiapi Return the application's globally unique id.
     * @tiresult[String] The application's globally unique id.
     */
    this->SetMethod("getGUID", &Application::GetGUID);

    /**
     * @tiapi(method=True,name=App.getStreamURL,since=0.4)
     * @tiapi Return the stream URL for the application's updates.
     * @tiarg[String, ...] Any number of String arguments which will be
     * @tiarg appended as path components of the stream URL.
     * @tiresult[String] The stream URL for application updates.
     */
    this->SetMethod("getStreamURL", &Application::GetStreamURL);

    /**
     * @tiapi(method=True,type=String,name=App.getPath,since=0.8)
     * @tiapi Return the full path to the application executable.
     * @tiresult[String] The full path to the application executable.
     */
    this->SetMethod("getPath", &Application::GetPath);

    /**
     * @tiapi(method=True,type=String,name=App.getHome,since=0.8)
     * @tiapi Return the full path to the application home directory.
     * @tiresult[String] The full path to the application home directory.
     */
    this->SetMethod("getHome", &Application::GetHome);

    /**
     * @tiapi(method=True,type=String,name=App.getArguments,since=0.8)
     * @tiapi Return the command-line arguments passed to this application,
     * @tiapi excluding the first -- which is generally the path to the
     * @tiapi application executable.
     * @tiresult[Array<String>] The arguments passed to this application.
     */
    this->SetMethod("getArguments", &Application::GetArguments);

    /**
     * @tiapi(method=True,name=App.appURLToPath,since=0.2)
     * @tiapi Return the full path equivalent of an app: protocol path
     * @tiresult(for=App.appURLToPath,type=String) returns the path
     */
    this->SetMethod("appURLToPath", &Application::AppURLToPath);

    /**
     * @tiapi(method=True,name=App.exit,since=0.2)
     * @tiapi Exit the application.
     */
    this->SetMethod("exit", &Application::Exit);

    /**
     * @tiapi(method=True,name=App.restart,since=0.9)
     * @tiapi Restart the application.
     */
    this->SetMethod("restart", &Application::Restart);

    /**
     * @tiapi(method=True,name=App.createProperties,since=0.6)
     * @tiapi create a new properties object
     * @tiarg[App.Properties, properties, optional=true]
     * @tiarg Initial properties for the new Properties object.
     * @tiresult[App.Properties] A new Properties instance.
     */
    this->SetMethod("createProperties", &Application::CreateProperties);

    /**
     * @tiapi(method=True,name=App.loadProperties,since=0.2)
     * @tiapi Loads a properties list from a file path
     * @tiarg[String, path] Path to a properties file.
     * @tiresult[Array<App.Properties>] A list of properties.
     */
    this->SetMethod("loadProperties", &Application::LoadProperties);

    /**
     * @tiapi(method=True,name=App.stdout,since=0.4) Writes to stdout
     * @tiarg(for=App.stdout,type=any,name=data) data to write
     */
    this->SetMethod("stdout", &Application::StdOut);

    /**
     * @tiapi(method=True,name=App.stderr,since=0.4) Writes to stderr
     * @tiarg(for=App.stderr,type=any,name=data) data to write
     */
    this->SetMethod("stderr", &Application::StdErr);

    /**
     * @tiapi(method=True,name=App.stdin,since=0.7) Reads from stdin
     * @tiarg(for=App.stdin,type=String,name=prompt,optional=True) text prompt (Default: no prompt)
     * @tiarg(for=App.stdin,type=String,name=delimiter,optional=True) Will continue reading stdin until the delimiter character is reached. (Default: newline)
     * @tiresult(for=App.stderr,type=String) data read from stdin
     */
    this->SetMethod("stdin", &Application::StdIn);
    
    /**
     * @tiapi(method=True,name=App.getSystemProperties,since=0.4)
     * @tiapi get the system properties defined in tiapp.xml
     * @tiapi (see App.Properties).
     * @tiresult[type=App.Properties] The system properties object 
     */
    this->SetMethod("getSystemProperties", &Application::GetSystemProperties);

    /**
     * @tiapi(method=True,name=App.getIcon,since=0.4)
     * @tiapi Return the path to the application icon.
     * @tiresult[String] The application icon path.
     */
    this->SetMethod("getIcon", &Application::GetIcon);

    // Don't document these temporary API points. This will be replaced by
    // a generic method of reading arbitrary property values from tiapp.xml.
    this->SetBool("analyticsEnabled",
        ApplicationConfig::Instance()->IsAnalyticsEnabled());
    this->SetBool("updateMonitorEnabled",
        ApplicationConfig::Instance()->IsUpdateMonitorEnabled());
        
    this->Setup();  
}

Application::~Application()
{
}

void Application::GetID(const ValueList& args, KValueRef result)
{
    result->SetString(ApplicationConfig::Instance()->GetAppID().c_str());
}
void Application::GetName(const ValueList& args, KValueRef result)
{
    result->SetString(ApplicationConfig::Instance()->GetAppName().c_str());
}
void Application::GetVersion(const ValueList& args, KValueRef result)
{
    result->SetString(ApplicationConfig::Instance()->GetVersion().c_str());
}
void Application::GetPublisher(const ValueList& args, KValueRef result)
{
    result->SetString(ApplicationConfig::Instance()->GetPublisher().c_str());
}
void Application::GetCopyright(const ValueList& args, KValueRef result)
{
    result->SetString(ApplicationConfig::Instance()->GetCopyright().c_str());
}
void Application::GetDescription(const ValueList& args, KValueRef result)
{
    result->SetString(ApplicationConfig::Instance()->GetDescription().c_str());
}
void Application::GetURL(const ValueList& args, KValueRef result)
{
    result->SetString(ApplicationConfig::Instance()->GetURL().c_str());
}
void Application::GetGUID(const ValueList& args, KValueRef result)
{
    std::string guid = host->GetApplication()->guid;
    result->SetString(guid);
}

void Application::Exit(const ValueList& args, KValueRef result)
{
    host->Exit(args.GetInt(0, 0));
}

void Application::AppURLToPath(const ValueList& args, KValueRef result)
{
    args.VerifyException("appURLToPath", "s");
    std::string url = args.GetString(0);
    if (url.find("app://") != 0 && url.find("://") == std::string::npos)
    {
        url = std::string("app://") + url;
    }
    std::string path = URLUtils::URLToPath(url);
    result->SetString(path);
}

void Application::CreateProperties(const ValueList& args, KValueRef result)
{
    AutoPtr<Properties> properties = new Properties();
    result->SetObject(properties);
    
    if (args.size() > 0 && args.at(0)->IsObject())
    {
        KObjectRef p = args.at(0)->ToObject();
        SharedStringList names = p->GetPropertyNames();
        for (size_t i = 0; i < names->size(); i++)
        {
            KValueRef value = p->Get(names->at(i));
            ValueList setterArgs;
            setterArgs.push_back(Value::NewString(names->at(i)));
            setterArgs.push_back(value);
            Properties::Type type;
            
            if (value->IsList()) type = Properties::List;
            else if (value->IsInt()) type = Properties::Int;
            else if (value->IsDouble()) type = Properties::Double;
            else if (value->IsBool()) type = Properties::Bool;
            else type = Properties::String;
            
            properties->Setter(setterArgs, type);
        }
    }
}

void Application::LoadProperties(const ValueList& args, KValueRef result)
{
    if (args.size() >= 1 && args.at(0)->IsString()) {
        std::string file_path = args.at(0)->ToString();
        KObjectRef properties = new Properties(file_path);
        result->SetObject(properties);
    }
}

void Application::GetSystemProperties(const ValueList& args, KValueRef result)
{
    result->SetObject(ApplicationConfig::Instance()->GetSystemProperties());
}

void Application::StdOut(const ValueList& args, KValueRef result)
{
    for (size_t c=0; c < args.size(); c++)
    {
        KValueRef arg = args.at(c);
        if (arg->IsString())
        {
            const char *s = arg->ToString();
            std::cout << s;
        }
        else
        {
            SharedString ss = arg->DisplayString();
            std::cout << *ss;
        }
    }
    std::cout << std::endl;
}

void Application::StdErr(const ValueList& args, KValueRef result)
{
    for (size_t c = 0; c < args.size(); c++)
    {
        KValueRef arg = args.at(c);
        if (arg->IsString())
        {
            const char *s = arg->ToString();
            std::cerr << s;
        }
        else
        {
            SharedString ss = arg->DisplayString();
            std::cerr << *ss;
        }
    }
    std::cerr << std::endl;
}

void Application::StdIn(const ValueList& args, KValueRef result)
{
    args.VerifyException("stdin", "?ss");
    std::string input;
    char delimiter = '\n';

    if (args.size() >= 1)
    {
        std::cout << args.GetString(0);
    }

    if (args.size() >= 2)
    {
        delimiter = args.GetString(1).at(0);
    }

    getline(std::cin, input, delimiter);
    result->SetString(input);
}

void Application::GetStreamURL(const ValueList& args, KValueRef result)
{
    SharedApplication app = this->host->GetApplication();
    std::string url(app->GetStreamURL("https"));

    for (size_t c = 0; c < args.size(); c++)
    {
        KValueRef arg = args.at(c);
        if (arg->IsString())
        {
            url.append("/");
            url.append(arg->ToString());
        }
    }
    result->SetString(url);
}

void Application::GetIcon(const ValueList& args, KValueRef result)
{
    SharedApplication app = this->host->GetApplication();
    result->SetNull();  

    if (app && !app->image.empty())
    {
        result->SetString(app->image);
    }
}

void Application::GetPath(const ValueList& args, KValueRef result)
{
    result->SetString(host->GetApplication()->GetArguments().at(0).c_str());
}

void Application::GetHome(const ValueList& args, KValueRef result)
{
    result->SetString(host->GetApplication()->path);
}

void Application::GetArguments(const ValueList& args, KValueRef result)
{
    static KListRef argList(0);
    if (argList.isNull())
    {
        // Skip the first argument which is the filename of the executable.
        std::vector<std::string>& args = host->GetApplication()->GetArguments();
        argList = new StaticBoundList();
        for (size_t i = 1; i < args.size(); i++)
            argList->Append(Value::NewString(args.at(i)));
    }

    result->SetList(argList);
}

} // namespace Titanium
