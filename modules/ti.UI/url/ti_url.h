/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef TI_URL_H_
#define TI_URL_H_

#include <curl/curl.h>
#include <string>

namespace ti {
	const char * TiURLGetAbsolutePath(const char *full_path);
}

extern struct Curl_local_handler Titanium_ti_url_handler;

#endif
