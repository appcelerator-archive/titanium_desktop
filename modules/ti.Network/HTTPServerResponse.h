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

#ifndef HTTPServerResponse_h
#define HTTPServerResponse_h

#include <kroll/kroll.h>
#include <Poco/Net/HTTPServerResponse.h>

namespace Titanium {

class HTTPServerResponse : public StaticBoundObject {
public:
    HTTPServerResponse(Poco::Net::HTTPServerResponse &response);
    
private:
    void SetStatus(const ValueList& args, KValueRef result);
    void SetReason(const ValueList& args, KValueRef result);
    void SetStatusAndReason(const ValueList& args, KValueRef result);
    void SetContentType(const ValueList& args, KValueRef result);
    void SetContentLength(const ValueList& args, KValueRef result);
    void AddCookie(const ValueList& args, KValueRef result);
    void SetHeader(const ValueList& args, KValueRef result);
    void Write(const ValueList& args, KValueRef result);

    Poco::Net::HTTPServerResponse& response;
};

} // namespace Titanium

#endif
