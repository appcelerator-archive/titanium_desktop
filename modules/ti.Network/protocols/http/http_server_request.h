/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _HTTP_SERVER_REQUEST_H_
#define _HTTP_SERVER_REQUEST_H_

#include <kroll/kroll.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>

namespace ti
{
	class HttpServerRequest : public Poco::Net::HTTPRequestHandler, public StaticBoundObject
	{
	public:
		HttpServerRequest(Host *host, KMethodRef callback, Poco::Net::HTTPServerRequest& request);
		virtual ~HttpServerRequest();

		void handleRequest(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse& response);

	private:
		Host *host;
		KMethodRef callback;
		Poco::Net::HTTPServerRequest& request;

		void GetMethod(const ValueList& args, KValueRef result);
		void GetVersion(const ValueList& args, KValueRef result);
		void GetURI(const ValueList& args, KValueRef result);
		void GetContentType(const ValueList& args, KValueRef result);
		void GetContentLength(const ValueList& args, KValueRef result);
		void GetHeader(const ValueList& args, KValueRef result);
		void GetHeaders(const ValueList& args, KValueRef result);
		void HasHeader(const ValueList& args, KValueRef result);
		void Read(const ValueList& args, KValueRef result);
	};
}

#endif
