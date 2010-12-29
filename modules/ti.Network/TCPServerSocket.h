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

#ifndef TCPServerSocket_h
#define TCPServerSocket_h

#include <kroll/kroll.h>
#include <Poco/Net/StreamSocket.h>
#include <Poco/Net/SocketReactor.h>
#include <Poco/Net/SocketAcceptor.h>
#include <Poco/Net/SocketNotification.h>
#include <Poco/NObserver.h>
#include <Poco/RunnableAdapter.h>
#include <Poco/Thread.h>

namespace Titanium {

class TCPServerSocketConnector {
public:
    TCPServerSocketConnector(KMethodRef callback,Poco::Net::ServerSocket& socket,Poco::Net::SocketReactor& reactor);
    virtual ~TCPServerSocketConnector();

    void onAccept(Poco::Net::ReadableNotification* pNotification);

private:
    KMethodRef callback;
    Poco::Net::ServerSocket& socket;
    Poco::Net::SocketReactor& reactor;
};

class TCPServerSocket : public StaticBoundObject {
public:
    TCPServerSocket(KMethodRef callback);
    virtual ~TCPServerSocket();

private:
    enum {
        BUFFER_SIZE = 1024
    };
    
    void ListenThread();
    void Listen(const ValueList& args, KValueRef result);
    void Close(const ValueList& args, KValueRef result);

    KMethodRef onCreate;
    Poco::Net::ServerSocket* socket;
    Poco::Net::SocketReactor reactor;
    TCPServerSocketConnector* acceptor;
    Poco::Thread listenThread;
    Poco::RunnableAdapter<TCPServerSocket>* listenAdapter;
    bool listening;
};
    
} // namespace Titanium

#endif
