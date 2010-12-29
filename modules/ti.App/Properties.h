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

#ifndef Properties_h
#define Properties_h

#include <kroll/kroll.h>
#include <Poco/AutoPtr.h>

#include "TitaniumPropertyFileConfiguration.h"

namespace Titanium {

class Properties : public kroll::StaticBoundObject
{
public:
    typedef enum { Bool, Double, Int, String, List } Type;

    Properties(const std::string& file_path = "");
    virtual ~Properties() {}

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

} // namespace Titanium

#endif
