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
		StaticBoundObject("HttpServerRequest"),
		host(host),
		callback(callback),
		request(request)
	{
		/**
		 * @tiapi(method=True,name=Network.HTTPServerRequest.getMethod,since=0.3) get the HTTP method of this request
		 * @tiresult(for=Network.HTTPServerRequest.getMethod,type=string) the http method of this request, i.e. "GET", "POST"
		 */
		SetMethod("getMethod",&HttpServerRequest::GetMethod);
		
		/**
		 * @tiapi(method=True,name=Network.HTTPServerRequest.getVersion,since=0.3) get the HTTP version of this request
		 * @tiresult(for=Network.HTTPServerRequest.getVersion,type=string) the http version of this request, i.e. "1.1"
		 */
		SetMethod("getVersion",&HttpServerRequest::GetVersion);
		
		/**
		 * @tiapi(method=True,name=Network.HTTPServerRequest.getURI,since=0.3) get the URI of this request
		 * @tiresult(for=Network.HTTPServerRequest.getURI,type=string) the URI of this request, i.e. "/path/index.html"
		 */
		SetMethod("getURI",&HttpServerRequest::GetURI);
		
		/**
		 * @tiapi(method=True,name=Network.HTTPServerRequest.getContentType,since=0.3) get the content type of this request
		 * @tiresult(for=Network.HTTPServerRequest.getContentType,type=string) the content type of this request, i.e. "text/plain"
		 */
		SetMethod("getContentType",&HttpServerRequest::GetContentType);
		
		/**
		 * @tiapi(method=True,name=Network.HTTPServerRequest.getContentLength,since=0.3) the content length of this request
		 * @tiresult(for=Network.HTTPServerRequest.getContentLength,type=integer) the content length of this request, i.e 100
		 */
		SetMethod("getContentLength",&HttpServerRequest::GetContentLength);
		
		/**
		 * @tiapi(method=True,name=Network.HTTPServerRequest.getHeader,since=0.3) get an HTTP header value from this request
		 * @tiarg(for=Network.HTTPServerRequest.getHeader,type=string,name=header) the header of the request
		 * @tiresult(for=Network.HTTPServerRequest.getHeader,type=string) the value of the passed in HTTP header or null
		 */
		SetMethod("getHeader",&HttpServerRequest::GetHeader);
		
		/**
		 * @tiapi(method=True,name=Network.HTTPServerRequest.hasHeader,since=0.3) check to see if this request has an HTTP header
		 * @tiarg(for=Network.HTTPServerRequest.hasHeader,type=string,name=header) the header of the request to check
		 * @tiresult(for=Network.HTTPServerRequest.hasHeader,type=boolean) whether or not this request has the passed in HTTP header
		 */
		SetMethod("hasHeader",&HttpServerRequest::HasHeader);
		
		/**
		 * @tiapi(method=True,name=Network.HTTPServerRequest.read,since=0.3) read content from this request
		 * @tiarg(for=Network.HTTPServerRequest.read,type=integer,optional=True,name=length) the number of bytes to read (default 8096)
		 * @tiresult(for=Network.HTTPServerRequest.read,type=string) the data read from this request
		 */
		SetMethod("read",&HttpServerRequest::Read);
	}

	HttpServerRequest::~HttpServerRequest()
	{
	}

	void HttpServerRequest::handleRequest(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response)
	{
		ValueList args;

		// We MUST duplicate 'this' before casting it into an
		// AutoPtr or else we will free this memory at the wrong time.
		this->duplicate();
		AutoPtr<HttpServerRequest> autoThis = this;

		args.push_back(Value::NewObject(autoThis));
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
		int count = static_cast<int>(in.gcount());
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
