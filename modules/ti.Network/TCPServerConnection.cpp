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

#include "TCPServerConnection.h"

namespace Titanium {

static kroll::Logger* GetLogger()
{
    return kroll::Logger::Get("Network.TCPServerSocketConnection");
}

TCPServerConnection::TCPServerConnection(Poco::Net::StreamSocket& s, Poco::Net::SocketReactor & r)
    : StaticBoundObject("Network.TCPServerSocketConnection")
    , socket(s)
    , reactor(r)
    , closed(false)
    , onRead(0)
    , onWrite(0)
    , onReadComplete(0)
    , currentSendDataOffset(0)
    , readStarted(false)
    , writeReadyHandlerInstalled(false)
{
    GetLogger()->Debug("TCPServerConnection creating");
    
    /**
     * @tiapi(method=True,name=Network.TCPServerSocketConnection.close,since=1.2)
     * Close the Socket object's connection, if it is open.
     * @tiresult[Boolean] true if the connection was successfully close, false if otherwise
     */
    this->SetMethod("close", &TCPServerConnection::Close);

    /**
     * @tiapi(method=True,name=Network.TCPServerSocketConnection.write,since=1.2)
     * @tiapi Write data to the Socket's connection, if open.
     * @tiarg[String, data] The data to write to the connection.
     * @tiresult[Boolean] True if the write was successful, false otherwise.
     */
    this->SetMethod("write", &TCPServerConnection::Write);

    /**
     * @tiapi(method=True,name=Network.TCPServerSocketConnection.isClosed,since=1.2)
     * @tiapi Check whether the Socket is closed.
     * @tiresult[Boolean] true if a Socket object is closed, false if otherwise
     */
    this->SetMethod("isClosed",&TCPServerConnection::IsClosed);

    /**
     * @tiapi(method=True,name=Network.TCPServerSocketConnection.onRead,since=1.2)
     * @tiapi Set a callback that will be fired when data is received on the Socket.
     * @tiarg[Function, onRead] Function to be called when data is received.
     */
    this->SetMethod("onRead",&TCPServerConnection::SetOnRead);

    /**
     * @tiapi(method=True,name=Network.TCPServerSocketConnection.onWrite,since=1.2)
     * @tiapi Set a callback that will be fired when data is written on the Socket.
     * @tiarg[Function, onWrite] Function to be called when data is written.
     */
    this->SetMethod("onWrite",&TCPServerConnection::SetOnWrite);

    /**
     * @tiapi(method=True,name=Network.TCPServerSocketConnection.onError,version=1.2)
     * @tiapi Set the callback that will be fired when the Socket encounters an error.
     * @tiarg[Function, onError] Function to be called when an error happens.
     */
    this->SetMethod("onError",&TCPServerConnection::SetOnError);
    
    /**
     * @tiapi(method=True,name=Network.TCPServerSocketConnection.onReadComplete,since=1.2)
     * @tiapi Set the callback function that will be fired when a read finishes. A read is
     * @tiapi considered finished if some bytes have been read and a subsequent call to read
     * @tiapi returns zero bytes.
     * @tiarg[Function, onReadComplete] Function be called when a read completes.
     */
    this->SetMethod("onReadComplete",&TCPServerConnection::SetOnReadComplete);

    reactor.addEventHandler(socket, Poco::NObserver<TCPServerConnection, Poco::Net::ReadableNotification>(*this, &TCPServerConnection::onReadable));
    reactor.addEventHandler(socket, Poco::NObserver<TCPServerConnection, Poco::Net::ShutdownNotification>(*this, &TCPServerConnection::onShutdown));
    reactor.addEventHandler(socket, Poco::NObserver<TCPServerConnection, Poco::Net::ErrorNotification>(*this, &TCPServerConnection::onErrored));
}

TCPServerConnection::~TCPServerConnection()
{
    reactor.removeEventHandler(socket, Poco::NObserver<TCPServerConnection, Poco::Net::ReadableNotification>(*this, &TCPServerConnection::onReadable));
    reactor.removeEventHandler(socket, Poco::NObserver<TCPServerConnection, Poco::Net::ShutdownNotification>(*this, &TCPServerConnection::onShutdown));
    reactor.removeEventHandler(socket, Poco::NObserver<TCPServerConnection, Poco::Net::WritableNotification>(*this, &TCPServerConnection::onWritable));
    reactor.removeEventHandler(socket, Poco::NObserver<TCPServerConnection, Poco::Net::ErrorNotification>(*this, &TCPServerConnection::onErrored));

    if (!this->closed)
    {
        this->socket.close();
    }
}

void TCPServerConnection::onReadable (const Poco::AutoPtr<Poco::Net::ReadableNotification>& notification)
{
    if (this->closed)
    {
        return;
    }
    static std::string eprefix("TCPServerConnection::OnRead: ");
    
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

            BytesRef bytes(new Bytes(data, size));
            ValueList args(Value::NewObject(bytes));
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
        // sometimes we'll get a I/O error (9) after closing during
        // a read, we can safely ignore errors if closed
        if (!this->closed)
        {
            GetLogger()->Error("Read failed: %s", e.displayText().c_str());
            ValueList args(Value::NewString(e.displayText()));

            if (!this->onError.isNull())
                RunOnMainThread(this->onError, args, false);
        }
    }
    catch (...)
    {
        GetLogger()->Error("Read failed: unknown exception");
        ValueList args(Value::NewString("Unknown exception during read"));

        if (!this->onError.isNull())
            RunOnMainThread(this->onError, args, false);
    }
}

void TCPServerConnection::onShutdown (const Poco::AutoPtr<Poco::Net::ShutdownNotification>& notification)
{
    this->closed = true;
    
    //FIXME? what to do when we shutdown... do we need to auto-release?
}

void TCPServerConnection::onWritable (const Poco::AutoPtr<Poco::Net::WritableNotification>& notification)
{
    if (this->closed)
    {
        return;
    }

    if (sendData.empty())
    {
        this->reactor.removeEventHandler(socket, Poco::NObserver<TCPServerConnection, Poco::Net::WritableNotification>(*this, &TCPServerConnection::onWritable));
        writeReadyHandlerInstalled = false;
        return;
    }
    
    BytesRef buffer(0);
    {
        Poco::Mutex::ScopedLock lock(sendDataMutex);
        buffer = sendData.front();
    }

    const char* data = buffer->Pointer() + currentSendDataOffset;
    size_t length = buffer->Length() - currentSendDataOffset;
    size_t count = this->socket.sendBytes(data, length);
    currentSendDataOffset += count;

    if (currentSendDataOffset == (size_t) buffer->Length())
    {
        // Only send the onWrite message when we've exhausted a Bytes.
        if (!this->onWrite.isNull())
        {
            ValueList args(Value::NewInt(buffer->Length()));
            RunOnMainThread(this->onWrite, args, false);
        }

        Poco::Mutex::ScopedLock lock(sendDataMutex);
        sendData.pop();
        currentSendDataOffset = 0;

        // Uninstall the ReadyForWrite reactor handler, because it will push
        // the CPU to 100% usage if there is nothing to write.
        if (sendData.size() == 0)
        {
            this->reactor.removeEventHandler(socket, Poco::NObserver<TCPServerConnection, Poco::Net::WritableNotification>(*this, &TCPServerConnection::onWritable));
            writeReadyHandlerInstalled = false;
        }
    }
}

void TCPServerConnection::onErrored(const Poco::AutoPtr<Poco::Net::ErrorNotification>& notification)
{
    if (this->onError.isNull())
    {
        return;
    }
    ValueList args(Value::NewString(notification->name()));
    RunOnMainThread(this->onError, args, false);
}

void TCPServerConnection::Close(const ValueList& args, KValueRef result)
{
    if (!closed)
    {
        this->closed = true;
        socket.close();
        result->SetBool(true);
    }
    else
    {
        result->SetBool(false);
    }
}

void TCPServerConnection::Write(const ValueList& args, KValueRef result)
{
    args.VerifyException("Write", "o|s");

    static std::string eprefix("TCPServerConnection::Write: ");
    if (this->closed)
        throw ValueException::FromString(eprefix +  "Socket is not open");

    BytesRef data(0);
    if (args.at(0)->IsString())
    {
        std::string sendString(args.GetString(0));
        data = new Bytes(sendString.c_str(), sendString.size());
    }
    else if (args.at(0)->IsObject())
    {
        KObjectRef dataObject(args.GetObject(0));
        data = dataObject.cast<Bytes>();
    }

    if (data.isNull())
        throw ValueException::FromString("Cannot send non-Bytes object");

    {
        Poco::Mutex::ScopedLock lock(sendDataMutex);
        sendData.push(data);

        // Only install the ReadyForWrite handler when there is actually data
        // to write, because otherwise the CPU usage will spike to 100%
        if (!writeReadyHandlerInstalled)
        {
            this->reactor.addEventHandler(socket, Poco::NObserver<TCPServerConnection, Poco::Net::WritableNotification>(*this, &TCPServerConnection::onWritable));
            writeReadyHandlerInstalled = true;
        }
    }

    result->SetBool(true);
}

void TCPServerConnection::SetOnRead(const ValueList& args, KValueRef result)
{
    this->onRead = args.at(0)->ToMethod();
}

void TCPServerConnection::SetOnWrite(const ValueList& args, KValueRef result)
{
    this->onWrite = args.at(0)->ToMethod();
}

void TCPServerConnection::SetOnError(const ValueList& args, KValueRef result)
{
    this->onError = args.at(0)->ToMethod();
}

void TCPServerConnection::SetOnReadComplete(const ValueList& args, KValueRef result)
{
    this->onReadComplete = args.at(0)->ToMethod();
}

void TCPServerConnection::IsClosed(const ValueList& args, KValueRef result)
{
    return result->SetBool(this->closed);
}

} // namespace Titanium
