/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008-2010 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef PROPERTIES_BINDING_H_
#define PROPERTIES_BINDING_H_

#include <kroll/kroll.h>
#include <Poco/AutoPtr.h>
#include "TitaniumPropertyFileConfiguration.h"

namespace ti
{
	class PropertiesBinding : public kroll::StaticBoundObject
	{
	public:
		typedef enum { Bool, Double, Int, String, List } Type;

		PropertiesBinding(const std::string& file_path = "");
		virtual ~PropertiesBinding() {}

		void GetBool(const ValueList& args, KValueRef result);
		void GetDouble(const ValueList& args, KValueRef result);
		void GetInt(const ValueList& args, KValueRef result);
		void GetString(const ValueList& args, KValueRef result);
		void GetList(const ValueList& args, KValueRef result);
		void SetBool(const ValueList& args, KValueRef result);
		void SetDouble(const ValueList& args, KValueRef result);
		void SetInt(const ValueList& args, KValueRef result);
		void SetString(const ValueList& args, KValueRef result);
		void SetList(const ValueList& args, KValueRef result);
		void HasProperty(const ValueList& args, KValueRef result);
		void RemoveProperty(const ValueList& args, KValueRef result);
		void ListProperties(const ValueList& args, KValueRef result);
		void SaveTo(const ValueList& args, KValueRef result);
		void Getter(const ValueList& args, KValueRef result, Type type);
		void Setter(const ValueList& args, Type type);
		void SaveConfig();

		Poco::AutoPtr<Poco::Util::TitaniumPropertyFileConfiguration> GetConfig()
		{
			return config;
		}

	protected:
		Logger* logger;
		std::string filePath;
		Poco::AutoPtr<Poco::Util::TitaniumPropertyFileConfiguration> config;
	};
}

#endif
