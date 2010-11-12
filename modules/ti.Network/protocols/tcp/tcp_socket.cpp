/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008-2010 Appcelerator, Inc. All Rights Reserved.
 */

#include "tcp_socket.h"

#include <Poco/ThreadPool.h>
#include <Poco/Timespan.h>

#define READ_BUFFER_SIZE 40*1024
#define READ_BUFFER_MIN_SIZE 128

namespace ti
{
	TCPSocket::TCPSocket(std::string& host, int port) :
		KEventObject("Network.TCPSocket"),
		address(host, port),
		socket(address.family()),
		state(CLOSED),
		reader(*this, &TCPSocket::ReadThread),
		writer(*this, &TCPSocket::WriteThread)
	{
		SetMethod("connect", &TCPSocket::_Connect);
		SetMethod("setTimeout", &TCPSocket::_SetTimeout);
		SetMethod("close", &TCPSocket::_Close);
		SetMethod("isClosed", &TCPSocket::_IsClosed);
		SetMethod("write", &TCPSocket::_Write);
		SetMethod("onRead", &TCPSocket::_OnRead);
		SetMethod("onReadComplete", &TCPSocket::_OnReadComplete);
		SetMethod("onError", &TCPSocket::_OnError);
		SetMethod("onTimeout", &TCPSocket::_OnTimeout);
	}

	TCPSocket::~TCPSocket()
	{
	}

	void TCPSocket::Connect()
	{
		Poco::FastMutex::ScopedLock lock(this->mutex);

		if (this->state != CLOSED)
			throw ValueException::FromString("socket is already connected");

		// Start up the reading thread.
		// This thread will establish the connection
		// and then begin reading data from the socket.
		this->state = CONNECTING;
		this->readThread.start(reader);
	}

	bool TCPSocket::Close()
	{
		{
			Poco::FastMutex::ScopedLock lock(this->mutex);

			if (this->state == CLOSED)
				return false;

			this->socket.close();
			this->state = CLOSED;

			// Delete any remaining buffers in write queue.
			this->writeQueue = std::queue<BytesRef>();
		}

		FireEvent("close");
		return true;
	}

	void TCPSocket::Write(BytesRef data)
	{
		Poco::FastMutex::ScopedLock lock(this->mutex);
		if (this->state != DUPLEX && this->state != WRITEONLY)
			throw ValueException::FromString("Socket is not writable");

		if (this->writeQueue.empty())
		{
			Poco::ThreadPool::defaultPool().start(this->writer);
		}

		this->writeQueue.push(data);
	}

	void TCPSocket::SetKeepAlive(bool enable)
	{
		this->socket.setKeepAlive(enable);
	}

	void TCPSocket::SetTimeout(long milliseconds)
	{
		try
		{
			Poco::Timespan t(0, milliseconds * 1000);
			this->socket.setReceiveTimeout(t);
		}
		catch (Poco::Exception& e)
		{
			HandleError(e);
		}
	}

	void TCPSocket::ReadThread()
	{
		try
		{
			this->socket.connect(this->address);

			{
				Poco::FastMutex::ScopedLock lock(this->mutex);
				this->state = DUPLEX;
			}

			FireEvent("connect");
		}
		catch (Poco::Exception& e)
		{
			HandleError(e);
			return;
		}

		BytesRef buffer = new Bytes(READ_BUFFER_SIZE);
		size_t usedSpace = 0;

		while (true)
		{
			// Re-allocate a new read buffer if the current
			// one has become too small.
			int freeSpace = READ_BUFFER_SIZE - usedSpace;
			if (freeSpace < READ_BUFFER_MIN_SIZE)
			{
				buffer = new Bytes(READ_BUFFER_SIZE);
				usedSpace = 0;
				freeSpace = READ_BUFFER_SIZE;
			}

			// Attempt to read data from socket into buffer.
			try
			{
				char* bufferPtr = buffer->Pointer() + usedSpace;
				int bytesRecv = this->socket.receiveBytes(bufferPtr, freeSpace);
				if (bytesRecv > 0)
				{
					BytesRef data = new Bytes(buffer, usedSpace, bytesRecv);
					usedSpace += bytesRecv;
					this->FireEvent("data", ValueList(Value::NewObject(data)));
				}
				else
				{
					// Remote host sent FIN, we are now write only.
					{
						Poco::FastMutex::ScopedLock lock(this->mutex);
						this->state = WRITEONLY;
					}

					FireEvent("end");
					return;
				}
			}
			catch (Poco::TimeoutException& e)
			{
				this->FireEvent("timeout");
			}
			catch (Poco::Exception& e)
			{
				HandleError(e);
				return;
			}
		}
	}

	void TCPSocket::WriteThread()
	{
		this->mutex.lock();
		while (!this->writeQueue.empty())
		{
			BytesRef data = this->writeQueue.front();
			char* buffer = data->Pointer();
			size_t remaining = data->Length();

			// Release lock while sending data to avoid blocking write().
			this->mutex.unlock();
			while (true)
			{
				try
				{
					size_t sent = this->socket.sendBytes(buffer, remaining);
					if (sent == remaining) break;

					buffer += sent;
					remaining -= sent;
				}
				catch (Poco::Exception& e)
				{
					HandleError(e);
					return;
				}
			}

			this->mutex.lock();
			this->writeQueue.pop();
		}

		// Notify listeners we have fully drained the queue.
		this->mutex.unlock();
		FireEvent("drain");
	}

	void TCPSocket::HandleError(Poco::Exception& e)
	{
		{
			Poco::FastMutex::ScopedLock lock(this->mutex);
			if (this->state == CLOSED || this->state == CLOSING)
				return;
			this->state = CLOSING;
		}

		FireErrorEvent(e);
		Close();
	}

	void TCPSocket::_Connect(const ValueList& args, KValueRef result)
	{
		Connect();
	}

	void TCPSocket::_SetTimeout(const ValueList& args, KValueRef result)
	{
		args.VerifyException("setTimeout", "n");
		SetTimeout((long)args.GetNumber(0));
	}

	void TCPSocket::_Close(const ValueList& args, KValueRef result)
	{
		result->SetBool(Close());
	}

	void TCPSocket::_IsClosed(const ValueList& args, KValueRef result)
	{
		Poco::FastMutex::ScopedLock lock(this->mutex);
		result->SetBool(this->state == CLOSED);
	}

	void TCPSocket::_Write(const ValueList& args, KValueRef result)
	{
		args.VerifyException("write", "s|o");

		BytesRef data;
		if (args.at(0)->IsString())
		{
			std::string dataStr(args.GetString(0));
			data = new Bytes(dataStr);
		}
		else
		{
			data = args.GetObject(0).cast<Bytes>();
			if (data.isNull())
			{
				throw ValueException::FromString("Argument is not Bytes object");
			}
		}

		Write(data);
	}

	void TCPSocket::_OnRead(const ValueList& args, KValueRef result)
	{
		args.VerifyException("onRead", "m");
		AddEventListener("data", args.GetMethod(0));
	}

	void TCPSocket::_OnReadComplete(const ValueList& args, KValueRef result)
	{
		args.VerifyException("onReadComplete", "m");
		AddEventListener("end", args.GetMethod(0));
	}

	void TCPSocket::_OnError(const ValueList& args, KValueRef result)
	{
		args.VerifyException("onError", "m");
		AddEventListener("error", args.GetMethod(0));
	}

	void TCPSocket::_OnTimeout(const ValueList& args, KValueRef result)
	{
		args.VerifyException("onTimeout", "m");
		AddEventListener("timeout", args.GetMethod(0));
	}
}

