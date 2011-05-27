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

#include <kroll/kroll.h>
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
    SetMethod("bind",&HTTPServer::Bind);
    SetMethod("close",&HTTPServer::Close);
    SetMethod("isClosed",&HTTPServer::IsClosed);
}

HTTPServer::~HTTPServer()
{
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
    
    connection = new Poco::Net::HTTPServer(new HTTPServerRequestFactory(callback), *socket, new Poco::Net::HTTPServerParams);
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
