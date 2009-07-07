/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#include "proxy.h"

namespace ti
{
	Proxy::Proxy(const std::string& _hostname,
		const std::string& _port,
		const std::string& _username,
		const std::string& _password)
		: hostname(_hostname),
		  port(_port),
		  username(_username),
		  password(_password)
	{

		/**
		 * @tiapi(method=True,name=Network.Proxy.getHostName,since=0.4) Returns the hostname of a Proxy object
		 * @tiresult(for=Network.Proxy.getHostName, type=String) the hostname of the Proxy object
		 */
		this->SetMethod("getHostName",&Proxy::getHostName);

		/**
		 * @tiapi(method=True,name=Network.Proxy.getPort,since=0.4) Returns the port of a Proxy object
		 * @tiresult(for=Network.Proxy.getPort, type=String) the port of the Proxy object
		 */
		this->SetMethod("getPort",&Proxy::getPort);
		
		/**
		 * @tiapi(method=True,name=Network.Proxy.getUserName,since=0.4) Returns the username of a Proxy object
		 * @tiresult(for=Network.Proxy.getUserName, type=String) the username of the Proxy object
		 */
		this->SetMethod("getUserName",&Proxy::getUserName);
		
		/**
		 * @tiapi(method=True,name=Network.Proxy.getPassword,since=0.4) Returns the password of a Proxy object
		 * @tiresult(for=Network.Proxy.getPassword, type=String) the password of the Proxy object
		 */
		this->SetMethod("getPassword",&Proxy::getPassword);
	}

	Proxy::~Proxy()
	{
	}
	
	void Proxy::getHostName(const ValueList& args, SharedValue result)
	{
		result->SetString(this->hostname.c_str());
	}
	
	void Proxy::getPort(const ValueList& args, SharedValue result)
	{
		result->SetString(this->port.c_str());
	}

	void Proxy::getUserName(const ValueList& args, SharedValue result)
	{
		result->SetString(this->username.c_str());
	}

	void Proxy::getPassword(const ValueList& args, SharedValue result)
	{
		result->SetString(this->password.c_str());
	}
}
