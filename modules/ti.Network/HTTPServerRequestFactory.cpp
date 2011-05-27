/*
 * Copyright (c) 2009-2010 Appcelerator, Inc. All Rights Reserved.
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

#include "HTTPServerRequestFactory.h"

#include <Poco/Net/HTTPRequestHandler.h>

#include "HTTPServerRequest.h"
#include "HTTPServerResponse.h"

namespace Titanium {

class HTTPRequestHandler : public Poco::Net::HTTPRequestHandler {
public:
    HTTPRequestHandler(KMethodRef callback)
        : m_callback(callback)
    {
    }

    virtual void handleRequest(Poco::Net::HTTPServerRequest&, Poco::Net::HTTPServerResponse&);

private:
    KMethodRef m_callback;
};

void HTTPRequestHandler::handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response) {
    // XXX(Josh): The request and response object's lifetime is limited to this functions call.
    // If the developer should keep a reference to these around past the callback lifetime and then
    // attempts to access it may result in a crash!
    ValueList args;
    args.push_back(Value::NewObject(new HTTPServerRequest(request)));
    args.push_back(Value::NewObject(new HTTPServerResponse(response)));
    RunOnMainThread(m_callback, args);
}

HTTPServerRequestFactory::HTTPServerRequestFactory(KMethodRef callback)
    : m_callback(callback)
{
}

Poco::Net::HTTPRequestHandler* HTTPServerRequestFactory::createRequestHandler(const Poco::Net::HTTPServerRequest& request)
{
    return new HTTPRequestHandler(m_callback);
}

} // namespace Titanium
