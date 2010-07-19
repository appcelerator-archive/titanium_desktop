/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _HTTP_SERVER_REQUEST_FACTORY_H_
#define _HTTP_SERVER_REQUEST_FACTORY_H_

#include <kroll/kroll.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPRequestHandler.h>

namespace ti
{
	class HttpServerRequestFactory : public Poco::Net::HTTPRequestHandlerFactory
	{
	public:
		HttpServerRequestFactory(Host *host, KMethodRef callback);
		virtual ~HttpServerRequestFactory();
		
		Poco::Net::HTTPRequestHandler* createRequestHandler(const Poco::Net::HTTPServerRequest &request);
	private:
		Host *host;
		KMethodRef callback;
	};
}

#endif
