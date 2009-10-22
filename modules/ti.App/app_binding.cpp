/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */
#include <kroll/kroll.h>
#include <Poco/Environment.h>
#include "app_binding.h"
#include "app_config.h"
#include "Properties/properties_binding.h"

namespace ti
{
	AppBinding::AppBinding(Host* host, KObjectRef global) :
		KAccessorObject("App"),
		host(host),
		global(global)
	{
		/**
		 * @tiapi(method=Truename=App.getID,since=0.2)
		 * @tiapi Return the application id.
		 * @tiresult[String] The application id.
		 */
		this->SetMethod("getID", &AppBinding::GetID);

		/**
		 * @tiapi(method=True,name=App.getName,since=0.2)
		 * @tiapi Return the application name.
		 * @tiresult[String] The application name.
		 */
		this->SetMethod("getName", &AppBinding::GetName);

		/**
		 * @tiapi(method=True,name=App.getVersion,since=0.2)
		 * @tiapi Return the application version.
		 * @tiresult[String] The application version.
		 */
		this->SetMethod("getVersion", &AppBinding::GetVersion);

		/**
		 * @tiapi(method=True,name=App.getPublisher,since=0.4)
		 * @tiapi Return the application publisher.
		 * @tiresult[String] The application publisher.
		 */
		this->SetMethod("getPublisher", &AppBinding::GetPublisher);

		/**
		 * @tiapi(method=True,name=App.getURL,since=0.4)
		 * @tiapi Return the application URL.
		 * @tiresult[String] The application URL.
		 */
		this->SetMethod("getURL", &AppBinding::GetURL);

		/**
		 * @tiapi(method=True,name=App.getDescription,since=0.4)
		 * @tiapi Return the application description.
		 * @tiresult[String] The application description.
		 */
		this->SetMethod("getDescription", &AppBinding::GetDescription);

		/**
		 * @tiapi(method=True,name=App.getCopyright,since=0.4)
		 * @tiapi Return the application's copyright information
		 * @tiresult[String] The application copyright.
		 */
		this->SetMethod("getCopyright", &AppBinding::GetCopyright);

		/**
		 * @tiapi(method=True,name=App.getGUID,since=0.2)
		 * @tiapi Return the application's globally unique id.
		 * @tiresult[String] The application's globally unique id.
		 */
		this->SetMethod("getGUID", &AppBinding::GetGUID);

		/**
		 * @tiapi(method=True,name=App.getStreamURL,since=0.4)
		 * @tiapi Return the stream URL for the application's updates.
		 * @tiarg[String, ...] Any number of String arguments which will be
		 * @tiarg appended as path components of the stream URL.
		 * @tiresult[String] The stream URL for application updates.
		 */
		this->SetMethod("getStreamURL", &AppBinding::GetStreamURL);

		/**
		 * @tiapi(method=True,type=String,name=App.getPath,since=0.8)
		 * @tiapi Return the full path to the application executable.
		 * @tiresult[String] The full path to the application executable.
		 */
		this->SetMethod("getPath", &AppBinding::GetPath);

		/**
		 * @tiapi(method=True,type=String,name=App.getHome,since=0.8)
		 * @tiapi Return the full path to the application home directory.
		 * @tiresult[String] The full path to the application home directory.
		 */
		this->SetMethod("getHome", &AppBinding::GetHome);

		/**
		 * @tiapi(method=True,type=String,name=App.getArguments,since=0.8)
		 * @tiapi Return the command-line arguments passed to this application,
		 * @tiapi excluding the first -- which is generally the path to the
		 * @tiapi application executable.
		 * @tiresult[Array<String>] The arguments passed to this application.
		 */
		this->SetMethod("getArguments", &AppBinding::GetArguments);

		/**
		 * @tiapi(method=True,name=App.appURLToPath,since=0.2)
		 * @tiapi Return the full path equivalent of an app: protocol path
		 * @tiresult(for=App.appURLToPath,type=String) returns the path
		 */
		this->SetMethod("appURLToPath", &AppBinding::AppURLToPath);

		/**
		 * @tiapi(method=True,name=App.exit,since=0.2)
		 * @tiapi Exit the application.
		 */
		this->SetMethod("exit",&AppBinding::Exit);

		/**
		 * @tiapi(method=True,name=App.createProperties,since=0.6)
		 * @tiapi create a new properties object
		 * @tiarg[App.Properties, properties, optional=true]
		 * @tiarg Initial properties for the new Properties object.
		 * @tiresult[App.Properties] A new Properties instance.
		 */
		this->SetMethod("createProperties", &AppBinding::CreateProperties);

		/**
		 * @tiapi(method=True,name=App.loadProperties,since=0.2)
		 * @tiapi Loads a properties list from a file path
		 * @tiarg[String, path] Path to a properties file.
		 * @tiresult[Array<App.Properties>] A list of properties.
		 */
		this->SetMethod("loadProperties", &AppBinding::LoadProperties);

		/**
		 * @tiapi(method=True,name=App.stdout,since=0.4) Writes to stdout
		 * @tiarg(for=App.stdout,type=any,name=data) data to write
		 */
		this->SetMethod("stdout", &AppBinding::StdOut);

		/**
		 * @tiapi(method=True,name=App.stderr,since=0.4) Writes to stderr
		 * @tiarg(for=App.stderr,type=any,name=data) data to write
		 */
		this->SetMethod("stderr", &AppBinding::StdErr);

		/**
		 * @tiapi(method=True,name=App.getSystemProperties,since=0.4)
		 * @tiapi get the system properties defined in tiapp.xml
		 * @tiapi (see App.Properties).
		 * @tiresult[type=App.Properties] The system properties object 
		 */
		this->SetMethod("getSystemProperties", &AppBinding::GetSystemProperties);

		/**
		 * @tiapi(method=True,name=App.getIcon,since=0.4)
		 * @tiapi Return the path to the application icon.
		 * @tiresult[String] The application icon path.
		 */
		this->SetMethod("getIcon", &AppBinding::GetIcon);
	}

	AppBinding::~AppBinding()
	{
	}
	void AppBinding::GetID(const ValueList& args, KValueRef result)
	{
		result->SetString(AppConfig::Instance()->GetAppID().c_str());
	}
	void AppBinding::GetName(const ValueList& args, KValueRef result)
	{
		result->SetString(AppConfig::Instance()->GetAppName().c_str());
	}
	void AppBinding::GetVersion(const ValueList& args, KValueRef result)
	{
		result->SetString(AppConfig::Instance()->GetVersion().c_str());
	}
	void AppBinding::GetPublisher(const ValueList& args, KValueRef result)
	{
		result->SetString(AppConfig::Instance()->GetPublisher().c_str());
	}
	void AppBinding::GetCopyright(const ValueList& args, KValueRef result)
	{
		result->SetString(AppConfig::Instance()->GetCopyright().c_str());
	}
	void AppBinding::GetDescription(const ValueList& args, KValueRef result)
	{
		result->SetString(AppConfig::Instance()->GetDescription().c_str());
	}
	void AppBinding::GetURL(const ValueList& args, KValueRef result)
	{
		result->SetString(AppConfig::Instance()->GetURL().c_str());
	}
	void AppBinding::GetGUID(const ValueList& args, KValueRef result)
	{
		std::string guid = host->GetApplication()->guid;
		result->SetString(guid);
	}
	void AppBinding::Exit(const ValueList& args, KValueRef result)
	{
		host->Exit(args.size()==0 ? 0 : args.at(0)->ToInt());
	}

	void AppBinding::AppURLToPath(const ValueList& args, KValueRef result)
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

	void AppBinding::CreateProperties(const ValueList& args, KValueRef result)
	{
		AutoPtr<PropertiesBinding> properties = new PropertiesBinding();
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
				PropertiesBinding::Type type;
				
				if (value->IsList()) type = PropertiesBinding::List;
				else if (value->IsInt()) type = PropertiesBinding::Int;
				else if (value->IsDouble()) type = PropertiesBinding::Double;
				else if (value->IsBool()) type = PropertiesBinding::Bool;
				else type = PropertiesBinding::String;
				
				properties->Setter(setterArgs, type);
			}
		}
	}

	void AppBinding::LoadProperties(const ValueList& args, KValueRef result)
	{
		if (args.size() >= 1 && args.at(0)->IsString()) {
			std::string file_path = args.at(0)->ToString();
			KObjectRef properties = new PropertiesBinding(file_path);
			result->SetObject(properties);
		}
	}
	
	void AppBinding::GetSystemProperties(const ValueList& args, KValueRef result)
	{
		result->SetObject(AppConfig::Instance()->GetSystemProperties());
	}

	void AppBinding::StdOut(const ValueList& args, KValueRef result)
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

	void AppBinding::StdErr(const ValueList& args, KValueRef result)
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

	void AppBinding::GetStreamURL(const ValueList& args, KValueRef result)
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

	void AppBinding::GetIcon(const ValueList& args, KValueRef result)
	{
		SharedApplication app = this->host->GetApplication();
		result->SetNull();	

		if (app && !app->image.empty())
		{
			result->SetString(app->image);
		}
	}

	void AppBinding::GetPath(const ValueList& args, KValueRef result)
	{
		static std::string path(host->GetCommandLineArg(0));
		result->SetString(path);
	}

	void AppBinding::GetHome(const ValueList& args, KValueRef result)
	{
		static std::string path(host->GetApplicationHomePath());
		result->SetString(path);
	}

	void AppBinding::GetArguments(const ValueList& args, KValueRef result)
	{
		static KListRef argList(0);
		if (argList.isNull())
		{
			// Skip the first argument which is the filename to the executable
			argList = new StaticBoundList();
			for (int i = 1; i < host->GetCommandLineArgCount(); i++)
				argList->Append(Value::NewString(host->GetCommandLineArg(i)));
		}

		result->SetList(argList);
	}
}
