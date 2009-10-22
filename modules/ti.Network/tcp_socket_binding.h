/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _TCP_SOCKET_BINDING_H_
#define _TCP_SOCKET_BINDING_H_

#ifndef WINVER
#define WINVER 0x0501
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif

#ifndef _WIN32_WINDOWS
#define _WIN32_WINDOWS 0x0410
#endif

#ifndef _WIN32_IE
#define _WIN32_IE 0x600
#endif

#include <kroll/kroll.h>
#include <Poco/Thread.h>
#include <Poco/Net/StreamSocket.h>
#include <Poco/Net/SocketReactor.h>
#include <Poco/Net/SocketNotification.h>

using namespace Poco;
using namespace Poco::Net;

namespace ti
{
	class TCPSocketBinding : public StaticBoundObject
	{
	public:
		TCPSocketBinding(Host *ti_host, std::string host, int port);
		virtual ~TCPSocketBinding();
	private:
		Host* ti_host;
		std::string host;
		int port;
		StreamSocket socket;
		SocketReactor reactor;
		Thread thread;
		bool opened;
		std::string buffer;
		Poco::Mutex bufferMutex; 

		KMethodRef onRead;
		KMethodRef onWrite;
		KMethodRef onTimeout;
		KMethodRef onError;
		KMethodRef onReadComplete;

		void Connect(const ValueList& args, KValueRef result);
		void Write(const ValueList& args, KValueRef result);
		void Close(const ValueList& args, KValueRef result);
		void IsClosed(const ValueList& args, KValueRef result);
		void SetOnRead(const ValueList& args, KValueRef result);
		void SetOnWrite(const ValueList& args, KValueRef result);
		void SetOnTimeout(const ValueList& args, KValueRef result);
		void SetOnError(const ValueList& args, KValueRef result);
		void SetOnReadComplete(const ValueList& args, KValueRef result);

		void OnRead(const Poco::AutoPtr<ReadableNotification>& n);
		void OnWrite(const Poco::AutoPtr<WritableNotification>& n);
		void OnTimeout(const Poco::AutoPtr<TimeoutNotification>& n);
		void OnError(const Poco::AutoPtr<ErrorNotification>& n);
	};
}

#endif
