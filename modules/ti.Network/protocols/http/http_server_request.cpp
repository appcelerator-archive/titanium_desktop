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
	HttpServerRequest::HttpServerRequest(Host *host, KMethodRef callback, 
		Poco::Net::HTTPServerRequest& request) :
			StaticBoundObject("Network.HttpServerRequest"),
			host(host),
			callback(callback),
			request(request)
	{
		/**
		 * @tiapi(method=True,name=Network.HTTPServerRequest.getMethod,since=0.3) get the HTTP method of this request
		 * @tiresult(for=Network.HTTPServerRequest.getMethod,type=String) the http method of this request, i.e. "GET", "POST"
		 */
		SetMethod("getMethod",&HttpServerRequest::GetMethod);
		
		/**
		 * @tiapi(method=True,name=Network.HTTPServerRequest.getVersion,since=0.3) get the HTTP version of this request
		 * @tiresult(for=Network.HTTPServerRequest.getVersion,type=String) the http version of this request, i.e. "1.1"
		 */
		SetMethod("getVersion",&HttpServerRequest::GetVersion);
		
		/**
		 * @tiapi(method=True,name=Network.HTTPServerRequest.getURI,since=0.3) get the URI of this request
		 * @tiresult(for=Network.HTTPServerRequest.getURI,type=String) the URI of this request, i.e. "/path/index.html"
		 */
		SetMethod("getURI",&HttpServerRequest::GetURI);
		
		/**
		 * @tiapi(method=True,name=Network.HTTPServerRequest.getContentType,since=0.3) get the content type of this request
		 * @tiresult(for=Network.HTTPServerRequest.getContentType,type=String) the content type of this request, i.e. "text/plain"
		 */
		SetMethod("getContentType",&HttpServerRequest::GetContentType);
		
		/**
		 * @tiapi(method=True,name=Network.HTTPServerRequest.getContentLength,since=0.3) the content length of this request
		 * @tiresult(for=Network.HTTPServerRequest.getContentLength,type=Number) the content length of this request, i.e 100
		 */
		SetMethod("getContentLength",&HttpServerRequest::GetContentLength);
		
		/**
		 * @tiapi(method=True,name=Network.HTTPServerRequest.getHeader,since=0.3) get an HTTP header value from this request
		 * @tiarg(for=Network.HTTPServerRequest.getHeader,type=String,name=header) the header of the request
		 * @tiresult(for=Network.HTTPServerRequest.getHeader,type=String) the value of the passed in HTTP header or null
		 */
		SetMethod("getHeader",&HttpServerRequest::GetHeader);
		
		/**
		 * @tiapi(method=True,name=Network.HTTPServerRequest.hasHeader,since=0.3) check to see if this request has an HTTP header
		 * @tiarg(for=Network.HTTPServerRequest.hasHeader,type=String,name=header) the header of the request to check
		 * @tiresult(for=Network.HTTPServerRequest.hasHeader,type=Boolean) whether or not this request has the passed in HTTP header
		 */
		SetMethod("hasHeader",&HttpServerRequest::HasHeader);
		
		/**
		 * @tiapi(method=True,name=Network.HTTPServerRequest.getHeaders,since=0.7) get an HTTP header value from this request
		 * @tiarg(for=Network.HTTPServerRequest.getHeader,type=String,name=header) the header of the request
		 * @tiresult(for=Network.HTTPServerRequest.getHeader,type=String) the value of the passed in HTTP header or null
		 */
		SetMethod("getHeaders", &HttpServerRequest::GetHeaders);
	
		/**
		 * @tiapi(method=True,name=Network.HTTPServerRequest.read,since=0.3) read content from this request
		 * @tiarg(for=Network.HTTPServerRequest.read,type=Number,optional=True,name=length) the number of bytes to read (default 8096)
		 * @tiresult(for=Network.HTTPServerRequest.read,type=String) the data read from this request
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
		RunOnMainThread(callback, args);
	}

	void HttpServerRequest::GetMethod(const ValueList& args, KValueRef result)
	{
		std::string method = request.getMethod();
		result->SetString(method);
	}

	void HttpServerRequest::GetVersion(const ValueList& args, KValueRef result)
	{
		std::string version = request.getVersion();
		result->SetString(version);
	}

	void HttpServerRequest::GetURI(const ValueList& args, KValueRef result)
	{
		std::string uri = request.getURI();
		result->SetString(uri);
	}

	void HttpServerRequest::GetContentType(const ValueList& args, KValueRef result)
	{
		std::string ct = request.getContentType();
		result->SetString(ct);
	}

	void HttpServerRequest::GetContentLength(const ValueList& args, KValueRef result)
	{
		result->SetInt(request.getContentLength());
	}

	void HttpServerRequest::GetHeader(const ValueList& args, KValueRef result)
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
	
	void HttpServerRequest::GetHeaders(const ValueList& args, KValueRef result)
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

	void HttpServerRequest::HasHeader(const ValueList& args, KValueRef result)
	{
		args.VerifyException("hasHeader", "s");
		std::string name = args.at(0)->ToString();
		result->SetBool(request.has(name));
	}

	void HttpServerRequest::Read(const ValueList& args, KValueRef result)
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

}
