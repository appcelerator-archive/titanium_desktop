/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef TI_APP_CONFIG_UTILS_H_
#define TI_APP_CONFIG_UTILS_H_

#include <string>
#include <algorithm>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>

namespace ti {
	class ConfigUtils
	{
		public:
		static std::string GetNodeValue(xmlNodePtr n)
		{
			xmlChar* strValue = xmlNodeListGetString(n->doc, n->children, 1);
			std::string value = (const char*) strValue;
			xmlFree(strValue);
			return value;
		}

		static std::string GetPropertyValue(xmlNodePtr n, const char* property)
		{
			char* strValue = (char*) xmlGetProp(n, (const xmlChar *) property);
			if (strValue != NULL)
			{
				std::string value = strValue;
				xmlFree(strValue);
				return value;
			}
			return std::string();
		}

		static bool GetNodeValueAsBool(xmlNodePtr n)
		{
			std::string str = GetNodeValue(n);
			return StringToBool(str);
		}

		static bool StringToBool(std::string str)
		{
			std::transform(str.begin(), str.end(), str.begin(), tolower);
			return (str == "yes" || str == "true" || str == "on");
		}
	};
}

#endif
