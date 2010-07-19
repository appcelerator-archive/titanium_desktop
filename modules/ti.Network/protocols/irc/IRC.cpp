/*
	cpIRC - C++ class based IRC protocol wrapper
	Copyright (C) 2003 Iain Sheppard

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

	Contacting the author:
	~~~~~~~~~~~~~~~~~~~~~~

	email:	iainsheppard@yahoo.co.uk
	IRC:	#magpie @ irc.quakenet.org


	Numeorous minor changes by Jeff Haynie <jhaynie@appcelerator.org> to
	get it working well under Titanium
*/

#include "IRC.h"
#ifdef OS_WIN32
#include <windows.h>
#include <winsock.h>
#else
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <cstdio>
#define closesocket(s) close(s)
#define SOCKET_ERROR -1
#define INVALID_SOCKET -1
#endif

#include <iostream>

IRC::IRC()
{
	hooks=0;
	chan_users=0;
	connected=false;
	connecting=false;
	sentnick=false;
	sentpass=false;
	sentuser=false;
	cur_nick=0;
}

IRC::~IRC()
{
	if (hooks)
		delete_irc_command_hook(hooks);
	if (cur_nick)
		delete [] cur_nick;
	cur_nick = 0;
}

void IRC::insert_irc_command_hook(irc_command_hook* hook, char* cmd_name, int (*function_ptr)(char *, char*, irc_reply_data*, void*, void*), void *pd)
{
	if (hook->function)
	{
		if (!hook->next)
		{
			hook->next=new irc_command_hook;
			hook->next->function=0;
			hook->next->irc_command=0;
			hook->next->next=0;
			hook->next->private_data = pd;
		}
		insert_irc_command_hook(hook->next, cmd_name, function_ptr, pd);
	}
	else
	{
		hook->function=function_ptr;
		if (cmd_name == NULL)
		{
			hook->irc_command = NULL;
		}
		else
		{
			hook->irc_command=new char[strlen(cmd_name)+1];
			strcpy(hook->irc_command, cmd_name);
		}
	}
}

void IRC::hook_irc_command(char* cmd_name, int (*function_ptr)(char*, char*, irc_reply_data*, void*, void*), void *pd)
{
	if (!hooks)
	{
		hooks=new irc_command_hook;
		hooks->function=0;
		hooks->irc_command=0;
		hooks->next=0;
		hooks->private_data=pd;
		insert_irc_command_hook(hooks, cmd_name, function_ptr,pd);
	}
	else
	{
		insert_irc_command_hook(hooks, cmd_name, function_ptr,pd);
	}
}

void IRC::delete_irc_command_hook(irc_command_hook* cmd_hook)
{
	if (cmd_hook->next)
		delete_irc_command_hook(cmd_hook->next);
	if (cmd_hook->irc_command)
		delete cmd_hook->irc_command;
	delete cmd_hook;
}

void IRC::send(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	char szBuf[1024];
#ifdef OS_WIN32
	int size = vsprintf_s(szBuf, 1024, fmt, args);
#else
	int size = vsprintf(szBuf, fmt, args);
#endif
	if (size > 0)
	{
#ifdef DEBUG
		PRINTD("IRC sending[" << size << "] => " << szBuf);
#endif
		::send(irc_socket,(const char*)szBuf,size,0);
	}
}

int IRC::start(char* server, int port, char* nick, char* user, char* name, char* pass)
{
	#ifdef WIN32
	HOSTENT* resolv;
	#else
	hostent* resolv;
	#endif
	sockaddr_in rem;

	if (connected)
		return 1;

	irc_socket=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (irc_socket==INVALID_SOCKET)
	{
		return 1;
	}
	resolv=gethostbyname(server);
	if (!resolv)
	{
		closesocket(irc_socket);
		return 1;
	}
	memcpy(&rem.sin_addr, resolv->h_addr, 4);
	rem.sin_family=AF_INET;
	rem.sin_port=htons(port);

	connecting = true;

	if (connect(irc_socket, (const sockaddr*)&rem, sizeof(rem))==SOCKET_ERROR)
	{
#ifdef WIN32
		Logger* logger = Logger::Get("Network.IRC");
		logger->Error("Failed to connect: %i", WSAGetLastError());
#endif
		connecting = false;
		closesocket(irc_socket);
		return 1;
	}

	cur_nick=new char[strlen(nick)+1];
	strcpy(cur_nick, nick);

	send("PASS %s\r\n", pass);
	send("NICK %s\r\n", cur_nick);
	send("USER %s * 0 :%s\r\n", user, name);

	connected=true;
	connecting=false;

	return 0;
}

void IRC::disconnect()
{
	if (connected)
	{
		PRINTD("Disconnected from server");
		connected=false;
		quit("Leaving");
#ifdef WIN32
		shutdown(irc_socket, 2);
#endif
		closesocket(irc_socket);
	}
}

int IRC::quit(const char* quit_message)
{
	if (connected)
	{
		if (quit_message)
			send("QUIT %s\r\n", quit_message);
		else
			send("QUIT\r\n");
		return 0;
	}
	return 0;
}

int IRC::message_loop()
{
	char buffer[4096];
	int ret_len;

	PRINTD("ENTER IRC::message_loop()");

	if (!connected)
	{
		while (!connected && connecting)
		{
#ifdef WIN32
			Sleep(1);
#else
			sleep(1);
#endif
		}
	}

	int rc = 0;

	while (connected)
	{
		ret_len=recv(irc_socket, buffer, 4095, 0);
		if (ret_len==SOCKET_ERROR || !ret_len)
		{
			rc = 1;
			break;
		}
		buffer[ret_len]='\0';
		try
		{
			split_to_replies(buffer);
		}
		catch(std::exception &e)
		{
			Logger* logger = Logger::Get("Network.IRC");
			logger->Error("ERROR DISPATCHING IRC RECEIVE BUFFER. Error=%s", e.what());
		}
		catch(...)
		{
			Logger* logger = Logger::Get("Network.IRC");
			logger->Error("UNKNOWN EXCEPTION IN IRC RECEIVE THREAD...");
		}
	}

	PRINTD("EXIT IRC::message_loop() => " << rc);
	return rc;
}

void IRC::split_to_replies(char* data)
{
	char* p;

	while ((p=strstr(data, "\r\n")))
	{
		*p='\0';
		parse_irc_reply(data);
		data=p+2;
	}
}

int IRC::is_op(char* channel, char* nick)
{
	channel_user* cup;

	cup=chan_users;

	while (cup)
	{
		if (!strcmp(cup->channel, channel) && !strcmp(cup->nick, nick))
		{
			return cup->flags&IRC_USER_OP;
		}
		cup=cup->next;
	}

	return 0;
}

int IRC::is_voice(char* channel, char* nick)
{
	channel_user* cup;

	cup=chan_users;

	while (cup)
	{
		if (!strcmp(cup->channel, channel) && !strcmp(cup->nick, nick))
		{
			return cup->flags&IRC_USER_VOICE;
		}
		cup=cup->next;
	}

	return 0;
}

channel_user* IRC::get_users()
{
	return chan_users;
}

void IRC::parse_irc_reply(char* data)
{
	char* hostd;
	char* cmd;
	char* params;
	irc_reply_data hostd_tmp;
	channel_user* cup;
	char* p;
	char* chan_temp;

	hostd_tmp.target=0;

	PRINTD("IRC INCOMING: " << data);

	if (data[0]==':')
	{
		hostd=&data[1];
		cmd=strchr(hostd, ' ');
		if (!cmd)
			return;
		*cmd='\0';
		cmd++;
		params=strchr(cmd, ' ');
		if (params)
		{
			*params='\0';
			params++;
		}
		hostd_tmp.nick=hostd;
		hostd_tmp.ident=strchr(hostd, '!');
		if (hostd_tmp.ident)
		{
			*hostd_tmp.ident='\0';
			hostd_tmp.ident++;
			hostd_tmp.host=strchr(hostd_tmp.ident, '@');
			if (hostd_tmp.host)
			{
				*hostd_tmp.host='\0';
				hostd_tmp.host++;
			}
		}

		if (!strcmp(cmd, "JOIN"))
		{
			cup=chan_users;
			if (cup)
			{
				while (cup->nick)
				{
					if (!cup->next)
					{
						cup->next=new channel_user;
						cup->next->channel=0;
						cup->next->flags=0;
						cup->next->next=0;
						cup->next->nick=0;
					}
					cup=cup->next;
				}
				cup->channel=new char[strlen(params)+1];
				strcpy(cup->channel, params);
				cup->nick=new char[strlen(hostd_tmp.nick)+1];
				strcpy(cup->nick, hostd_tmp.nick);
			}
		}
		else if (!strcmp(cmd, "PART"))
		{
			channel_user* d;
			channel_user* prev;

			d=0;
			prev=0;
			cup=chan_users;
			while (cup)
			{
				if (!strcmp(cup->channel, params) && !strcmp(cup->nick, hostd_tmp.nick))
				{
					d=cup;
					break;
				}
				else
				{
					prev=cup;
				}
				cup=cup->next;
			}
			if (d)
			{
				if (d==chan_users)
				{
					chan_users=d->next;
					if (d->channel)
						delete [] d->channel;
					if (d->nick)
						delete [] d->nick;
					delete d;
				}
				else
				{
					if (prev)
					{
						prev->next=d->next;
					}
					chan_users=d->next;
					if (d->channel)
						delete [] d->channel;
					if (d->nick)
						delete [] d->nick;
					delete d;
				}
			}
		}
		else if (!strcmp(cmd, "QUIT"))
		{
			channel_user* d;
			channel_user* prev;

			d=0;
			prev=0;
			cup=chan_users;
			while (cup)
			{
				if (!strcmp(cup->nick, hostd_tmp.nick))
				{
					d=cup;
					if (d==chan_users)
					{
						chan_users=d->next;
						if (d->channel)
							delete [] d->channel;
						if (d->nick)
							delete [] d->nick;
						delete d;
					}
					else
					{
						if (prev)
						{
							prev->next=d->next;
						}
						if (d->channel)
							delete [] d->channel;
						if (d->nick)
							delete [] d->nick;
						delete d;
					}
					break;
				}
				else
				{
					prev=cup;
				}
				cup=cup->next;
			}
		}
		else if (!strcmp(cmd, "MODE"))
		{
			char* chan;
			char* changevars;
			channel_user* cup;
			channel_user* d;
			char* tmp;
			int i;
			bool plus;

			chan=params;
			params=strchr(chan, ' ');
			*params='\0';
			params++;
			changevars=params;
			params=strchr(changevars, ' ');
			if (!params)
			{
				return;
			}
			if (chan[0]!='#')
			{
				return;
			}
			*params='\0';
			params++;

			plus=false;
			for (i=0; i<(signed)strlen(changevars); i++)
			{
				switch (changevars[i])
				{
				case '+':
					plus=true;
					break;
				case '-':
					plus=false;
					break;
				case 'o':
					tmp=strchr(params, ' ');
					if (tmp)
					{
						*tmp='\0';
						tmp++;
					}
					tmp=params;
					if (plus)
					{
						// user has been opped (chan, params)
						cup=chan_users;
						d=0;
						while (cup)
						{
							if (cup->next && cup->channel)
							{
								if (!strcmp(cup->channel, chan) && !strcmp(cup->nick, tmp))
								{
									d=cup;
									break;
								}
							}
							cup=cup->next;
						}
						if (d)
						{
							d->flags=d->flags|IRC_USER_OP;
						}
					}
					else
					{
						// user has been deopped (chan, params)
						cup=chan_users;
						d=0;
						while (cup)
						{
							if (!strcmp(cup->channel, chan) && !strcmp(cup->nick, tmp))
							{
								d=cup;
								break;
							}
							cup=cup->next;
						}
						if (d)
						{
							d->flags=d->flags^IRC_USER_OP;
						}
					}
					params=tmp;
					break;
				case 'v':
					tmp=strchr(params, ' ');
					if (tmp)
					{
						*tmp='\0';
						tmp++;
					}
					if (plus)
					{
						// user has been voiced
						cup=chan_users;
						d=0;
						while (cup)
						{
							if (!strcmp(cup->channel, params) && !strcmp(cup->nick, hostd_tmp.nick))
							{
								d=cup;
								break;
							}
							cup=cup->next;
						}
						if (d)
						{
							d->flags=d->flags|IRC_USER_VOICE;
						}
					}
					else
					{
						// user has been devoiced
						cup=chan_users;
						d=0;
						while (cup)
						{
							if (!strcmp(cup->channel, params) && !strcmp(cup->nick, hostd_tmp.nick))
							{
								d=cup;
								break;
							}
							cup=cup->next;
						}
						if (d)
						{
							d->flags=d->flags^IRC_USER_VOICE;
						}
					}
					params=tmp;
					break;
				default:
					return;
					break;
				}
				// ------------ END OF MODE ---------------
			}
		}
		else if (!strcmp(cmd, "353"))
		{
			// receiving channel names list
			if (!chan_users)
			{
				chan_users=new channel_user;
				chan_users->next=0;
				chan_users->nick=0;
				chan_users->flags=0;
				chan_users->channel=0;
			}
			cup=chan_users;
			chan_temp=strchr(params, '#');
			if (chan_temp)
			{
				//chan_temp+=3;
				p=strstr(chan_temp, " :");
				if (p)
				{
					*p='\0';
					p+=2;
					while (strchr(p, ' '))
					{
						char* tmp;

						tmp=strchr(p, ' ');
						*tmp='\0';
						tmp++;
						while (cup->nick)
						{
							if (!cup->next)
							{
								cup->next=new channel_user;
								cup->next->channel=0;
								cup->next->flags=0;
								cup->next->next=0;
								cup->next->nick=0;
							}
							cup=cup->next;
						}
						if (p[0]=='@')
						{
							cup->flags=cup->flags|IRC_USER_OP;
							p++;
						}
						else if (p[0]=='+')
						{
							cup->flags=cup->flags|IRC_USER_VOICE;
							p++;
						}
						cup->nick=new char[strlen(p)+1];
						strcpy(cup->nick, p);
						cup->channel=new char[strlen(chan_temp)+1];
						strcpy(cup->channel, chan_temp);
						p=tmp;
					}
					while (cup->nick)
					{
						if (!cup->next)
						{
							cup->next=new channel_user;
							cup->next->channel=0;
							cup->next->flags=0;
							cup->next->next=0;
							cup->next->nick=0;
						}
						cup=cup->next;
					}
					if (p[0]=='@')
					{
						cup->flags=cup->flags|IRC_USER_OP;
						p++;
					}
					else if (p[0]=='+')
					{
						cup->flags=cup->flags|IRC_USER_VOICE;
						p++;
					}
					cup->nick=new char[strlen(p)+1];
					strcpy(cup->nick, p);
					cup->channel=new char[strlen(chan_temp)+1];
					strcpy(cup->channel, chan_temp);
				}
			}
		}
		else if (!strcmp(cmd, "NOTICE"))
		{
			hostd_tmp.target=params;
			params=strchr(hostd_tmp.target, ' ');
			if (params)
				*params='\0';
			params++;
		}
		else if (!strcmp(cmd, "PRIVMSG"))
		{
			hostd_tmp.target=params;
			params=strchr(hostd_tmp.target, ' ');
			if (!params)
				return;
			*(params++)='\0';
			std::string d(&params[1]);
			std::string h(hostd_tmp.nick);
			if (d.find("VERSION")==1 && h.find("freenode-connect")==0)
			{
				// ignore this message
				return;
			}
			#ifdef __IRC_DEBUG__
			PRINTD(hostd_tmp.target << ": <" << hostd_tmp.nick << "> " << &params[1]);
			#endif
		}
		else if (!strcmp(cmd, "NICK"))
		{
//			if (!strcmp(hostd_tmp.nick, cur_nick))
			// if the nicks don't match get the new one.
			if (strcmp(hostd_tmp.nick, cur_nick))
			{
				delete [] cur_nick;
				cur_nick=new char[strlen(params)+1];
				strcpy(cur_nick, params);
			}
		}
		/* else if (!strcmp(cmd, ""))
		{
			#ifdef __IRC_DEBUG__
			#endif
		} */
		call_hook(cmd, params, &hostd_tmp);
	}
	else
	{
		cmd=data;
		data=strchr(cmd, ' ');
		if (!data)
			return;
		*data='\0';
		params=data+1;

		if (!strcmp(cmd, "PING"))
		{
			if (!params)
				return;
			send("PONG %s\r\n", &params[1]);
		}
		else
		{
			hostd_tmp.host=0;
			hostd_tmp.ident=0;
			hostd_tmp.nick=0;
			hostd_tmp.target=0;
			call_hook(cmd, params, &hostd_tmp);
		}
	}
}

void IRC::call_hook(char* irc_command, char* params, irc_reply_data* hostd)
{
	irc_command_hook* p;

	if (!hooks)
		return;

	p=hooks;
	while (p)
	{
		//JGH: added wildcard ability by setting null
		if (p->irc_command == NULL || !strcmp(p->irc_command, irc_command))
		{
			//JGH: added private data pointer in callback
			(*(p->function))(irc_command, params, hostd, this, p->private_data);
			p=0;
		}
		else
		{
			p=p->next;
		}
	}
}

int IRC::notice(char* target, char* message)
{
	if (!connected)
		return 1;
	send("NOTICE %s :%s\r\n", target, message);
	return 0;
}

int IRC::notice(char* fmt, ...)
{
	va_list argp;

	if (!connected)
		return 1;
	va_start(argp, fmt);
	char szBuf[1024];
	send("NOTICE %s :", fmt);
#ifdef OS_WIN32
	int count = vsprintf_s(szBuf, va_arg(argp,char*), argp);
#else
	int count = vsprintf(szBuf, va_arg(argp,char*), argp);
#endif
	if (count>0) send(szBuf);
	va_end(argp);
	send("\r\n");
	return 0;
}

int IRC::privmsg(char* target, char* message)
{
	if (!connected)
		return 1;
	send("PRIVMSG %s :%s\r\n", target, message);
	return 0;
}

int IRC::privmsg(char* fmt, ...)
{
	va_list argp;

	if (!connected)
		return 1;
	va_start(argp, fmt);
	send("PRIVMSG %s :", fmt);
	char szBuf[1024];
#ifdef OS_WIN32
	int count = vsprintf_s(szBuf, va_arg(argp,char*), argp);
#else
	int count = vsprintf(szBuf, va_arg(argp,char*), argp);
#endif
	if (count>0) send(szBuf);
	va_end(argp);
	send("\r\n");
	return 0;
}


int IRC::join(char* channel)
{
	if (!connected)
		return 1;
	send("JOIN %s\r\n", channel);
	return 0;
}

int IRC::part(char* channel)
{
	if (!connected)
		return 1;
	send("PART %s\r\n", channel);
	return 0;
}

int IRC::kick(char* channel, char* nick)
{
	if (!connected)
		return 1;
	send("KICK %s %s\r\n", channel, nick);
	return 0;
}

int IRC::raw(char* data)
{
	if (!connected)
		return 1;
	send("%s\r\n", data);
	return 0;
}

int IRC::kick(char* channel, char* nick, char* message)
{
	if (!connected)
		return 1;
	send("KICK %s %s :%s\r\n", channel, nick, message);
	return 0;
}

int IRC::mode(char* channel, char* modes, char* targets)
{
	if (!connected)
		return 1;
	if (!targets)
		send("MODE %s %s\r\n", channel, modes);
	else
		send("MODE %s %s %s\r\n", channel, modes, targets);
	return 0;
}

int IRC::mode(char* modes)
{
	if (!connected)
		return 1;
	mode(cur_nick, modes, 0);
	return 0;
}

int IRC::nick(char* newnick)
{
	if (!connected)
		return 1;
	send("NICK %s\r\n", newnick);
	return 0;
}

char* IRC::current_nick()
{
	return cur_nick;
}
