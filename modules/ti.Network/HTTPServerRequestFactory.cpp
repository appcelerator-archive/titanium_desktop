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
    ValueList args;
    args.push_back(Value::NewObject(new HTTPServerRequest(request)));
    HTTPServerResponse * resp = new HTTPServerResponse(response);
    args.push_back(Value::NewObject(resp));
    resp->asyncState = 0;
    RunOnMainThread(m_callback, args);
    while (resp->asyncState > 0) {
        Poco::Thread::sleep(5);
    }
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
