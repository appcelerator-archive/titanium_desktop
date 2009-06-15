/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef APP_URL_H_
#define APP_URL_H_

#include <curl/curl.h>
#include <string>

namespace ti {
	const char * AppURLGetAbsolutePath(const char *full_path);
	std::string AppURLNormalizeURL(std::string originalURL, std::string appID);
}

extern struct Curl_local_handler Titanium_app_url_handler;

#endif /* APP_URL_H_ */
