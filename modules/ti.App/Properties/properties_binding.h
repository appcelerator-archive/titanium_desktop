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
		void ListProperties(const ValueList& args, KValueRef result);
		void SaveTo(const ValueList& args, KValueRef result);
		
		typedef enum
		{
			Bool, Double, Int, String, List
		} Type;
		void Getter(const ValueList& args, KValueRef result, Type type);
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
