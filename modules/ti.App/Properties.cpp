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

#include "Properties.h"

#include <Poco/StringTokenizer.h>
#include <Poco/File.h>

using Poco::Util::TitaniumPropertyFileConfiguration;

namespace Titanium {

Properties::Properties(const std::string& filePath) :
    StaticBoundObject("App.Properties"),
    logger(Logger::Get("App.Properties")),
    filePath(filePath)
{
    if (!filePath.empty())
    {
        Poco::File file(filePath);
        if (!file.exists()) 
            file.createFile();

        config = new TitaniumPropertyFileConfiguration(filePath);
    }
    else
    {
        config = new TitaniumPropertyFileConfiguration();
    }

    SetMethod("getBool", &Properties::GetBool);
    SetMethod("getDouble", &Properties::GetDouble);
    SetMethod("getInt", &Properties::GetInt);
    SetMethod("getString", &Properties::GetString);
    SetMethod("getList", &Properties::GetList);
    SetMethod("setBool", &Properties::SetBool);
    SetMethod("setDouble", &Properties::SetDouble);
    SetMethod("setInt", &Properties::SetInt);
    SetMethod("setString", &Properties::SetString);
    SetMethod("setList", &Properties::SetList);
    SetMethod("hasProperty", &Properties::HasProperty);
    SetMethod("removeProperty", &Properties::RemoveProperty);
    SetMethod("listProperties", &Properties::ListProperties);
    SetMethod("saveTo", &Properties::SaveTo);
}

void Properties::SaveConfig()
{
    if (!filePath.empty())
        config->save(filePath);
}

void Properties::Getter(const ValueList& args, KValueRef result, Type type)
{
    std::string eprefix = "Properties::Get: ";
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

void Properties::Setter(const ValueList& args, Type type)
{
    std::string eprefix = "Properties::Set: ";
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
                KValueRef result;
                this->SetList(args, result);
            }
            default: break;
        }

        this->SaveConfig();
    }
    catch(Poco::Exception &e)
    {
        throw ValueException::FromString(eprefix + e.displayText());
    }
}

void Properties::GetBool(const ValueList& args, KValueRef result)
{
    args.VerifyException("getBool", "s");
    Getter(args, result, Bool);
}

void Properties::GetDouble(const ValueList& args, KValueRef result)
{
    args.VerifyException("getDouble", "s");
    Getter(args, result, Double);
}

void Properties::GetInt(const ValueList& args, KValueRef result)
{
    args.VerifyException("getInt", "s");
    Getter(args, result, Int);
}

void Properties::GetString(const ValueList& args, KValueRef result)
{
    args.VerifyException("getString", "s");
    Getter(args, result, String);
}

void Properties::GetList(const ValueList& args, KValueRef result)
{
    args.VerifyException("getList", "s");
    KValueRef stringValue = Value::Null;
    GetString(args, stringValue);

    if (!stringValue->IsNull())
    {
        KListRef list = new StaticBoundList();
        std::string string = stringValue->ToString();
        Poco::StringTokenizer t(string, ",", Poco::StringTokenizer::TOK_TRIM);
        for (size_t i = 0; i < t.count(); i++)
        {
            KValueRef token = Value::NewString(t[i].c_str());
            list->Append(token);
        }

        KListRef list2 = list;
        result->SetList(list2);
    }
}

void Properties::SetBool(const ValueList& args, KValueRef result)
{
    args.VerifyException("setBool", "s b");
    Setter(args, Bool);
}

void Properties::SetDouble(const ValueList& args, KValueRef result)
{
    args.VerifyException("setDouble", "s n");
    Setter(args, Double);
}

void Properties::SetInt(const ValueList& args, KValueRef result)
{
    args.VerifyException("setInt", "s n");
    Setter(args, Int);
}

void Properties::SetString(const ValueList& args, KValueRef result)
{
    args.VerifyException("setString", "s s");
    Setter(args, String);
}

void Properties::SetList(const ValueList& args, KValueRef result)
{
    args.VerifyException("setList", "s l");

    std::string property = args.at(0)->ToString();
    KListRef list = args.at(1)->ToList();

    std::string value = "";
    for (unsigned int i = 0; i < list->Size(); i++)
    {
        KValueRef arg = list->At(i);
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
            logger->Warn("Skipping list entry %ui, not a string", i);
        }
    }
    config->setString(property, value);
    this->SaveConfig();
    
}

void Properties::HasProperty(const ValueList& args, KValueRef result)
{
    args.VerifyException("hasProperty", "s");
    result->SetBool(config->hasProperty(args.GetString(0)));
}

void Properties::RemoveProperty(const ValueList& args, KValueRef result)
{
    args.VerifyException("removeProperty", "s");
    result->SetBool(config->removeProperty(args.GetString(0)));
    if (result) {
        this->SaveConfig();
    }
}

void Properties::ListProperties(const ValueList& args, KValueRef result)
{
    std::vector<std::string> keys;
    config->keys(keys);

    KListRef property_list = new StaticBoundList();
    for (size_t i = 0; i < keys.size(); i++)
    {
        std::string property_name = keys.at(i);
        KValueRef name_value = Value::NewString(property_name.c_str());
        property_list->Append(name_value);
    }
    result->SetList(property_list);
}

void Properties::SaveTo(const ValueList& args, KValueRef result)
{
    args.VerifyException("saveTo", "s");

    this->filePath = args.at(0)->ToString();
    this->SaveConfig();
}

} // namespace Titanium
