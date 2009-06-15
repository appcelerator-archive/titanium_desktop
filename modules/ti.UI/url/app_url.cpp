/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 *
 * A custom URL handler for app:// URLs
 */

#include <kroll/base.h>
#include "app_url.h"
#include <Poco/Environment.h>
#include <cstring>
#include <algorithm>
#include <cctype>
#include <kroll/kroll.h>

struct Curl_local_handler Titanium_app_url_handler = {
	"app",
	ti::AppURLGetAbsolutePath
};

namespace ti {

	static const char *kAppURLPrefix = "/Resources";
	/* TODO: Memory leak here */
	const char * AppURLGetAbsolutePath(const char *url)
	{
		std::string urlString = url;
		std::string appID = kroll::Host::GetInstance()->GetApplication()->id;

		// strip the "appid" (even if it's not the same..
		// eventually we'll need smarter logic here if we want to support intra-app URLs
		int slashIndex = urlString.find('/');
		while (slashIndex == 0) {
			urlString = urlString.substr(1);
			slashIndex = urlString.find('/');
		}
		urlString = urlString.substr(slashIndex+1);
		
		
		std::string path = Poco::Environment::get("KR_HOME", "");
		path = path + kAppURLPrefix + "/" + urlString;
		
		return strdup(path.c_str());
	}

	std::string AppURLNormalizeURL(std::string originalURL, std::string appID)
	{
		// append <appID> if needed to the url

		bool appurl = true;

		std::string url(originalURL);
		std::transform(url.begin(), url.end(), url.begin(), ::tolower);
		if(url.find("http://") == 0 || url.find("https://") == 0)
		{
			appurl = false;
		}

		if(appurl)
		{
			// url will end up as app://<appID>/path

			std::string urlPrefix("app://");
			urlPrefix.append(appID);

			if(url.find(urlPrefix) == 0)
			{
				// all good - nothing to do
				url = originalURL;
			}
			else if(url.find("app://") == 0)
			{
				// need to add the <appID> .. keep one of the / characters
				url = urlPrefix + originalURL.substr(5);
			}
			else if(url.find("/") == 0)
			{
				// need to add app://<appID>
				url = urlPrefix + originalURL;
			}
			else
			{
				// need to add app://<appID>
				// TODO - how do we handle relative URLs??
				url = urlPrefix + "/" + originalURL;
			}
		}
		else
		{
			// no need to modify the url
			url = originalURL;
		}

		return url;
	}
}
