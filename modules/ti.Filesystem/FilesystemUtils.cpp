/*
 * Copyright (c) 2009-2010 Appcelerator, Inc. All Rights Reserved.
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

#include "FilesystemUtils.h"

#include <Poco/File.h>
#include <Poco/Path.h>
#include <Poco/FileStream.h>
#include <Poco/Exception.h>

namespace Titanium {

std::string FilenameFromValue(KValueRef v)
{
    if (v->IsString())
    {
        return v->ToString();
    }
    else if (v->IsObject())
    {
        return *(v->ToObject()->DisplayString());
    }
    else
    {
        throw ValueException::FromFormat(
            "Can't convert object of type %s to filename", v->GetType().c_str());
    }
}

std::string FilenameFromArguments(const ValueList& args)
{
    std::string filename;
    if (args.at(0)->IsList())
    {
        // you can pass in an array of parts to join
        KListRef list(args.GetList(0));
        for (size_t c = 0; c < list->Size(); c++)
        {
            filename = kroll::FileUtils::Join(
                filename.c_str(), list->At(c)->ToString(), NULL);
        }
    }
    else
    {
        // you can pass in vararg of strings which acts like  a join
        for (size_t c = 0; c < args.size(); c++)
        {
            filename = kroll::FileUtils::Join(filename.c_str(),
                FilenameFromValue(args.at(c)).c_str(), 0);
        }
    }

    if (filename.empty())
        throw ValueException::FromString("Could not construct filename from arguments");

    return filename;
}

} // namespace Titanium
