/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef PROPERTIES_BINDING_H_
#define PROPERTIES_BINDING_H_

#include <kroll/kroll.h>
#include <string>
#include <vector>
#include <Poco/AutoPtr.h>
#include <Poco/Util/PropertyFileConfiguration.h>
#include "../app_config.h"

using namespace kroll;

namespace ti
{
	class PropertiesBinding : public kroll::StaticBoundObject
	{
		public:
		PropertiesBinding(std::string& file_path);
		PropertiesBinding();
		virtual ~PropertiesBinding();

		void GetBool(const ValueList& args, SharedValue result);
		void GetDouble(const ValueList& args, SharedValue result);
		void GetInt(const ValueList& args, SharedValue result);
		void GetString(const ValueList& args, SharedValue result);
		void GetList(const ValueList& args, SharedValue result);
		void SetBool(const ValueList& args, SharedValue result);
		void SetDouble(const ValueList& args, SharedValue result);
		void SetInt(const ValueList& args, SharedValue result);
		void SetString(const ValueList& args, SharedValue result);
		void SetList(const ValueList& args, SharedValue result);
		void HasProperty(const ValueList& args, SharedValue result);
		void ListProperties(const ValueList& args, SharedValue result);
		void SaveTo(const ValueList& args, SharedValue result);
		
		typedef enum
		{
			Bool, Double, Int, String, List
		} Type;
		void Getter(const ValueList& args, SharedValue result, Type type);
		void Setter(const ValueList& args, Type type);
		
		Poco::AutoPtr<Poco::Util::PropertyFileConfiguration> GetConfig()
		{
			return config;
		}

		protected:
		Logger* logger;

		void Init();

		Poco::AutoPtr<Poco::Util::PropertyFileConfiguration> config;
		std::string file_path;
	};
}

#endif /* PROPERTIES_BINDING_H_ */
