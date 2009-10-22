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
	FileSystemUtils::FileSystemUtils() { }
	FileSystemUtils::~FileSystemUtils() { }
	
	/*static*/
	SharedString FileSystemUtils::GetFileName(KValueRef v)
	{
		if (v->IsString())
		{
			return new std::string(v->ToString());
		}
		else if (v->IsObject())
		{
			return v->ToObject()->DisplayString();
		}
		else
		{
			std::string message = "can't convert object of type ";
			message += v->GetType();
			message += " to filename: don't know what to do";
			throw ValueException::FromString(message);
		}
	}
	
	/*static*/
	AutoPtr<File> FileSystemUtils::ToFile(KObjectRef object)
	{
		AutoPtr<File> file = object.cast<File>();
		return file;
	}
}
