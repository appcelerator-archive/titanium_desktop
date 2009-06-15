/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#include "http_server_response.h"
#include <Poco/Net/HTTPCookie.h>

namespace ti
{
	HttpServerResponse::HttpServerResponse(Poco::Net::HTTPServerResponse &response) :
		response(response)
	{
		SetMethod("setStatus",&HttpServerResponse::SetStatus);
		SetMethod("setReason",&HttpServerResponse::SetReason);
		SetMethod("setStatusAndReason",&HttpServerResponse::SetStatusAndReason);
		SetMethod("setContentType",&HttpServerResponse::SetContentType);
		SetMethod("setContentLength",&HttpServerResponse::SetContentLength);
		SetMethod("addCookie",&HttpServerResponse::AddCookie);
		SetMethod("setHeader",&HttpServerResponse::SetHeader);
		SetMethod("write",&HttpServerResponse::Write);
	}
	HttpServerResponse::~HttpServerResponse()
	{
	}
	void HttpServerResponse::SetStatus(const ValueList& args, SharedValue result)
	{
		std::string status = args.at(0)->ToString();
		response.setStatus(status);
	}
	void HttpServerResponse::SetReason(const ValueList& args, SharedValue result)
	{
		std::string reason = args.at(0)->ToString();
		response.setReason(reason);
	}
	void HttpServerResponse::SetStatusAndReason(const ValueList& args, SharedValue result)
	{
		std::string status = args.at(0)->ToString();
		std::string reason = args.at(1)->ToString();
		response.setStatus(status);
		response.setReason(reason);
	}
	void HttpServerResponse::SetContentType(const ValueList& args, SharedValue result)
	{
		std::string ct = args.at(0)->ToString();
		response.setContentType(ct);
	}
	void HttpServerResponse::SetContentLength(const ValueList& args, SharedValue result)
	{
		int len = args.at(0)->ToInt();
		response.setContentLength(len);
	}
	void HttpServerResponse::AddCookie(const ValueList& args, SharedValue result)
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
	void HttpServerResponse::SetHeader(const ValueList& args, SharedValue result)
	{
		std::string name = args.at(0)->ToString();
		std::string value = args.at(1)->ToString();
		response.set(name,value);
	}
	void HttpServerResponse::Write(const ValueList& args, SharedValue result)
	{
		std::ostream& ostr = response.send();
		
		if (args.at(0)->IsString())
		{
			const char *data = args.at(0)->ToString();
			ostr << data;
			ostr.flush();
			return;
		}
		else if (args.at(0)->IsObject())
		{
			SharedKObject obj = args.at(0)->ToObject();
			SharedPtr<Blob> blob = obj.cast<Blob>();
			if (!blob.isNull())
			{
				const char *data = blob->Get();
				ostr << data;
				ostr.flush();
				return;
			}
		}
		throw ValueException::FromString("unknown type");
	}
}