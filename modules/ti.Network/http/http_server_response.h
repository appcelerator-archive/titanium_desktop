/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _HTTP_SERVER_RESPONSE_H_
#define _HTTP_SERVER_RESPONSE_H_

#include <kroll/kroll.h>
#include <Poco/Net/HTTPServerResponse.h>

namespace ti
{
	class HttpServerResponse : public StaticBoundObject
	{
	public:
		HttpServerResponse(Poco::Net::HTTPServerResponse &response);
		virtual ~HttpServerResponse();
		
	private:
		Poco::Net::HTTPServerResponse& response;

		void SetStatus(const ValueList& args, KValueRef result);
		void SetReason(const ValueList& args, KValueRef result);
		void SetStatusAndReason(const ValueList& args, KValueRef result);
		void SetContentType(const ValueList& args, KValueRef result);
		void SetContentLength(const ValueList& args, KValueRef result);
		void AddCookie(const ValueList& args, KValueRef result);
		void SetHeader(const ValueList& args, KValueRef result);
		void Write(const ValueList& args, KValueRef result);

	};
}

#endif
