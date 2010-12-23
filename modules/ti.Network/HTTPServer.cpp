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

#include "HTTPServer.h"

#include <cstring>
#include <iostream>
#include <sstream>
#include <fstream>

// For some reason, 10.5 was fine with Cocoa headers being last, but 10.4 balks.
#ifdef OS_OSX
#import <Cocoa/Cocoa.h>
#endif

#include <kroll/kroll.h>
#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/HTTPServerParams.h>
#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/SocketAddress.h>

#include "HTTPServerRequestFactory.h"

namespace Titanium {

HTTPServer::HTTPServer()
	: StaticBoundObject("Network.HTTPServer")
	, global(kroll::Host::GetInstance()->GetGlobalObject())
	, callback(0)
	, socket(0)
	, connection(0)
{
	/**
	 * @tiapi(method=True,name=Network.HTTPServer.bind,since=0.3) bind this server to a port on a specific interface
	 * @tiarg(for=Network.HTTPServer.bind,name=port,type=Number) port to bind on
	 * @tiarg(for=Network.HTTPServer.bind,name=address,type=String,optional=True) address to bind to
	 * @tiarg(for=Network.HTTPServer.bind,name=callback,type=Method) callback for server logic (in seperate thread)
	 */
	SetMethod("bind",&HTTPServer::Bind);
	
	/**
	 * @tiapi(method=True,name=Network.HTTPServer.close,since=0.3) close this server
	 */
	SetMethod("close",&HTTPServer::Close);
	
	/**
	 * @tiapi(method=True,name=Network.HTTPServer.isClosed,since=0.3) check to see if this server socket is closed
	 * @tiresult(for=Network.HTTPServer.isClosed,type=Boolean) return whether or not this server socket is closed
	 */
	SetMethod("isClosed",&HTTPServer::IsClosed);
}

HTTPServer::~HTTPServer()
{
	KR_DUMP_LOCATION
	Close();
}

void HTTPServer::Bind(const ValueList& args, KValueRef result)
{
	Close();
	
	// port, callback
	// port, ipaddress, callback
	int port = args.at(0)->ToInt();
	std::string ipaddress = "127.0.0.1";
	
	if (args.at(1)->IsString())
	{
		ipaddress = args.at(1)->ToString();
	}
	else if (args.at(1)->IsMethod())
	{
		callback = args.at(1)->ToMethod();
	}
	if (args.size()==3)
	{
		callback = args.at(2)->ToMethod();
	}
	
	Poco::Net::SocketAddress addr(ipaddress,port);
	this->socket = new Poco::Net::ServerSocket(addr);		
	
	connection = new Poco::Net::HTTPServer(new HttpServerRequestFactory(callback), *socket, new Poco::Net::HTTPServerParams);
	connection->start();
}

void HTTPServer::Close()
{
	if (this->connection!=NULL)
	{
		this->connection->stop();
		delete this->connection;
		connection = NULL;
	}
	if (this->socket!=NULL)
	{
		delete this->socket;
		this->socket = NULL;
	}
	this->callback = NULL;
}

void HTTPServer::Close(const ValueList& args, KValueRef result)
{
	Close();
}

void HTTPServer::IsClosed(const ValueList& args, KValueRef result)
{
	result->SetBool(this->connection==NULL);
}

} // namespace Titanium
