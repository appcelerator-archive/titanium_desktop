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
	AppBinding::AppBinding(Host *host, SharedKObject global) : host(host), global(global)
	{
		/**
		 * @tiapi(method=True,immutable=True,name=App.getID,since=0.2) Returns the application id
		 * @tiresult(for=App.getID,type=String) returns the id
		 */
		this->SetMethod("getID", &AppBinding::GetID);
		/**
		 * @tiapi(method=True,immutable=True,name=App.getName,since=0.2) Returns the application name
		 * @tiresult(for=App.getName,type=String) returns the name
		 */
		this->SetMethod("getName", &AppBinding::GetName);
		/**
		 * @tiapi(method=True,immutable=True,name=App.getVersion,since=0.2) Returns the application version
		 * @tiresult(for=App.getVersion,type=String) returns the version
		 */
		this->SetMethod("getVersion", &AppBinding::GetVersion);
		/**
		 * @tiapi(method=True,immutable=True,name=App.getPublisher,since=0.4) Returns the application publisher
		 * @tiresult(for=App.getPublisher,type=String) returns the publisher
		 */
		this->SetMethod("getPublisher", &AppBinding::GetPublisher);
		/**
		 * @tiapi(method=True,immutable=True,name=App.getURL,since=0.4) Returns the application url
		 * @tiresult(for=App.getURL,type=String) returns the url for the app
		 */
		this->SetMethod("getURL", &AppBinding::GetURL);
		/**
		 * @tiapi(method=True,immutable=True,name=App.getDescription,since=0.4) Returns the application description
		 * @tiresult(for=App.getDescription,type=String) returns the description for the app
		 */
		this->SetMethod("getDescription", &AppBinding::GetDescription);
		/**
		 * @tiapi(method=True,immutable=True,name=App.getCopyright,since=0.4) Returns the application copyright information
		 * @tiresult(for=App.getCopyright,type=String) returns the copyright for the app
		 */
		this->SetMethod("getCopyright", &AppBinding::GetCopyright);
		/**
		 * @tiapi(method=True,immutable=True,name=App.getGUID,since=0.2) Returns the application globally unique id
		 * @tiresult(for=App.getGUID,type=String) returns the unique id
		 */
		this->SetMethod("getGUID", &AppBinding::GetGUID);
		/**
		 * @tiapi(method=True,immutable=True,name=App.getStreamURL,since=0.4) Returns the application stream URL for the update channel
		 * @tiresult(for=App.getStreamURL,type=String) returns the stream URL
		 */
		this->SetMethod("getStreamURL", &AppBinding::GetStreamURL);

		
		/**
		 * @tiapi(method=True,immutable=True,name=App.appURLToPath,since=0.2) Returns the full path equivalent of an app: protocol path
		 * @tiresult(for=App.appURLToPath,type=String) returns the path
		 */
		this->SetMethod("appURLToPath", &AppBinding::AppURLToPath);
		
		/**
		 * @tiapi(property=True,immutable=True,type=String,name=App.path,since=0.2) Returns the full path to the application
		 */
		this->Set("path",Value::NewString(host->GetCommandLineArg(0)));

		/**
		 * @tiapi(property=True,immutable=True,type=String,name=App.home,since=0.4) Returns the full path to the application home directory
		 */
		this->Set("home",Value::NewString(host->GetApplicationHomePath()));


		/**
		 * @tiapi(property=True,immutable=True,type=String,name=version,since=0.2) The Titanium product version
		 */
		SharedValue version = Value::NewString(STRING(PRODUCT_VERSION));
		global->Set("version", version);

		/**
		 * @tiapi(property=True,immutable=True,type=String,name=platform,since=0.2) The Titanium platform
		 */
		SharedValue platform = Value::NewString(host->GetPlatform());
		global->Set("platform",platform);

		// skip the first argument which is the filepath to the
		// executable
		SharedKList argList = new StaticBoundList();
		for (int i = 1; i < Host::GetInstance()->GetCommandLineArgCount(); i++) {
			argList->Append(Value::NewString(Host::GetInstance()->GetCommandLineArg(i)));
		}
		SharedValue arguments = Value::NewList(argList);
		/**
		 * @tiapi(property=True,immutable=True,type=array<String>,name=App.arguments,since=0.2) The command line arguments
		 */
		Set("arguments", arguments);

		/**
		 * @tiapi(method=True,immutable=True,name=App.exit,since=0.2) Exits the application
		 */
		this->SetMethod("exit",&AppBinding::Exit);

		/**
		 * @tiapi(method=True,name=App.createProperties,since=0.6) create a new properties object
		 * @tiarg(for=App.createProperties,type=App.Properties,name=initialProperties,optional=True) optional initial properties
		 * @tiresult(for=App.createProperties,type=App.Properties) returns a new properties instance
		 */
		this->SetMethod("createProperties", &AppBinding::CreateProperties);
		
		/**
		 * @tiapi(method=True,name=App.loadProperties,since=0.2) Loads a properties list from a file path
		 * @tiarg(for=App.loadProperties,type=String,name=path) path to properties file
		 * @tiresult(for=App.loadProperties,type=Array<App.Properties>) returns the properties as a list
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
		 * @tiapi(method=True,name=App.getSystemProperties,since=0.4) get the system properties defined in tiapp.xml
		 * @tiresult(for=App.getSystemProperties,type=App.Properties) returns the system properties object (see Titanium.App.Properties)
		 */
		this->SetMethod("getSystemProperties", &AppBinding::GetSystemProperties);

		/**
		 * @tiapi(method=True,name=App.getIcon,since=0.4) Returns the application icon
		 * @tiresult(for=App.getIcon,type=String) returns the icon path
		 */
		this->SetMethod("getIcon", &AppBinding::GetIcon);
	}

	AppBinding::~AppBinding()
	{
	}
	void AppBinding::GetID(const ValueList& args, SharedValue result)
	{
		result->SetString(AppConfig::Instance()->GetAppID().c_str());
	}
	void AppBinding::GetName(const ValueList& args, SharedValue result)
	{
		result->SetString(AppConfig::Instance()->GetAppName().c_str());
	}
	void AppBinding::GetVersion(const ValueList& args, SharedValue result)
	{
		result->SetString(AppConfig::Instance()->GetVersion().c_str());
	}
	void AppBinding::GetPublisher(const ValueList& args, SharedValue result)
	{
		result->SetString(AppConfig::Instance()->GetPublisher().c_str());
	}
	void AppBinding::GetCopyright(const ValueList& args, SharedValue result)
	{
		result->SetString(AppConfig::Instance()->GetCopyright().c_str());
	}
	void AppBinding::GetDescription(const ValueList& args, SharedValue result)
	{
		result->SetString(AppConfig::Instance()->GetDescription().c_str());
	}
	void AppBinding::GetURL(const ValueList& args, SharedValue result)
	{
		result->SetString(AppConfig::Instance()->GetURL().c_str());
	}
	void AppBinding::GetGUID(const ValueList& args, SharedValue result)
	{
		std::string guid = host->GetApplication()->guid;
		result->SetString(guid);
	}
	void AppBinding::Exit(const ValueList& args, SharedValue result)
	{
		host->Exit(args.size()==0 ? 0 : args.at(0)->ToInt());
	}

	void AppBinding::AppURLToPath(const ValueList& args, SharedValue result)
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

	void AppBinding::CreateProperties(const ValueList& args, SharedValue result)
	{
		AutoPtr<PropertiesBinding> properties = new PropertiesBinding();
		result->SetObject(properties);
		
		if (args.size() > 0 && args.at(0)->IsObject())
		{
			SharedKObject p = args.at(0)->ToObject();
			SharedStringList names = p->GetPropertyNames();
			for (size_t i = 0; i < names->size(); i++)
			{
				SharedValue value = p->Get(names->at(i));
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

	void AppBinding::LoadProperties(const ValueList& args, SharedValue result)
	{
		if (args.size() >= 1 && args.at(0)->IsString()) {
			std::string file_path = args.at(0)->ToString();
			SharedKObject properties = new PropertiesBinding(file_path);
			result->SetObject(properties);
		}
	}
	
	void AppBinding::GetSystemProperties(const ValueList& args, SharedValue result)
	{
		result->SetObject(AppConfig::Instance()->GetSystemProperties());
	}

	void AppBinding::StdOut(const ValueList& args, SharedValue result)
	{
		for (size_t c=0; c < args.size(); c++)
		{
			SharedValue arg = args.at(c);
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

	void AppBinding::StdErr(const ValueList& args, SharedValue result)
	{
		for (size_t c = 0; c < args.size(); c++)
		{
			SharedValue arg = args.at(c);
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

	void AppBinding::GetStreamURL(const ValueList& args, SharedValue result)
	{
		
		SharedApplication app = this->host->GetApplication();
		std::string url(app->GetStreamURL("https"));

		for (size_t c = 0; c < args.size(); c++)
		{
			SharedValue arg = args.at(c);
			if (arg->IsString())
			{
				url.append("/");
				url.append(arg->ToString());
			}
		}
		result->SetString(url);
	}

	void AppBinding::GetIcon(const ValueList& args, SharedValue result)
	{
		SharedApplication app = this->host->GetApplication();
		result->SetNull();	

		if (app && !app->image.empty())
		{
			std::string iconPath = app->image;
			result->SetString(iconPath);
		}
	}
}
