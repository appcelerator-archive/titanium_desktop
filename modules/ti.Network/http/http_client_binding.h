/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _HTTP_CLIENT_BINDING_H_
#define _HTTP_CLIENT_BINDING_H_

#include <kroll/kroll.h>
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/NameValueCollection.h>
#include <Poco/Net/HTTPBasicCredentials.h>
#include <Poco/Path.h>
#include <Poco/URI.h>
#include <Poco/Exception.h>
#include <Poco/Thread.h>
#include <Poco/FileStream.h>
#include <Poco/Runnable.h>
#include <Poco/Event.h>
#include "http_cookie.h"

namespace ti
{
	class HTTPClientEvent;

	class HTTPClientBinding : public KEventObject, public Poco::Runnable
	{
	public:
		HTTPClientBinding(Host* host, std::string path);
		virtual ~HTTPClientBinding();

		void Abort(const ValueList& args, SharedValue result);
		void Open(const ValueList& args, SharedValue result);
		void SetBasicCredentials(const ValueList& args, SharedValue result);
		void Send(const ValueList& args, SharedValue result);
		void Receive(const ValueList& args, SharedValue result);
		void SetRequestHeader(const ValueList& args, SharedValue result);
		void GetResponseHeader(const ValueList& args, SharedValue result);
		void SetCookie(const ValueList& args, SharedValue result);
		void ClearCookies(const ValueList& args, SharedValue result);
		void GetCookie(const ValueList& args, SharedValue result);
		void SetTimeout(const ValueList& args, SharedValue result);

		bool FireEvent(std::string& eventName);

	private:
		Host* host;
		std::string modulePath;
		SharedKObject global;
		std::string url;
		std::map<std::string,std::string> headers;
		Poco::Net::HTTPResponse response;
		std::string method;
		bool async;
		int timeout;
		int maxRedirects;
		int bufferSize;
		std::string userAgent;
		static bool initialized;
		SharedKMethod outputHandler;
		Poco::Net::NameValueCollection requestCookies;
		std::map<std::string, Poco::Net::HTTPCookie> responseCookies;
		Poco::Net::HTTPBasicCredentials basicCredentials;

		SharedKMethod ondatastream;
		SharedKMethod onreadystate;
		SharedKMethod onsendstream;
		SharedKMethod onload;

		// This variables must be reset on each send()
		SharedPtr<Poco::Thread> thread;
		SharedPtr<std::istream> datastream;
		SharedPtr<std::ostringstream> outstream;
		int contentLength;
		Poco::Event abort;
		bool dirty;

		bool ExecuteRequest(SharedValue sendData);
		void ChangeState(int readyState);
		void Reset();
		void InitHTTPS();

		// Thread main		
		void run();
	};
}

#endif
