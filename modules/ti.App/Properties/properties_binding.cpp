/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */

#include "properties_binding.h"
#include <Poco/StringTokenizer.h>
#include <Poco/File.h>

using namespace kroll;

namespace ti
{
	PropertiesBinding::PropertiesBinding(std::string& file_path) :
		StaticBoundObject("Properties"),
		logger(Logger::Get("App.Properties"))
	{
		Poco::File file(file_path);
		if (!file.exists()) 
		{
			file.createFile();
		}

		config = new Poco::Util::PropertyFileConfiguration(file_path);
		this->file_path = file_path.c_str();
		
		this->Init();
	}
	
	PropertiesBinding::PropertiesBinding() :
		StaticBoundObject("Properties")
	{
		this->file_path = "";
		this->config = new Poco::Util::PropertyFileConfiguration();
		
		this->Init();
	}
	
	void PropertiesBinding::Init()
	{
		/**
		 * @tiapi(method=True,name=App.Properties.getBool,since=0.2) Returns a property value as boolean
		 * @tiarg[string, name] The name of the property to return
		 * @tiarg[bool, default] The value to return if this property is not set
		 * @tiresult[bool] The value of the property with the given name
		 */
		SetMethod("getBool", &PropertiesBinding::GetBool);
		/**
		 * @tiapi(method=True,name=App.Properties.getDouble,since=0.2) Returns a property value as double
		 * @tiarg[string, name] The name of the property to return
		 * @tiarg[double, default] The value to return if this property is not set
		 * @tiresult[double] The value of the property with the given name
		 */
		SetMethod("getDouble", &PropertiesBinding::GetDouble);
		/**
		 * @tiapi(method=True,name=App.Properties.getInt,since=0.2) Returns a property value as integer
		 * @tiarg[string, name] The name of the property to return
		 * @tiarg[int, default] The value to return if this property is not set
		 * @tiresult[int] The value of the property with the given name
		 */
		SetMethod("getInt", &PropertiesBinding::GetInt);
		/**
		 * @tiapi(method=True,name=App.Properties.getString,since=0.2) Returns a property value as string
		 * @tiarg[string, name] The name of the property to return
		 * @tiarg[string, default] The value to return if this property is not set
		 * @tiresult[string] The value of the property with the given name
		 */
		SetMethod("getString", &PropertiesBinding::GetString);
		/**
		 * @tiapi(method=True,name=App.Properties.getList,since=0.2) Returns a property value as a list
		 * @tiarg[list, name] The name of the property to return
		 * @tiarg[list, default] The value to return if this property is not set
		 * @tiresult[list] The value of the property with the given name
		 */
		SetMethod("getList", &PropertiesBinding::GetList);
		/**
		 * @tiapi(method=True,name=App.Properties.setBool,since=0.2) Sets a boolean property value
		 * @tiarg(for=App.Properties.setBool,name=name,type=string) the property name
		 * @tiarg(for=App.Properties.setBool,name=value,type=boolean) the value
		 */
		SetMethod("setBool", &PropertiesBinding::SetBool);
		/**
		 * @tiapi(method=True,name=App.Properties.setDouble,since=0.2) Sets a double property value
		 * @tiarg(for=App.Properties.setDouble,name=name,type=string) the property name
		 * @tiarg(for=App.Properties.setDouble,name=value,type=double) the value
		 */
		SetMethod("setDouble", &PropertiesBinding::SetDouble);
		/**
		 * @tiapi(method=True,name=App.Properties.setInt,since=0.2) Sets an integer property value
		 * @tiarg(for=App.Properties.setInt,name=name,type=string) the property name
		 * @tiarg(for=App.Properties.setInt,name=value,type=integer) the value
		 */
		SetMethod("setInt", &PropertiesBinding::SetInt);
		/**
		 * @tiapi(method=True,name=App.Properties.setString,since=0.2) Sets a string property value
		 * @tiarg(for=App.Properties.setString,name=name,type=string) the property name
		 * @tiarg(for=App.Properties.setString,name=value,type=string) the value
		 */
		SetMethod("setString", &PropertiesBinding::SetString);
		/**
		 * @tiapi(method=True,name=App.Properties.setList,since=0.2) Sets a list property value
		 * @tiarg(for=App.Properties.setList,name=name,type=string) the property name
		 * @tiarg(for=App.Properties.setList,name=value,type=list) the value
		 */
		SetMethod("setList", &PropertiesBinding::SetList);
		/**
		 * @tiapi(method=True,name=App.Properties.hasProperty,since=0.2) Checks whether a property exists
		 * @tiarg(for=App.Properties.hasProperty,name=name,type=string) the property name
		 * @tiresult(for=App.Properties.hasProperty,type=boolean) returns true if the property exists
		 */
		SetMethod("hasProperty", &PropertiesBinding::HasProperty);
		/**
		 * @tiapi(method=True,name=App.Properties.listProperties,since=0.2) Returns a list of property values
		 * @tiresult(for=App.Properties.listProperties,type=list) returns a list of property values
		 */
		SetMethod("listProperties", &PropertiesBinding::ListProperties);
		/**
		 * @tiapi(method=True,name=App.Properties.saveTo,since=0.2) save this properties object to a file
		 * @tiarg(for=App.Properties.saveTo,name=filename,type=string) the filename
		 */
		SetMethod("saveTo", &PropertiesBinding::SaveTo);
	
	}

	PropertiesBinding::~PropertiesBinding()
	{
		if (file_path.size() > 0)
		{
			config->save(file_path);
		}
	}

	void PropertiesBinding::Getter(const ValueList& args, SharedValue result, Type type)
	{
		if (args.size() > 0 && args.at(0)->IsString())
		{
			std::string eprefix = "PropertiesBinding::Get: ";
			try
			{
				std::string property = args.at(0)->ToString();
				if (args.size() == 1)
				{
					switch (type)
					{
						case Bool: 
							result->SetBool(config->getBool(property));
							break;
						case Double:
							result->SetDouble(config->getDouble(property));
							break;
						case Int:
							result->SetInt(config->getInt(property));
							break;
						case String:
							result->SetString(config->getString(property).c_str());
							break;
						default:
							break;
					}
					return;
				}

				else if (args.size() >= 2)
				{
					switch (type)
					{
						case Bool:
							result->SetBool(config->getBool(property, args.at(1)->ToBool()));
							break;
						case Double:
							result->SetDouble(config->getDouble(property, args.at(1)->ToDouble()));
							break;
						case Int:
							result->SetInt(config->getInt(property, args.at(1)->ToInt()));
							break;
						case String:
							result->SetString(config->getString(property, args.at(1)->ToString()).c_str());
							break;
						default: break;
					}
					return;
				}
			}
			catch(Poco::Exception &e)
			{
				throw ValueException::FromString(eprefix + e.displayText());
			}
		}
	}

	void PropertiesBinding::Setter(const ValueList& args, Type type)
	{
		if (args.size() >= 2 && args.at(0)->IsString())
		{
			std::string eprefix = "PropertiesBinding::Set: ";
			try
			{
				std::string property = args.at(0)->ToString();
				switch (type)
				{
					case Bool:
						config->setBool(property, args.at(1)->ToBool());
						break;
					case Double:
						config->setDouble(property, args.at(1)->ToDouble());
						break;
					case Int:
						config->setInt(property, args.at(1)->ToInt());
						break;
					case String:
						config->setString(property, args.at(1)->ToString());
						break;
					case List: {
						SharedValue result;
						this->SetList(args, result);
					}
					default: break;
				}

				if (file_path.size() > 0)
				{
					config->save(file_path);
				}
			}
			catch(Poco::Exception &e)
			{
				throw ValueException::FromString(eprefix + e.displayText());
			}
		}
	}

	void PropertiesBinding::GetBool(const ValueList& args, SharedValue result)
	{
		Getter(args, result, Bool);
	}

	void PropertiesBinding::GetDouble(const ValueList& args, SharedValue result)
	{
		Getter(args, result, Double);
	}

	void PropertiesBinding::GetInt(const ValueList& args, SharedValue result)
	{
		Getter(args, result, Int);
	}

	void PropertiesBinding::GetString(const ValueList& args, SharedValue result)
	{
		Getter(args, result, String);
	}

	void PropertiesBinding::GetList(const ValueList& args, SharedValue result)
	{
		SharedValue stringValue = Value::Null;
		GetString(args, stringValue);

		if (!stringValue->IsNull())
		{
			SharedKList list = new StaticBoundList();
			std::string string = stringValue->ToString();
			Poco::StringTokenizer t(string, ",", Poco::StringTokenizer::TOK_TRIM);
			for (size_t i = 0; i < t.count(); i++)
			{
				SharedValue token = Value::NewString(t[i].c_str());
				list->Append(token);
			}

			SharedKList list2 = list;
			result->SetList(list2);
		}
	}

	void PropertiesBinding::SetBool(const ValueList& args, SharedValue result)
	{
		Setter(args, Bool);
	}

	void PropertiesBinding::SetDouble(const ValueList& args, SharedValue result)
	{
		Setter(args, Double);
	}

	void PropertiesBinding::SetInt(const ValueList& args, SharedValue result)
	{
		Setter(args, Int);
	}

	void PropertiesBinding::SetString(const ValueList& args, SharedValue result)
	{
		Setter(args, String);
	}

	void PropertiesBinding::SetList(const ValueList& args, SharedValue result)
	{
		if (args.size() >= 2 && args.at(0)->IsString() && args.at(1)->IsList())
		{
			std::string property = args.at(0)->ToString();
			SharedKList list = args.at(1)->ToList();

			std::string value = "";
			for (unsigned int i = 0; i < list->Size(); i++)
			{
				SharedValue arg = list->At(i);
				if (arg->IsString())
				{
					value += list->At(i)->ToString();
					if (i < list->Size() - 1)
					{
						value += ",";
					}
				}
				else
				{
					std::cerr << "skipping object: " << arg->GetType() << std::endl;
				}
			}
			config->setString(property, value);
			if (file_path.size() > 0)
			{
				config->save(file_path);
			}
		}
	}

	void PropertiesBinding::HasProperty(const ValueList& args, SharedValue result)
	{
		result->SetBool(false);

		if (args.size() >= 1 && args.at(0)->IsString())
		{
			std::string property = args.at(0)->ToString();
			result->SetBool(config->hasProperty(property));
		}
	}

	void PropertiesBinding::ListProperties(const ValueList& args, SharedValue result)
	{
		std::vector<std::string> keys;
		config->keys(keys);

		SharedKList property_list = new StaticBoundList();
		for (size_t i = 0; i < keys.size(); i++)
		{
			std::string property_name = keys.at(i);
			SharedValue name_value = Value::NewString(property_name.c_str());
			property_list->Append(name_value);
		}
		result->SetList(property_list);
	}
	
	void PropertiesBinding::SaveTo(const ValueList& args, SharedValue result)
	{
		if (args.size() == 0 || !args.at(0)->IsString())
		{
			throw ValueException::FromString("Error saving properties, no filename given (or filename wasn't a string)");
		}
		
		std::string filename = args.at(0)->ToString();
		this->file_path = filename;
		config->save(this->file_path);
	}
}
