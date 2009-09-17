/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */
#include "tcp_socket_binding.h"
#include <Poco/NObserver.h>
#include <kroll/kroll.h>

#define BUFFER_SIZE 1024   // choose a reasonable size to send back to JS

namespace ti
{
	TCPSocketBinding::TCPSocketBinding(Host* ti_host, std::string host, int port) :
		StaticBoundObject("TCPSocket"),
		ti_host(ti_host), host(host), port(port), opened(false), 
		onRead(NULL), onWrite(NULL), onTimeout(NULL), onReadComplete(NULL)
	{
		/**
		 * @tiapi(method=True,name=Network.TCPSocket.connect,since=0.2) Connects a Socket object to the host specified during creation
		 * @tiresult(for=Network.TCPSocket.connect,type=Boolean) true if the Socket object successfully connects, false if otherwise
		 */
		this->SetMethod("connect",&TCPSocketBinding::Connect);
		/**
		 * @tiapi(method=True,name=Network.TCPSocket.close,since=0.2) Close the connection of a Socket object
		 * @tiresult(for=Network.TCPSocket.close,type=Boolean) true if the connection was successfully close, false if otherwise
		 */
		this->SetMethod("close",&TCPSocketBinding::Close);
		/**
		 * @tiapi(method=True,name=Network.TCPSocket.write,since=0.2) Writes data to a socket
		 * @tiarg(for=Network.TCPSocket.write,type=String,name=data) data to write
		 * @tiresult(for=Network.TCPSocket.write,type=Boolean) true if the data was successfully written to the socket, false if otherwise
		 */
		this->SetMethod("write",&TCPSocketBinding::Write);
		/**
		 * @tiapi(method=True,name=Network.TCPSocket.isClosed,since=0.2) Checks whether a Socket object is closed
		 * @tiresult(for=Network.TCPSocket.isClosed,type=Boolean) true if a Socket object is closed, false if otherwise
		 */
		this->SetMethod("isClosed",&TCPSocketBinding::IsClosed);

		// event handler callbacks
		/**
		 * @tiapi(method=True,name=Network.TCPSocket.onRead,since=0.2) Sets a callback function that will be fired when data is received from a socket
		 * @tiarg(for=Network.TCPSocket.onRead,type=Function,name=callback) callback function to be fired when data is received from a socket connection
		 */
		this->SetMethod("onRead",&TCPSocketBinding::SetOnRead);
		/**
		 * @tiapi(method=True,name=Network.TCPSocket.onWrite,since=0.2) Sets a callback function that will be fired when data is written to the socket
		 * @tiarg(for=Network.TCPSocket.onWrite,type=Function,name=callback) callback function to be fired when data is written to the socket
		 */
		this->SetMethod("onWrite",&TCPSocketBinding::SetOnWrite);
		/**
		 * @tiapi(method=True,name=Network.TCPSocket.onTimeout,since=0.2) Sets the callback function that will be fired when a socket times-out
		 * @tiarg(for=Network.TCPSocket.onTimeout,type=Function,name=callback) callback function to be fired when a socket times-out
		 */
		this->SetMethod("onTimeout",&TCPSocketBinding::SetOnTimeout);
		/**
		 * @tiapi(method=True,name=Network.TCPSocket.onError,version=0.5) Sets the callback function that will be fired when a socket throws an error
		 * @tiarg(for=Network.TCPSocket.onError,type=method,name=callback) callback function to be fired when a socket throws an error
		 */
		this->SetMethod("onError",&TCPSocketBinding::SetOnError);
		/**
		 * @tiapi(method=True,name=Network.TCPSocket.onReadComplete,since=0.2) Sets the callback function that will be fired when no more data is available
		 * @tiarg(for=Network.TCPSocket.onReadComplete,type=Function,name=callback) callback function be fired when no more data is available
		 */
		this->SetMethod("onReadComplete",&TCPSocketBinding::SetOnReadComplete);

		// our reactor event handlers
		this->reactor.addEventHandler(this->socket,NObserver<TCPSocketBinding, ReadableNotification>(*this, &TCPSocketBinding::OnRead));
		this->reactor.addEventHandler(this->socket,NObserver<TCPSocketBinding, WritableNotification>(*this, &TCPSocketBinding::OnWrite));
		this->reactor.addEventHandler(this->socket,NObserver<TCPSocketBinding, TimeoutNotification>(*this, &TCPSocketBinding::OnTimeout));
		this->reactor.addEventHandler(this->socket,NObserver<TCPSocketBinding, ErrorNotification>(*this, &TCPSocketBinding::OnError));
	}
	TCPSocketBinding::~TCPSocketBinding()
	{
		this->reactor.removeEventHandler(this->socket,NObserver<TCPSocketBinding, ReadableNotification>(*this, &TCPSocketBinding::OnRead));
		this->reactor.removeEventHandler(this->socket,NObserver<TCPSocketBinding, WritableNotification>(*this, &TCPSocketBinding::OnWrite));
		this->reactor.removeEventHandler(this->socket,NObserver<TCPSocketBinding, TimeoutNotification>(*this, &TCPSocketBinding::OnTimeout));
		this->reactor.removeEventHandler(this->socket,NObserver<TCPSocketBinding, ErrorNotification>(*this, &TCPSocketBinding::OnError));
		if (this->opened)
		{
			this->reactor.stop();
			this->socket.close();
		}
	}
	void TCPSocketBinding::SetOnRead(const ValueList& args, SharedValue result)
	{
		this->onRead = args.at(0)->ToMethod();
	}
	void TCPSocketBinding::SetOnWrite(const ValueList& args, SharedValue result)
	{
		this->onWrite = args.at(0)->ToMethod();
	}
	void TCPSocketBinding::SetOnTimeout(const ValueList& args, SharedValue result)
	{
		this->onTimeout = args.at(0)->ToMethod();
	}
	void TCPSocketBinding::SetOnError(const ValueList& args, SharedValue result)
	{
		this->onError = args.at(0)->ToMethod();
	}
	void TCPSocketBinding::SetOnReadComplete(const ValueList& args, SharedValue result)
	{
		this->onReadComplete = args.at(0)->ToMethod();
	}
	void TCPSocketBinding::IsClosed(const ValueList& args, SharedValue result)
	{
		return result->SetBool(!this->opened);
	}
	void TCPSocketBinding::Connect(const ValueList& args, SharedValue result)
	{
		int timeout = 10;
		if (args.size() > 0)
		{
			timeout = args.at(0)->ToInt();
		}
		std::string eprefix = "Connect exception: ";
		if (this->opened)
		{
			throw ValueException::FromString(eprefix + "Socket is already open");
		}
		try
		{
			SocketAddress a(this->host.c_str(),this->port);
			this->reactor.setTimeout(Poco::Timespan(timeout, 0));
			this->socket.connectNB(a);
			this->thread.start(this->reactor);
			this->opened = true;
			result->SetBool(true);
		}
		catch(Poco::IOException &e)
		{
			throw ValueException::FromString(eprefix + e.displayText());
		}
		catch(std::exception &e)
		{
			throw ValueException::FromString(eprefix + e.what());
		}
		catch(...)
		{
			throw ValueException::FromString(eprefix + "Unknown exception");
		}
	}
	void TCPSocketBinding::OnRead(const Poco::AutoPtr<ReadableNotification>& n)
	{
		std::string eprefix = "TCPSocketBinding::OnRead: ";
		try
		{
			// Always read bytes, so that the tubes get cleared.
			char data[BUFFER_SIZE + 1];
			int size = socket.receiveBytes(&data, BUFFER_SIZE);

			bool read_complete = (size <= 0);
			if (read_complete && !this->onReadComplete.isNull())
			{
				ValueList args;
				ti_host->InvokeMethodOnMainThread(this->onReadComplete, args, false);
			}
			else if (!read_complete && !this->onRead.isNull())
			{
				data[size] = '\0';

				ValueList args;
				args.push_back(Value::NewString(data));
				ti_host->InvokeMethodOnMainThread(this->onRead, args, false);
			}
		}
		catch(ValueException& e)
		{
			std::cerr << eprefix << *(e.GetValue()->DisplayString()) << std::endl;
			ValueList args(Value::NewString(e.ToString()));
			ti_host->InvokeMethodOnMainThread(this->onError, args, false);
		}
		catch(Poco::Exception &e)
		{
			std::cerr << eprefix << e.displayText() << std::endl;
			ValueList args(Value::NewString(e.displayText()));
			ti_host->InvokeMethodOnMainThread(this->onError, args, false);
		}
		catch(...)
		{
			std::cerr << eprefix << "Unknown exception" << std::endl;
			ValueList args(Value::NewString("Unknown exception"));
			ti_host->InvokeMethodOnMainThread(this->onError, args, false);
		}
	}
	void TCPSocketBinding::OnWrite(const Poco::AutoPtr<WritableNotification>& n)
	{
		int count = 0;

		{
			Poco::Mutex::ScopedLock lock(bufferMutex);
			if (!buffer.empty())
			{
				count = this->socket.sendBytes(buffer.c_str(), buffer.length());
				buffer.clear();
			}
		}

		if (!this->onWrite.isNull())
		{
			ValueList args;
			args.push_back(Value::NewInt(count));
			ti_host->InvokeMethodOnMainThread(this->onWrite, args, false);
		}
		else
		{
			Poco::Thread::sleep(100);
		}
	}
	void TCPSocketBinding::OnTimeout(const Poco::AutoPtr<TimeoutNotification>& n)
	{
		if (this->onTimeout.isNull())
		{
			return;
		}
		ValueList args;
		ti_host->InvokeMethodOnMainThread(this->onTimeout, args, false);
	}
	void TCPSocketBinding::OnError(const Poco::AutoPtr<ErrorNotification>& n)
	{
		if (this->onError.isNull())
		{
			return;
		}
		ValueList args;
		args.push_back(Value::NewString(n->name()));
		ti_host->InvokeMethodOnMainThread(this->onError, args, false);
	}
	void TCPSocketBinding::Write(const ValueList& args, SharedValue result)
	{
		std::string eprefix = "TCPSocketBinding::Write: ";
		if (!this->opened)
		{
			throw ValueException::FromString(eprefix +  "Socket is not open");
		}

		try
		{
			Poco::Mutex::ScopedLock lock(bufferMutex);
			buffer += args.at(0)->ToString();
			result->SetBool(true);
		}
		catch(Poco::Exception &e)
		{
			throw ValueException::FromString(eprefix + e.displayText());
		}

	}
	void TCPSocketBinding::Close(const ValueList& args, SharedValue result)
	{
		if (this->opened)
		{
			this->opened = false;
			this->reactor.stop();
			this->socket.close();
			result->SetBool(true);
		}
		else
		{
			result->SetBool(false);
		}
	}
}

