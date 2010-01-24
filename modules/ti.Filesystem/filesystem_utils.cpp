/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#include "filesystem_utils.h"

#include <Poco/File.h>
#include <Poco/Path.h>
#include <Poco/FileStream.h>
#include <Poco/Exception.h>

namespace ti
{

namespace FilesystemUtils
{

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
}
}
