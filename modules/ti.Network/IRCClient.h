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

#ifndef IRCClient_h
#define IRCClient_h

#ifdef OS_LINUX
#include <unistd.h>
#endif

#include <kroll/kroll.h>
#include <Poco/Thread.h>

#include "IRC.h"

namespace Titanium {

class IRCClient : public StaticBoundObject {
public:
	IRCClient();
	virtual ~IRCClient();

private:	
	static void Run(void*);
	static int Callback(char *cmd, char* params, irc_reply_data* data, void* conn, void* pd);
	
	void Connect(const ValueList& args, KValueRef result);
	void Disconnect(const ValueList& args, KValueRef result);
	void Send(const ValueList& args, KValueRef result);
	void SetNick(const ValueList& args, KValueRef result);
	void GetNick(const ValueList& args, KValueRef result);
	void Join(const ValueList& args, KValueRef result);
	void Unjoin(const ValueList& args, KValueRef result);
	void IsOp(const ValueList& args, KValueRef result);
	void IsVoice(const ValueList& args, KValueRef result);
	void GetUsers(const ValueList& args, KValueRef result);

	KObjectRef global;
	IRC irc;
	KMethodRef callback;
	Poco::Thread *thread;
};

} // namespace Titanium

#endif
