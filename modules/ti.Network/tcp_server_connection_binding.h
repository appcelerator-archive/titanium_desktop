/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2010 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _TCP_SERVER_CONNECTION_BINDING_H_
#define _TCP_SERVER_CONNECTION_BINDING_H_

#include <kroll/kroll.h>
#include <Poco/Thread.h>
#include <Poco/Net/StreamSocket.h>
#include <Poco/Net/SocketReactor.h>
#include <Poco/Net/SocketAcceptor.h>
#include <Poco/Net/SocketNotification.h>
#include <Poco/NObserver.h>
#include <Poco/RunnableAdapter.h>
#include <queue>

namespace ti
{
	class TCPServerConnectionBinding : public StaticBoundObject
	{
	public:
		TCPServerConnectionBinding(Poco::Net::StreamSocket& s, Poco::Net::SocketReactor & reactor_);
		virtual ~TCPServerConnectionBinding();

	private:
		enum
		{
			BUFFER_SIZE = 1024
		};
		Poco::Net::StreamSocket socket;
		Poco::Net::SocketReactor& reactor;
		bool closed;
		KMethodRef onRead;
		KMethodRef onWrite;
		KMethodRef onError;
		KMethodRef onReadComplete;
		std::queue<BytesRef> sendData;
		Poco::Mutex sendDataMutex;
		size_t currentSendDataOffset;
		bool readStarted;
		bool writeReadyHandlerInstalled;

		void onReadable (const Poco::AutoPtr<Poco::Net::ReadableNotification>&);
		void onShutdown (const Poco::AutoPtr<Poco::Net::ShutdownNotification>&);
		void onWritable (const Poco::AutoPtr<Poco::Net::WritableNotification>&);
		void onErrored(const Poco::AutoPtr<Poco::Net::ErrorNotification>&);

		void Write(const ValueList& args, KValueRef result);
		void Close(const ValueList& args, KValueRef result);
		void IsClosed(const ValueList& args, KValueRef result);
		void SetOnRead(const ValueList& args, KValueRef result);
		void SetOnWrite(const ValueList& args, KValueRef result);
		void SetOnError(const ValueList& args, KValueRef result);
		void SetOnReadComplete(const ValueList& args, KValueRef result);

	};
	
}

#endif