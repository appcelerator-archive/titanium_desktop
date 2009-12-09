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
	static kroll::Logger* GetLogger()
	{
		return kroll::Logger::Get("Network.TCPSocket");
	}

	TCPSocketBinding::TCPSocketBinding(Host* ti_host, std::string host, int port) :
		StaticBoundObject("Network.TCPSocket"),
		ti_host(ti_host),
		host(host),
		port(port),
		opened(false), 
		currentSendDataOffset(0),
		readStarted(false),
		onRead(0),
		onWrite(0),
		onTimeout(0),
		onReadComplete(0),
		readObserver(*this, &TCPSocketBinding::ReadyForRead),
		writeObserver(*this, &TCPSocketBinding::ReadyForWrite),
		timeoutObserver(*this, &TCPSocketBinding::OnTimeout),
		errorObserver(*this, &TCPSocketBinding::OnError)
	{
		/**
		 * @tiapi(method=True,name=Network.TCPSocket.connect,since=0.2)
		 * @tiapi Connect the Socket object to the host specified during creation
		 * @tiresult[Boolean] true if the connection was successful, false otherwise.
		 */
		this->SetMethod("connect", &TCPSocketBinding::Connect);

		/**
		 * @tiapi(method=True,name=Network.TCPSocket.close,since=0.2)
		 * Close the Socket object's connection, if it is open.
		 * @tiresult[Boolean] true if the connection was successfully close, false if otherwise
		 */
		this->SetMethod("close", &TCPSocketBinding::Close);

		/**
		 * @tiapi(method=True,name=Network.TCPSocket.write,since=0.2)
		 * @tiapi Write data to the Socket's connection, if open.
		 * @tiarg[String, data] The data to write to the connection.
		 * @tiresult[Boolean] True if the write was successful, false otherwise.
		 */
		this->SetMethod("write", &TCPSocketBinding::Write);

		/**
		 * @tiapi(method=True,name=Network.TCPSocket.isClosed,since=0.2)
		 * @tiapi Check whether the Socket is closed.
		 * @tiresult[Boolean] true if a Socket object is closed, false if otherwise
		 */
		this->SetMethod("isClosed",&TCPSocketBinding::IsClosed);

		/**
		 * @tiapi(method=True,name=Network.TCPSocket.onRead,since=0.2)
		 * @tiapi Set a callback that will be fired when data is received on the Socket.
		 * @tiarg[Function, onRead] Function to be called when data is received.
		 */
		this->SetMethod("onRead",&TCPSocketBinding::SetOnRead);

		/**
		 * @tiapi(method=True,name=Network.TCPSocket.onWrite,since=0.2)
		 * @tiapi Set a callback that will be fired when data is written on the Socket.
		 * @tiarg[Function, onWrite] Function to be called when data is written.
		 */
		this->SetMethod("onWrite",&TCPSocketBinding::SetOnWrite);

		/**
		 * @tiapi(method=True,name=Network.TCPSocket.onTimeout,since=0.2)
		 * @tiapi Set the callback that will be fired when an operation times out on the Socket.
		 * @tiarg[Function, onTimeout] Function to be called when an operation times out.
		 */
		this->SetMethod("onTimeout",&TCPSocketBinding::SetOnTimeout);

		/**
		 * @tiapi(method=True,name=Network.TCPSocket.onError,version=0.5)
		 * @tiapi Set the callback that will be fired when the Socket encounters an error.
		 * @tiarg[Function, onError] Function to be called when an error happens.
		 */
		this->SetMethod("onError",&TCPSocketBinding::SetOnError);

		/**
		 * @tiapi(method=True,name=Network.TCPSocket.onReadComplete,since=0.2)
		 * @tiapi Set the callback function that will be fired when a read finishes. A read is
		 * @tiapi considered finished if some bytes have been read and a subsequent call to read
		 * @tiapi returns zero bytes.
		 * @tiarg[Function, onReadComplete] Function be called when a read completes.
		 */
		this->SetMethod("onReadComplete",&TCPSocketBinding::SetOnReadComplete);

		// Attach reactor event handlers
		this->reactor.addEventHandler(this->socket, readObserver);
		this->reactor.addEventHandler(this->socket, writeObserver);
		this->reactor.addEventHandler(this->socket, timeoutObserver);
		this->reactor.addEventHandler(this->socket, errorObserver);
	}

	TCPSocketBinding::~TCPSocketBinding()
	{
		// Attach reactor event handlers
		this->reactor.removeEventHandler(this->socket, readObserver);
		this->reactor.removeEventHandler(this->socket, writeObserver);
		this->reactor.removeEventHandler(this->socket, timeoutObserver);
		this->reactor.removeEventHandler(this->socket, errorObserver);

		if (this->opened)
		{
			this->reactor.stop();
			this->socket.close();
		}
	}

	void TCPSocketBinding::SetOnRead(const ValueList& args, KValueRef result)
	{
		this->onRead = args.at(0)->ToMethod();
	}

	void TCPSocketBinding::SetOnWrite(const ValueList& args, KValueRef result)
	{
		this->onWrite = args.at(0)->ToMethod();
	}

	void TCPSocketBinding::SetOnTimeout(const ValueList& args, KValueRef result)
	{
		this->onTimeout = args.at(0)->ToMethod();
	}

	void TCPSocketBinding::SetOnError(const ValueList& args, KValueRef result)
	{
		this->onError = args.at(0)->ToMethod();
	}

	void TCPSocketBinding::SetOnReadComplete(const ValueList& args, KValueRef result)
	{
		this->onReadComplete = args.at(0)->ToMethod();
	}

	void TCPSocketBinding::IsClosed(const ValueList& args, KValueRef result)
	{
		return result->SetBool(!this->opened);
	}

	void TCPSocketBinding::Connect(const ValueList& args, KValueRef result)
	{
		int timeout = 10;
		if (args.size() > 0)
		{
			timeout = args.at(0)->ToInt();
		}

		static std::string eprefix("Connect exception: ");
		if (this->opened)
		{
			throw ValueException::FromString("Socket is already open");
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
		catch (Poco::IOException &e)
		{
			throw ValueException::FromString(eprefix + e.displayText());
		}
		catch (std::exception &e)
		{
			throw ValueException::FromString(eprefix + e.what());
		}
		catch (...)
		{
			throw ValueException::FromString(eprefix + "Unknown exception");
		}
	}

	void TCPSocketBinding::ReadyForRead(const Poco::AutoPtr<ReadableNotification>& n)
	{
		static std::string eprefix("TCPSocketBinding::OnRead: ");

		try
		{
			// Always read bytes, so that the tubes get cleared.
			char data[BUFFER_SIZE + 1];
			int size = socket.receiveBytes(&data, BUFFER_SIZE);

			// A read is only complete if we've already read some bytes from the socket.
			bool readComplete = this->readStarted && (size <= 0);
			this->readStarted = (size > 0);

			if (readComplete && !this->onReadComplete.isNull())
			{
				ValueList args;
				RunOnMainThread(this->onReadComplete, args, false);
			}
			else if (size > 0 && !this->onRead.isNull())
			{
				data[size] = '\0';

				BlobRef blob(new Blob(data, size));
				ValueList args(Value::NewObject(blob));
				RunOnMainThread(this->onRead, args, false);
			}
		}
		catch (ValueException& e)
		{
			GetLogger()->Error("Read failed: %s", e.ToString().c_str());
			ValueList args(Value::NewString(e.ToString()));

			if (!this->onError.isNull())
				RunOnMainThread(this->onError, args, false);
		}
		catch (Poco::Exception &e)
		{
			GetLogger()->Error("Read failed: %s", e.displayText().c_str());
			ValueList args(Value::NewString(e.displayText()));

			if (!this->onError.isNull())
				RunOnMainThread(this->onError, args, false);
		}
		catch (...)
		{
			GetLogger()->Error("Read failed: unknown exception");
			ValueList args(Value::NewString("Unknown exception during read"));

			if (!this->onError.isNull())
				RunOnMainThread(this->onError, args, false);
		}
	}

	void TCPSocketBinding::ReadyForWrite(const Poco::AutoPtr<WritableNotification>& n)
	{
		if (sendData.empty())
			return;

		BlobRef buffer(0);
		{
			Poco::Mutex::ScopedLock lock(sendDataMutex);
			buffer = sendData.front();
		}

		const char* data = buffer->Get() + currentSendDataOffset;
		size_t length = buffer->Length() - currentSendDataOffset;
		size_t count = this->socket.sendBytes(data, length);
		currentSendDataOffset += count;

		if (currentSendDataOffset == (size_t) buffer->Length())
		{
			// Only send the onWrite message when we've exhausted a Blob.
			if (!this->onWrite.isNull())
			{
				ValueList args(Value::NewInt(buffer->Length()));
				RunOnMainThread(this->onWrite, args, false);
			}

			Poco::Mutex::ScopedLock lock(sendDataMutex);
			sendData.pop();
		}
	}

	void TCPSocketBinding::OnTimeout(const Poco::AutoPtr<TimeoutNotification>& n)
	{
		if (this->onTimeout.isNull())
		{
			return;
		}
		RunOnMainThread(this->onTimeout, ValueList(), false);
	}

	void TCPSocketBinding::OnError(const Poco::AutoPtr<ErrorNotification>& n)
	{
		if (this->onError.isNull())
		{
			return;
		}
		ValueList args(Value::NewString(n->name()));
		RunOnMainThread(this->onError, args, false);
	}

	void TCPSocketBinding::Write(const ValueList& args, KValueRef result)
	{
		args.VerifyException("Send", "o|s");

		static std::string eprefix("TCPSocketBinding::Write: ");
		if (!this->opened)
			throw ValueException::FromString(eprefix +  "Socket is not open");

		BlobRef data(0);
		if (args.at(0)->IsString())
		{
			std::string sendString(args.GetString(0));
			data = new Blob(sendString.c_str(), sendString.size());
		}
		else if (args.at(0)->IsObject())
		{
			KObjectRef dataObject(args.GetObject(0));
			data = dataObject.cast<Blob>();
		}

		if (data.isNull())
			throw ValueException::FromString("Cannot send non-Blob object");

		{
			Poco::Mutex::ScopedLock lock(sendDataMutex);
			sendData.push(data);
		}

		result->SetBool(true);
	}

	void TCPSocketBinding::Close(const ValueList& args, KValueRef result)
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

