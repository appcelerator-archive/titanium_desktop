/*
 * Copyright (c) 2008-2010 Appcelerator, Inc. All Rights Reserved.
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

#ifndef TCPSocket_h
#define TCPSocket_h

#include <queue>
#include <string>

#include <Poco/Net/StreamSocket.h>
#include <Poco/Thread.h>
#include <Poco/Mutex.h>

#include <kroll/kroll.h>

namespace Titanium {

class TCPSocket : public KEventObject {
public:
	TCPSocket(const std::string& host, int port);
	virtual ~TCPSocket();

	void Connect();
	bool Close();
	void Write(BytesRef data);
	void SetKeepAlive(bool enable);
	void SetTimeout(long milliseconds);

	void ReadThread();
	void WriteThread();

private:
    enum State {
	    CONNECTING,
	    READONLY,
	    WRITEONLY,
	    DUPLEX,
	    CLOSING,
	    CLOSED
	};

	void HandleError(Poco::Exception& e);

	void _Connect(const ValueList& args, KValueRef result);
	void _SetTimeout(const ValueList& args, KValueRef result);
	void _Close(const ValueList& args, KValueRef result);
	void _IsClosed(const ValueList& args, KValueRef result);
	void _Write(const ValueList& args, KValueRef result);
	void _OnRead(const ValueList& args, KValueRef result);
	void _OnReadComplete(const ValueList& args, KValueRef result);
	void _OnError(const ValueList& args, KValueRef result);
	void _OnTimeout(const ValueList& args, KValueRef result);

    State state;
	Poco::Net::SocketAddress address;
	Poco::Net::StreamSocket socket;
	Poco::Thread readThread;
	Poco::RunnableAdapter<TCPSocket> reader, writer;
	std::queue<BytesRef> writeQueue;
	Poco::FastMutex mutex;
};

} // namespace Titanium

#endif
