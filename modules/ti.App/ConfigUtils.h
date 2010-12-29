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

#ifndef ConfigUtils_h
#define ConfigUtils_h

#include <string>
#include <algorithm>

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>

namespace Titanium {

class ConfigUtils {
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

} // namespace Titanium

#endif
