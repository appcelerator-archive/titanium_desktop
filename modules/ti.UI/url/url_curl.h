/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef URL_CURL_H_
#define URL_CURL_H_
#include <curl/curl.h>
extern struct Curl_local_handler CurlTiURLHandler;
extern struct Curl_local_handler CurlAppURLHandler;
namespace ti
{
	const char* TiURLToPathCurl(const char *url);
	const char* AppURLToPathCurl(const char *url);
}
#endif
