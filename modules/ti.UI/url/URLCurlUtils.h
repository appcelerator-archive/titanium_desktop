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

#ifndef URLCurlUtils_h
#define URLCurlUtils_h

#include <curl/curl.h>

extern struct Curl_local_handler CurlTiURLHandler;
extern struct Curl_local_handler CurlAppURLHandler;

namespace Titanium {

	const char* TiURLToPathCurl(const char *url);
	const char* AppURLToPathCurl(const char *url);

} // namespace Titanium

#endif
