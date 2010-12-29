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

#ifndef HTTPServerRequest_h
#define HTTPServerRequest_h

#include <kroll/kroll.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>

namespace Titanium {

class HttpServerRequest : public Poco::Net::HTTPRequestHandler, public StaticBoundObject {
public:
    HttpServerRequest(KMethodRef callback, Poco::Net::HTTPServerRequest& request);
    virtual ~HttpServerRequest();

    void handleRequest(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse& response);

private:
    void GetMethod(const ValueList& args, KValueRef result);
    void GetVersion(const ValueList& args, KValueRef result);
    void GetURI(const ValueList& args, KValueRef result);
    void GetContentType(const ValueList& args, KValueRef result);
    void GetContentLength(const ValueList& args, KValueRef result);
    void GetHeader(const ValueList& args, KValueRef result);
    void GetHeaders(const ValueList& args, KValueRef result);
    void HasHeader(const ValueList& args, KValueRef result);
    void Read(const ValueList& args, KValueRef result);

    KMethodRef callback;
    Poco::Net::HTTPServerRequest& request;
};

} // namespace Titanium

#endif
