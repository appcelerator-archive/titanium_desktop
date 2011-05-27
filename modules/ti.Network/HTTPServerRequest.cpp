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

#include "HTTPServerRequest.h"

namespace Titanium {

HTTPServerRequest::HTTPServerRequest(Poco::Net::HTTPServerRequest& request)
    : StaticBoundObject("Network.HTTPServerRequest")
    , request(request)
{
    SetMethod("getMethod",&HTTPServerRequest::GetMethod);
    SetMethod("getVersion",&HTTPServerRequest::GetVersion);
    SetMethod("getURI",&HTTPServerRequest::GetURI);
    SetMethod("getContentType",&HTTPServerRequest::GetContentType);
    SetMethod("getContentLength",&HTTPServerRequest::GetContentLength);
    SetMethod("getHeader",&HTTPServerRequest::GetHeader);
    SetMethod("hasHeader",&HTTPServerRequest::HasHeader);
    SetMethod("getHeaders", &HTTPServerRequest::GetHeaders);
    SetMethod("read",&HTTPServerRequest::Read);
}

void HTTPServerRequest::GetMethod(const ValueList& args, KValueRef result)
{
    std::string method = request.getMethod();
    result->SetString(method);
}

void HTTPServerRequest::GetVersion(const ValueList& args, KValueRef result)
{
    std::string version = request.getVersion();
    result->SetString(version);
}

void HTTPServerRequest::GetURI(const ValueList& args, KValueRef result)
{
    std::string uri = request.getURI();
    result->SetString(uri);
}

void HTTPServerRequest::GetContentType(const ValueList& args, KValueRef result)
{
    std::string ct = request.getContentType();
    result->SetString(ct);
}

void HTTPServerRequest::GetContentLength(const ValueList& args, KValueRef result)
{
    result->SetInt(request.getContentLength());
}

void HTTPServerRequest::GetHeader(const ValueList& args, KValueRef result)
{
    args.VerifyException("getHeader", "s");
    std::string name = args.at(0)->ToString();
    if (request.has(name))
    {
        std::string value = request.get(name);
        result->SetString(value);
    }
    else
    {
        result->SetNull();
    }
}

void HTTPServerRequest::GetHeaders(const ValueList& args, KValueRef result)
{
    Poco::Net::HTTPServerRequest::ConstIterator iter = request.begin();
    KObjectRef headers = new StaticBoundObject();
    
    for(; iter != request.end(); iter++)
    {
        std::string name = iter->first;
        std::string value = iter->second;
        headers->SetString(name.c_str(), value.c_str());
    }
    result->SetObject(headers);
}

void HTTPServerRequest::HasHeader(const ValueList& args, KValueRef result)
{
    args.VerifyException("hasHeader", "s");
    std::string name = args.at(0)->ToString();
    result->SetBool(request.has(name));
}

void HTTPServerRequest::Read(const ValueList& args, KValueRef result)
{
    args.VerifyException("read", "?i");

    std::istream &in = request.stream();
    if (in.eof() || in.fail())
    {
        result->SetNull();
        return;
    }

    int maxSize = args.GetInt(0, 8096);
    char *buf = new char[maxSize];
    in.read(buf, maxSize);
    int count = static_cast<int>(in.gcount());
    if (count == 0)
    {
        result->SetNull();
    }
    else
    {
        result->SetObject(new Bytes(buf,count));
    }
    delete [] buf;
}

} // namespace Titanium
