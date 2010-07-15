/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2010 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _TCP_SERVER_SOCKET_BINDING_H_
#define _TCP_SERVER_SOCKET_BINDING_H_

#include <kroll/kroll.h>
#include <Poco/Thread.h>
#include <Poco/Net/StreamSocket.h>
#include <Poco/Net/SocketReactor.h>
#include <Poco/Net/SocketAcceptor.h>
#include <Poco/Net/SocketNotification.h>
#include <Poco/NObserver.h>
#include <Poco/RunnableAdapter.h>
#include "tcp_server_connection_binding.h"

/**
 *   Ti.Network.createTCPServer(function(c)
 * 	 {
 *		c.onread = function(data){
 *
 *      };
 *   }).listen(1000);
 *
 */
namespace ti
{
	class TCPServerSocketConnector
	{
	public:
		TCPServerSocketConnector(KMethodRef callback,Poco::Net::ServerSocket& socket,Poco::Net::SocketReactor& reactor);
		virtual ~TCPServerSocketConnector();
		void onAccept(Poco::Net::ReadableNotification* pNotification);
	private:
		KMethodRef callback;
		Poco::Net::ServerSocket& socket;
		Poco::Net::SocketReactor& reactor;
	};

	class TCPServerSocketBinding : public StaticBoundObject
	{
	public:
		TCPServerSocketBinding(Host *ti_host, KMethodRef callback);
		virtual ~TCPServerSocketBinding();
	private:
		enum
		{
			BUFFER_SIZE = 1024
		};
		KMethodRef onCreate;
		Poco::Net::ServerSocket* socket;
		Poco::Net::SocketReactor reactor;
		TCPServerSocketConnector* acceptor;
		Poco::Thread listenThread;
		Poco::RunnableAdapter<TCPServerSocketBinding>* listenAdapter;
		bool listening;
		
		void ListenThread();
		void Listen(const ValueList& args, KValueRef result);
		void Close(const ValueList& args, KValueRef result);
	};
	
}

#endif
