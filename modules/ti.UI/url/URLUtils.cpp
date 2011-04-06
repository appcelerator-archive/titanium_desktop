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

#include "URLUtils.h"

#include <kroll/kroll.h>
#include <Poco/Environment.h>
#include <Poco/URI.h>

using namespace Poco;
using namespace std;

namespace Titanium {

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
        kroll::Logger* log = kroll::Logger::Get("UI.URL");
        log->Error("Could not convert %s to a path: %s", url, ss->c_str());
    }
    catch (...)
    {
        kroll::Logger* log = kroll::Logger::Get("UI.URL");
        log->Error("Could not convert %s to a path", url);
    }
}

void ProxyForURLCallback(const char* url, char* buffer, int bufferLength)
{
    buffer[bufferLength - 1] = '\0';

    std::string urlString(url);
    SharedProxy proxy(ProxyConfig::GetProxyForURL(urlString));
    if (proxy.isNull())
        strncpy(buffer, "direct://", bufferLength);
    else
        strncpy(buffer, proxy->ToString().c_str(), bufferLength);
}

} // namespace Titanium
