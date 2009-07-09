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

	string TiURLToPath(string tiURL)
	{
		try
		{
			URI inURI = URI(tiURL);

			if (inURI.getScheme() != "ti")
			{
				return tiURL;
			}

			string host = inURI.getHost();
			SharedApplication app = Host::GetInstance()->GetApplication();
			string path = app->GetComponentPath(host);

			if (path.empty())
			{
				throw ValueException::FromString("Could not find component "+host);
			}

			std::vector<std::string> segments;
			inURI.getPathSegments(segments);

			for (size_t i = 0; i < segments.size(); i++)
			{
				path = FileUtils::Join(path.c_str(), segments[i].c_str(), NULL);
			}
			return path;
		}
		catch (ValueException& e)
		{
			SharedString ss = e.DisplayString();
			Logger* log = Logger::Get("UI.URL");
			log->Error("Could not convert %s to a path: %s", tiURL.c_str(), ss->c_str());
		}
		catch (...)
		{
			Logger* log = Logger::Get("UI.URL");
			log->Error("Could not convert %s to a path", tiURL.c_str());
		}
		return tiURL;
	}

	string NormalizeAppURL(string url)
	{
		size_t appLength = 6; // app://
		string id = Host::GetInstance()->GetApplication()->id;
		size_t idLength = id.size();
		string idPart = url.substr(appLength, idLength);

		if (idPart == id)
		{
			return url;
		}
		else
		{
			return string("app://") + id + "/" + url.substr(appLength);
		}
	}

	string AppURLToPath(string appURL)
	{
		try
		{
			URI inURI = URI(appURL);
			if (inURI.getScheme() != "app")
			{
				return appURL;
			}

			appURL = NormalizeAppURL(appURL);
			inURI = URI(appURL);

			SharedApplication app = Host::GetInstance()->GetApplication();
			string path = app->GetResourcesPath();

			vector<string> segments;
			inURI.getPathSegments(segments);
			for (size_t i = 0; i < segments.size(); i++)
			{
				path = FileUtils::Join(path.c_str(), segments[i].c_str(), NULL);
			}
			return path;
		}
		catch (ValueException& e)
		{
			SharedString ss = e.DisplayString();
			Logger* log = Logger::Get("UI.URL");
			log->Error("Could not convert %s to a path: %s", appURL.c_str(), ss->c_str());
		}
		catch (...)
		{
			Logger* log = Logger::Get("UI.URL");
			log->Error("Could not convert %s to a path", appURL.c_str());
		}

		return appURL;
	}

	void NormalizeURLCallback(const char* url, char* buffer, int bufferLength)
	{
		strncpy(buffer, url, bufferLength);
		buffer[bufferLength - 1] = '\0';

		URI inURI = URI(url);
		if (inURI.getScheme() == "app")
		{
			string normalized = NormalizeAppURL(url);
			strncpy(buffer, normalized.c_str(), bufferLength);
			buffer[bufferLength - 1] = '\0';
		}

	}

	string URLToPathOrURL(string& url)
	{
		URI inURI = URI(url);
		try {

			if (inURI.getScheme() == "ti") {
				return TiURLToPath(url);

			} else if (inURI.getScheme() == "app") {
				return AppURLToPath(url);
			}

		} catch (ValueException& e) {
			SharedString ss = e.DisplayString();
			Logger* log = Logger::Get("UI.URL");
			log->Error("Could not convert %s to a path: %s", url.c_str(), ss->c_str());
		}
		return url;
	}

	void URLToFileURLCallback(const char* url, char* buffer, int bufferLength)
	{
		strncpy(buffer, url, bufferLength);
		buffer[bufferLength - 1] = '\0';

		try
		{
			URI inURI = URI(url);
			if (inURI.getScheme() == "ti")
			{
				string path = TiURLToPath(url);
				string fileURL = FileUtils::PathToFileURL(path);
				strncpy(buffer, fileURL.c_str(), bufferLength);
				buffer[bufferLength - 1] = '\0';
			}
			else if (inURI.getScheme() == "app")
			{
				string path = AppURLToPath(url);
				string fileURL = FileUtils::PathToFileURL(path);
				strncpy(buffer, fileURL.c_str(), bufferLength);
				buffer[bufferLength - 1] = '\0';
			}
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


}
