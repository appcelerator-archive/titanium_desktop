/*
 * Copyright (c) 2010 Appcelerator, Inc. All Rights Reserved.
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

#include "TCPServerSocket.h"

#include <Poco/Net/SocketNotification.h>
#include <Poco/NObserver.h>

#include "TCPServerConnection.h"

namespace Titanium {

TCPServerSocketConnector::TCPServerSocketConnector(KMethodRef callback_, Poco::Net::ServerSocket& socket_, Poco::Net::SocketReactor & reactor_)
    : callback(callback_)
    , socket(socket_)
    , reactor(reactor_)
{
    reactor.addEventHandler(socket, Poco::Observer<TCPServerSocketConnector, Poco::Net::ReadableNotification>(*this, &TCPServerSocketConnector::onAccept));
}

TCPServerSocketConnector::~TCPServerSocketConnector()
{
    reactor.removeEventHandler(socket, Poco::Observer<TCPServerSocketConnector, Poco::Net::ReadableNotification>(*this, &TCPServerSocketConnector::onAccept));
}

void TCPServerSocketConnector::onAccept(Poco::Net::ReadableNotification *n)
{
    kroll::Logger *logger = kroll::Logger::Get("Network.TCPServerSocketConnector");

    n->release();
    Poco::Net::StreamSocket sock = socket.acceptConnection();
    AutoPtr<TCPServerConnection> conn = new TCPServerConnection(sock,reactor);

    ValueList args = ValueList();
    args.push_back(Value::NewObject(conn));
    try
    {
        RunOnMainThread(callback, args, false);
    }
    catch(ValueException& e)
    {
        SharedString ss = e.GetValue()->DisplayString();
        logger->Error("onAccept callback failed: %s", ss->c_str());
    }
}

TCPServerSocket::TCPServerSocket(KMethodRef create)
    : StaticBoundObject("Network.TCPServerSocket")
    , onCreate(create)
    , socket(0)
    , acceptor(0)
    , listenAdapter(new Poco::RunnableAdapter<TCPServerSocket>(
        *this, &TCPServerSocket::ListenThread))
    , listening(false)
{
    /**
     * @tiapi(method=True,name=Network.TCPServerSocket.listen,since=1.2)
     * @tiapi start listening for incoming connections
     * @tiarg[int, port] port to bind server socket
     */
    this->SetMethod("listen", &TCPServerSocket::Listen);

    /**
     * @tiapi(method=True,name=Network.TCPServerSocket.close,since=1.2)
     * @tiapi stop listening for incoming connections
     */
    this->SetMethod("close", &TCPServerSocket::Close);
}

TCPServerSocket::~TCPServerSocket()
{
    if (this->listening)
    {
        this->listening = false;
        this->reactor.stop();
        this->socket->close();
    }

    listenThread.join();
    delete listenAdapter;
}

void TCPServerSocket::ListenThread()
{
    this->acceptor = new TCPServerSocketConnector(this->onCreate,*this->socket,this->reactor);
    this->listening = true;

    while(this->listening)
    {
        this->reactor.run();
    }
}

void TCPServerSocket::Listen(const ValueList& args, KValueRef result)
{
    args.VerifyException("listen", "n");

    if (this->listening || listenThread.isRunning())
    {
        throw ValueException::FromString("Socket is already listening");
    }
    
    //TODO: add support for bind ipaddress
    
    int port = args.at(0)->ToInt();
    this->socket = new Poco::Net::ServerSocket(port);

    listenThread.start(*listenAdapter);
    result->SetBool(true);
}

void TCPServerSocket::Close(const ValueList& args, KValueRef result)
{
    if (this->listening)
    {
        this->listening = false;
        this->reactor.stop();
        this->socket->close();
        result->SetBool(true);
    }
    else
    {
        result->SetBool(false);
    }
}

} // namespace Titanium
