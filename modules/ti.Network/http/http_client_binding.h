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
#include <Poco/Path.h>
#include <Poco/URI.h>
#include <Poco/Exception.h>
#include <Poco/Thread.h>
#include <Poco/FileStream.h>

namespace ti
{
	class HTTPClientBinding : public StaticBoundObject
	{
	public:
		HTTPClientBinding(Host* host, std::string path);
		virtual ~HTTPClientBinding();
		
		SharedValue GetSelf() { return self;}
		
	private:
		Host* host;
		std::string modulePath;
		SharedKObject global;
		Poco::Thread *thread;
		std::string url;
		Poco::Net::HTTPResponse* response;
		std::map<std::string,std::string> headers;
		std::string method;
		bool async;
		std::string user;
		std::string password;
		SharedValue self;
		Poco::FileInputStream *filestream;
		std::string filename;
		std::string datastream;
		std::string dirstream;
		int timeout;
		bool shutdown;
		SharedKMethod readystate;
		SharedKMethod onchange;
		static bool initialized;
		
		static void Run(void*);

		void Abort(const ValueList& args, SharedValue result);
		void Open(const ValueList& args, SharedValue result);
		void SetRequestHeader(const ValueList& args, SharedValue result);
		void Send(const ValueList& args, SharedValue result);
		void SendFile(const ValueList& args, SharedValue result);
		void SendDir(const ValueList& args, SharedValue result);
		void GetResponseHeader(const ValueList& args, SharedValue result);
		void SetTimeout(const ValueList& args, SharedValue result);

		void ChangeState(int readyState);
	};
}

#endif
