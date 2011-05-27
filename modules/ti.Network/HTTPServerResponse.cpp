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

#include "HTTPServerResponse.h"

#include <Poco/Net/HTTPCookie.h>
#include <Poco/Net/HTTPServerResponse.h>

#include "NetworkUtils.h"

namespace Titanium {

HTTPServerResponse::HTTPServerResponse(Poco::Net::HTTPServerResponse &response)
    : StaticBoundObject("Network.HTTPServerResponse")
    , response(response)
{
    SetMethod("setStatus",&HTTPServerResponse::SetStatus);
    SetMethod("setReason",&HTTPServerResponse::SetReason);
    SetMethod("setStatusAndReason",&HTTPServerResponse::SetStatusAndReason);
    SetMethod("setContentType",&HTTPServerResponse::SetContentType);
    SetMethod("setContentLength",&HTTPServerResponse::SetContentLength);
    SetMethod("addCookie",&HTTPServerResponse::AddCookie);
    SetMethod("setHeader",&HTTPServerResponse::SetHeader);
    SetMethod("write",&HTTPServerResponse::Write);
}

void HTTPServerResponse::SetStatus(const ValueList& args, KValueRef result)
{
    std::string status = args.at(0)->ToString();
    response.setStatus(status);
}

void HTTPServerResponse::SetReason(const ValueList& args, KValueRef result)
{
    std::string reason = args.at(0)->ToString();
    response.setReason(reason);
}

void HTTPServerResponse::SetStatusAndReason(const ValueList& args, KValueRef result)
{
    std::string status = args.at(0)->ToString();
    std::string reason = args.at(1)->ToString();
    response.setStatus(status);
    response.setReason(reason);
}

void HTTPServerResponse::SetContentType(const ValueList& args, KValueRef result)
{
    std::string ct = args.at(0)->ToString();
    response.setContentType(ct);
}

void HTTPServerResponse::SetContentLength(const ValueList& args, KValueRef result)
{
    int len = args.at(0)->ToInt();
    response.setContentLength(len);
}

void HTTPServerResponse::AddCookie(const ValueList& args, KValueRef result)
{
    //name,value,[max_age,domain,path]
    std::string name = args.at(0)->ToString();
    std::string value = args.at(1)->ToString();
    Poco::Net::HTTPCookie cookie(name,value);
    if (args.size()>2)
    {
        int max_age = args.at(2)->ToInt();
        cookie.setMaxAge(max_age);
    }
    if (args.size()>3)
    {
        std::string domain = args.at(3)->ToString();
        cookie.setDomain(domain);
    }
    if (args.size()>4)
    {
        std::string path = args.at(4)->ToString();
        cookie.setPath(path);
    }
}

void HTTPServerResponse::SetHeader(const ValueList& args, KValueRef result)
{
    std::string name = args.at(0)->ToString();
    std::string value = args.at(1)->ToString();
    response.set(name,value);
}

void HTTPServerResponse::Write(const ValueList& args, KValueRef result)
{
    std::ostream& ostr = response.send();
    
    if (args.at(0)->IsString())
    {
        ostr << args.at(0)->ToString();
        ostr.flush();
        return;
    }
    else if (args.at(0)->IsObject())
    {
        BytesRef bytes(ObjectToBytes(args.at(0)->ToObject()));
        if (bytes.isNull())
            throw ValueException::FromString("Don't know how to write that kind of data.");

        ostr.write(bytes->Pointer(), bytes->Length());
    }
    else
    {
        throw ValueException::FromString("Don't know how to write that kind of data.");
    }
}

} // namespace Titanium
