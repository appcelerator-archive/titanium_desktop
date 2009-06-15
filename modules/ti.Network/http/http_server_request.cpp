/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#include "http_server_request.h"
#include "http_server_response.h"
#include <Poco/Buffer.h>

namespace ti
{
	HttpServerRequest::HttpServerRequest(Host *host, SharedKMethod callback, Poco::Net::HTTPServerRequest &request) :
		host(host),
		callback(callback),
		request(request)
	{
		SetMethod("getMethod",&HttpServerRequest::GetMethod);
		SetMethod("getVersion",&HttpServerRequest::GetVersion);
		SetMethod("getURI",&HttpServerRequest::GetURI);
		SetMethod("getContentType",&HttpServerRequest::GetContentType);
		SetMethod("getContentLength",&HttpServerRequest::GetContentLength);
		SetMethod("getHeader",&HttpServerRequest::GetHeader);
		SetMethod("hasHeader",&HttpServerRequest::HasHeader);
		SetMethod("read",&HttpServerRequest::Read);

		// FIXME: This is a memory leak -- Poco manages the reference
		// count of this objects, yet we need to pass it into Kroll.
		// Things we could do:
		// 1. Keep a static registration of all SharedPtr* to these objects
		//    and lazily free them.
		// - Why not: Poco keeps an AutoPtr to this object and may try to free
		//   it before or after we do.
		//
		// 2. Bump the reference count and try to manage destruction ourselves.
		// - Why not: We have no good way of knowing when Poco is done
		//   with the object, so we can't ever safely delete it.
		//
		// Solution for now: leak.
		// Solution for later: move from SharedPtr to AutoPtr
		this->sharedPtr = new SharedPtr<HttpServerRequest>(this);
	}

	HttpServerRequest::~HttpServerRequest()
	{
	}

	void HttpServerRequest::handleRequest(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response)
	{
		ValueList args;
		args.push_back(Value::NewObject(*this->sharedPtr));
		args.push_back(Value::NewObject(new HttpServerResponse(response)));
		host->InvokeMethodOnMainThread(callback, args);
	}
	void HttpServerRequest::GetMethod(const ValueList& args, SharedValue result)
	{
		std::string method = request.getMethod();
		result->SetString(method);
	}
	void HttpServerRequest::GetVersion(const ValueList& args, SharedValue result)
	{
		std::string version = request.getVersion();
		result->SetString(version);
	}
	void HttpServerRequest::GetURI(const ValueList& args, SharedValue result)
	{
		std::string uri = request.getURI();
		result->SetString(uri);
	}
	void HttpServerRequest::GetContentType(const ValueList& args, SharedValue result)
	{
		std::string ct = request.getContentType();
		result->SetString(ct);
	}
	void HttpServerRequest::GetContentLength(const ValueList& args, SharedValue result)
	{
		result->SetInt(request.getContentLength());
	}
	void HttpServerRequest::GetHeader(const ValueList& args, SharedValue result)
	{
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
	void HttpServerRequest::HasHeader(const ValueList& args, SharedValue result)
	{
		std::string name = args.at(0)->ToString();
		result->SetBool(request.has(name));
	}
	void HttpServerRequest::Read(const ValueList& args, SharedValue result)
	{
		std::istream &in = request.stream();
		if (in.eof() || in.fail())
		{
			result->SetNull();
			return;
		}
		int max_size = 8096;
		if (args.size()==1)
		{
			max_size = args.at(0)->ToInt();
		}
		char *buf = new char[max_size];
		in.read(buf,max_size);
		std::streamsize count = in.gcount();
		if (count == 0)
		{
			result->SetNull();
		}
		else
		{
			result->SetObject(new Blob(buf,count));
		}
		delete [] buf;
	}

}
