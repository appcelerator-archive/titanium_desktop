/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#include <kroll/kroll.h>
#include <kroll/thread_manager.h>
#include "irc_client_binding.h"
#include <cstring>

#ifdef OS_OSX
  #import <Cocoa/Cocoa.h>
#endif


namespace ti
{
	IRCClientBinding::IRCClientBinding(Host* host) : 
		StaticBoundObject("IRCClient"), host(host), global(host->GetGlobalObject()), thread(NULL)
	{
		/**
		 * @tiapi(property=True,type=Boolean,name=Network.IRCClient.connected,since=0.2) The connected property of an IRCClient object
		 */
		this->Set("connected",Value::NewBool(false));
		/**
		 * @tiapi(method=True,name=Network.IRCClient.connect,since=0.2) Connects an IRC to the host specified during creation of the IRCClient object
		 * @tiarg(for=Network.IRCClient.connect,type=String,name=hostname) the hostname 
		 * @tiarg(for=Network.IRCClient.connect,type=Number,name=port) the port
		 * @tiarg(for=Network.IRCClient.connect,type=String,name=nick) the users nickname
		 * @tiarg(for=Network.IRCClient.connect,type=String,name=name) the users full name
		 * @tiarg(for=Network.IRCClient.connect,type=String,name=user) the users login name
		 * @tiarg(for=Network.IRCClient.connect,type=String,name=pass) the users password
		 * @tiarg(for=Network.IRCClient.connect,type=Function,name=callback) a callback function to recieve IRC events.
		 */
		this->SetMethod("connect",&IRCClientBinding::Connect);
		/**
		 * @tiapi(method=True,name=Network.IRCClient.disconnect,since=0.2) Disconnects an IRC connection
		 */
		this->SetMethod("disconnect",&IRCClientBinding::Disconnect);
		/**
		 * @tiapi(method=True,name=Network.IRCClient.send,since=0.2) Sends data to the IRC connection
		 * @tiarg(for=Network.IRCClient.send,type=String,name=channel) the channel to send the data to
		 * @tiarg(for=Network.IRCClient.send,type=String,name=message) message to send
		 */
		this->SetMethod("send",&IRCClientBinding::Send);
		/**
		 * @tiapi(method=True,name=Network.IRCClient.setNick,since=0.2) Sets the nick name for the connection
		 * @tiarg(for=Network.IRCClient.setNick,type=String,name=nick) nickname to use
		 */
		this->SetMethod("setNick",&IRCClientBinding::SetNick);
		/**
		 * @tiapi(method=True,name=Network.IRCClient.getNick,since=0.2) Returns the nick name for the connection
		 * @tiresult(for=Network.IRCClient.getNick,type=String) the nickname for the current connection
		 */
		this->SetMethod("getNick",&IRCClientBinding::GetNick);
		/**
		 * @tiapi(method=True,name=Network.IRCClient.getUsers,since=0.2) Returns a list of users for the channel
		 * @tiresult(for=Network.IRCClient.getUsers,type=Array<String>) the users of the current channel
		 */
		this->SetMethod("getUsers",&IRCClientBinding::GetUsers);
		/**
		 * @tiapi(method=True,name=Network.IRCClient.join,since=0.2) Joins a channel
		 * @tiarg(for=Network.IRCClient.join,type=String,name=channel) channel to join to
		 */
		this->SetMethod("join",&IRCClientBinding::Join);
		/**
		 * @tiapi(method=True,name=Network.IRCClient.unjoin,since=0.2) Leaves a channel
		 * @tiarg(for=Network.IRCClient.unjoin,type=String,name=channel) channel to leave
		 */
		this->SetMethod("unjoin",&IRCClientBinding::Unjoin);
		/**
		 * @tiapi(method=True,name=Network.IRCClient.isOp,since=0.2) Checks whether a user has OP status
		 * @tiresult(for=Network.IRCClient.isOp,type=Boolean) true if the user has OP status, false if otherwise
		 */
		this->SetMethod("isOp",&IRCClientBinding::IsOp);
		/**
		 * @tiapi(method=True,returns=Boolean,name=Network.IRCClient.isVoice,since=0.2) Checks whether a user has VOICE status
		 * @tiresult(for=Network.IRCClient.isVoice,type=Boolean) true if the user has VOICE status, false if otherwise
		 */
		this->SetMethod("isVoice",&IRCClientBinding::IsVoice);

		// NULL is how we hook all commands (wildcard)
		this->irc.hook_irc_command(NULL,&IRCClientBinding::Callback,(void*)this);
		this->thread = new Poco::Thread();
	}
	IRCClientBinding::~IRCClientBinding()
	{
		bool connected = this->Get("connected")->ToBool();
		if (connected)
		{
			this->irc.quit("Leaving"); 
		}
		if (this->thread!=NULL)
		{
			delete this->thread;
			this->thread = NULL;
		}
	}
	int IRCClientBinding::Callback(char *irc_command, char* param, irc_reply_data* data, void* conn, void *pd)
	{
		PRINTD("IRC Received: " << param)
		
		IRCClientBinding *binding = (IRCClientBinding*)pd;
		if (!binding->callback.isNull())
		{
			ValueList args;
			args.push_back(irc_command ? Value::NewString(irc_command) : Value::Null);
			args.push_back(param ? Value::NewString(param) : Value::Null);
			args.push_back(data->target ? Value::NewString(data->target): Value::Null);
			args.push_back(data->nick ? Value::NewString(data->nick): Value::Null);

			try
			{
				binding->host->InvokeMethodOnMainThread(binding->callback,args,false);
			}
			catch(std::exception &e)
			{
				Logger* logger = Logger::Get("Network.IRC");
				logger->Error("Caught exception dispatching IRC callback: %s, Error: %s", irc_command, e.what());
			}
		}
		return 0;
	}
	void IRCClientBinding::Run (void* p)
	{
		START_KROLL_THREAD;

		IRC *irc = (IRC*)p;
		irc->message_loop();

		END_KROLL_THREAD;
	}
	void IRCClientBinding::GetUsers(const ValueList& args, SharedValue result)
	{
		const char *channel = args.at(0)->ToString();
		SharedKList list = new StaticBoundList();
		channel_user* cu = irc.get_users();
		while(cu)
		{
			if (!strcmp(cu->channel,(char*)channel) && cu->nick && strlen(cu->nick)>0)
			{
				SharedKObject entry = new StaticBoundObject();
				entry->Set("name",Value::NewString(cu->nick));
				entry->Set("operator",Value::NewBool(cu->flags & IRC_USER_OP));
				entry->Set("voice",Value::NewBool(cu->flags & IRC_USER_VOICE));
				list->Append(Value::NewObject(entry));
			}
			cu = cu->next;
		}
		result->SetList(list);
	}
	void IRCClientBinding::Connect(const ValueList& args, SharedValue result)
	{
		//TODO: check to make sure not connected already
		//TODO: check args
		std::string hostname = args.at(0)->ToString();
		int port = args.at(1)->ToInt();
		std::string nick = args.at(2)->ToString(); 
		std::string name = args.at(3)->ToString();
		std::string user = args.at(4)->ToString();
		std::string pass = args.at(5)->ToString();
		this->callback = args.at(6)->ToMethod();

		//char* server, int port, char* nick, char* user, char* name, char* pass
		this->irc.start((char*)hostname.c_str(),
						port,
						(char*)nick.c_str(),
						(char*)user.c_str(),
						(char*)name.c_str(),
						(char*)pass.c_str());

		this->Set("connected",Value::NewBool(true));
		this->thread->start(&IRCClientBinding::Run,&irc);
	}
	void IRCClientBinding::Disconnect(const ValueList& args, SharedValue result)
	{
		bool connected = this->Get("connected")->ToBool();
		if (connected)
		{
			const char *msg = args.size()>0 ? args.at(0)->ToString() : "Leaving";
			this->irc.quit((char*)msg); 
			this->Set("connected",Value::NewBool(false));
		}
	}
	void IRCClientBinding::Send(const ValueList& args, SharedValue result)
	{
		bool connected = this->Get("connected")->ToBool();
		if (connected)
		{
			const char *channel = args.at(0)->ToString();
			const char *msg = args.at(1)->ToString();
#ifdef DEBUG
			PRINTD("sending IRC: " << channel << " => " << msg);
#endif
			std::string cmd(msg);
			size_t pos = std::string::npos;
			// this is a little raw, we need to probably refactor
			// this to something more sane...
			if ((pos = cmd.find("/nick "))==0)
			{
				this->irc.nick((char*)cmd.substr(6).c_str());
			}
			else
			{
				this->irc.privmsg((char*)channel,(char*)msg);
			}
		}
	}
	void IRCClientBinding::SetNick(const ValueList& args, SharedValue result)
	{
		const char *nick = args.at(0)->ToString();
#ifdef DEBUG
		PRINTD("setNickname " << std::string(nick));
#endif
		this->irc.nick((char*)nick);
	}
	void IRCClientBinding::GetNick(const ValueList& args, SharedValue result)
	{
		std::string nick = this->irc.current_nick();
#ifdef DEBUG
		PRINTD("getNickname " << nick);
#endif
		result->SetString(nick);
	}
	void IRCClientBinding::Join(const ValueList& args, SharedValue result)
	{
		bool connected = this->Get("connected")->ToBool();
		if (connected)
		{
			const char *channel = args.at(0)->ToString();
#ifdef DEBUG
			PRINTD("JOIN " << channel);
#endif
			this->irc.join((char*)channel);
		}
	}
	void IRCClientBinding::Unjoin(const ValueList& args, SharedValue result)
	{
		bool connected = this->Get("connected")->ToBool();
		if (connected)
		{
			const char *channel = args.at(0)->ToString();
			this->irc.part((char*)channel);
		}
	}
	void IRCClientBinding::IsOp(const ValueList& args, SharedValue result)
	{
		//TODO:
	}
	void IRCClientBinding::IsVoice(const ValueList& args, SharedValue result)
	{
		//TODO:
	}
}
