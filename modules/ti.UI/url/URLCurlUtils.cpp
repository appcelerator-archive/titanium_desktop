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

#include "URLCurlUtils.h"

#include <cstring>

struct Curl_local_handler CurlTiURLHandler = { "ti", ti::TiURLToPathCurl };
struct Curl_local_handler CurlAppURLHandler = { "app", ti::AppURLToPathCurl };

namespace Titanium {

/* TODO: Every URL will leak in the curl handlers 
 * unless curl frees the result of these calls */
const char* TiURLToPathCurl(const char *url)
{
    string stURL = url;
    if (stURL.find("ti://") != 0)
    {
        stURL = string("ti://") + stURL;
    }

    std::string path = URLUtils::TiURLToPath(stURL);
    return strdup(path.c_str());
}

const char* AppURLToPathCurl(const char *url)
{
    string stURL = url;
    if (stURL.find("app://") != 0)
    {
        stURL = string("app://") + stURL;
    }

    std::string path = URLUtils::AppURLToPath(stURL);
    return strdup(path.c_str());
}

} // namespace Titanium
