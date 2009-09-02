/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#include "../ui_module.h"
#include <Poco/Environment.h>
#include <Poco/URI.h>
using Poco::URI;
using std::string;
namespace ti
{
	void NormalizeURLCallback(const char* url, char* buffer, int bufferLength)
	{
		strncpy(buffer, url, bufferLength);
		buffer[bufferLength - 1] = '\0';

		string urlString = url;
		string normalized = URLUtils::NormalizeURL(urlString);
		if (normalized != urlString)
		{
			strncpy(buffer, normalized.c_str(), bufferLength);
			buffer[bufferLength - 1] = '\0';
		}
	}

	void URLToFileURLCallback(const char* url, char* buffer, int bufferLength)
	{
		strncpy(buffer, url, bufferLength);
		buffer[bufferLength - 1] = '\0';

		try
		{
			string newURL = url;
			string path = URLUtils::URLToPath(newURL);
			if (path != newURL)
				newURL = URLUtils::PathToFileURL(path);

			strncpy(buffer, newURL.c_str(), bufferLength);
			buffer[bufferLength - 1] = '\0';
		}
		catch (ValueException& e)
		{
			SharedString ss = e.DisplayString();
			Logger* log = Logger::Get("UI.URL");
			log->Error("Could not convert %s to a path: %s", url, ss->c_str());
		}
		catch (...)
		{
			Logger* log = Logger::Get("UI.URL");
			log->Error("Could not convert %s to a path", url);
		}
	}

	int CanPreprocessURLCallback(const char* url)
	{
		return Script::GetInstance()->CanPreprocess(url);
	}

	char* PreprocessURLCallback(const char* url, KeyValuePair* headers, char** mimeType)
	{
		Logger* logger = Logger::Get("UI.URL");

		SharedKObject scope = new StaticBoundObject();
		SharedKObject kheaders = new StaticBoundObject();
		while (headers->key)
		{
			kheaders->SetString(headers->key, headers->value);
			headers++;
		}

		try
		{
			AutoPtr<PreprocessData> result = 
				Script::GetInstance()->Preprocess(url, scope);
			*mimeType = strdup(result->mimeType.c_str());
			printf("%s\n", *mimeType);
			return strdup(result->data->Get());
		}
		catch (ValueException& e)
		{
			logger->Error("Error in preprocessing: %s", e.ToString().c_str());
		}
		catch (...)
		{
			logger->Error("Unknown Error in preprocessing");
		}

		return NULL;
	}

}
