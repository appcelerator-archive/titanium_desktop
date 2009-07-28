/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */
#include <kroll/kroll.h>
#include <sstream>

#include "network_binding.h"
#include "tcp_socket_binding.h"
#include "ipaddress_binding.h"
#include "host_binding.h"
#include "irc/irc_client_binding.h"
#include "http/http_client_binding.h"
#include "http/http_server_binding.h"
#include "proxy/proxy.h"
using kroll::URLUtils;

namespace ti
{
	std::vector<SharedKObject> NetworkBinding::bindings;
	NetworkBinding::NetworkBinding(Host* host, std::string modulePath) :
		host(host),modulePath(modulePath),
		global(host->GetGlobalObject()),
		proxy(NULL),
		next_listener_id(0)
	{
		SharedValue online = Value::NewBool(true);
		/**
		 * @tiapi(property=True,name=Network.online,since=0.2) Whether or not the system is connected to the internet
		 * @tiresult(for=Network.online,type=Boolean) true if the system is connected to the internet, false if otherwise
		 */
		this->Set("online", online);

		// methods that are available on Titanium.Network
		/**
		 * @tiapi(method=True,name=Network.createTCPSocket,since=0.2) Creates a TCPSocket object
		 * @tiarg(for=Network.createTCPSocket,name=host,type=String) the hostname to connect to
		 * @tiarg(for=Network.createTCPSocket,name=port,type=Number) the port to use
		 * @tiresult(for=Network.createTCPSocket,type=Network.TCPSocket) a TCPSocket object
		 */
		this->SetMethod("createTCPSocket",&NetworkBinding::CreateTCPSocket);
		/**
		 * @tiapi(method=True,name=Network.createIRCClient,since=0.2) Creates an IRCClient object
		 * @tiresult(for=Network.createIRCClient,type=Network.IRCClient) an IRCClient object
		 */
		this->SetMethod("createIRCClient",&NetworkBinding::CreateIRCClient);
		/**
		 * @tiapi(method=True,name=Network.createIPAddress,since=0.2) Creates an IPAddress object
		 * @tiarg(for=Network.createIPAddress,name=address,type=String) the IP address
		 * @tiresult(for=Network.createIPAddress,type=Network.IPAddress) an IPAddress object
		 */
		this->SetMethod("createIPAddress",&NetworkBinding::CreateIPAddress);
		/**
		 * @tiapi(method=True,name=Network.createHTTPClient,since=0.3) Creates an HTTPClient object
		 * @tiresult(for=Network.createHTTPClient,type=Network.HTTPClient) an HTTPClient object
		 */
		this->SetMethod("createHTTPClient",&NetworkBinding::CreateHTTPClient);
		/**
		 * @tiapi(method=True,name=Network.createHTTPServer,since=0.4) Creates an HTTPServer object
		 * @tiresult(for=Network.createHTTPServer,type=Network.HTTPServer) a HTTPServer object
		 */
		this->SetMethod("createHTTPServer",&NetworkBinding::CreateHTTPServer);
		/**
		 * @tiapi(method=True,name=Network.getHostByName,since=0.2) Returns a Host object using a hostname
		 * @tiarg(for=Network.getHostByName,name=name,type=String) the hostname
		 * @tiresult(for=Network.getHostByName,type=Network.Host) a Host object referencing the hostname
		 */
		this->SetMethod("getHostByName",&NetworkBinding::GetHostByName);
		/**
		 * @tiapi(method=True,name=Network.getHostByAddress,since=0.2) Returns a Host object using an address
		 * @tiarg(for=Network.getHostByAddress,name=address,type=String) the address
		 * @tiresult(for=Network.getHostByAddress,type=Network.Host) a Host object referencing the address
		 */
		this->SetMethod("getHostByAddress",&NetworkBinding::GetHostByAddress);
		/**
		 * @tiapi(method=True,name=Network.encodeURIComponent,since=0.3) Encodes a URI Component
		 * @tiarg(for=Network.encodeURIComponent,name=value,type=String) value to encode
		 * @tiresult(for=Network.encodeURIComponent,type=String) the encoded value
		 */
		this->SetMethod("encodeURIComponent",&NetworkBinding::EncodeURIComponent);
		/**
		 * @tiapi(method=True,name=Network.decodeURIComponent,since=0.3) Decodes a URI component
		 * @tiarg(for=Network.decodeURIComponent,name=value,type=String) value to decode
		 * @tiresult(for=Network.decodeURIComponent,type=String) the decoded value
		 */
		this->SetMethod("decodeURIComponent",&NetworkBinding::DecodeURIComponent);

		/**
		 * @tiapi(method=True,name=Network.addConnectivityListener,since=0.2) Adds a connectivity change listener that fires when the system connects or disconnects from the internet
		 * @tiarg(for=Network.addConnectivityListener,type=method,name=listener) a callback method to be fired when the system connects or disconnects from the internet
		 * @tiresult(for=Network.addConnectivityListener,type=Number) a callback id for the event
		 */
		this->SetMethod("addConnectivityListener",&NetworkBinding::AddConnectivityListener);
		/**
		 * @tiapi(method=True,name=Network.removeConnectivityListener,since=0.2) Removes a connectivity change listener
		 * @tiarg(for=Network.removeConnectivityListener,type=Number,name=id) the callback id of the method
		 */
		this->SetMethod("removeConnectivityListener",&NetworkBinding::RemoveConnectivityListener);

		/**
		 * @tiapi(method=True,name=Network.setProxy,since=0.2) Sets the proxy parameters of the system
		 * @tiarg(for=Network.setProxy,name=hostname,type=Number) the proxy hostname
		 * @tiarg(for=Network.setProxy,name=port,type=String) the proxy port
		 * @tiarg(for=Network.setProxy,name=username,type=String) the proxy username
		 * @tiarg(for=Network.setProxy,name=password,type=String) the proxy password
		 * @tiresult(for=Network.setProxy,type=Boolean) returns true if the new values are accepted, false on error.
		 */
		this->SetMethod("setProxy",&NetworkBinding::SetProxy);

		/**
		 * @tiapi(method=True,name=Network.getProxy,since=0.2) Returns the proxy parameters of the system
		 * @tiresult(for=Network.getProxy,type=Network.Proxy,name=id) a Proxy object
		 */
		this->SetMethod("getProxy",&NetworkBinding::GetProxy);

		// NOTE: this is only used internally and shouldn't be published
		this->SetMethod("FireOnlineStatusChange",&NetworkBinding::FireOnlineStatusChange);

#if defined(OS_LINUX)
		this->net_status = new DBusNetworkStatus(this);
		this->net_status->Start();
#elif defined(OS_OSX)
		SharedKMethod delegate = this->Get("FireOnlineStatusChange")->ToMethod();
		networkDelegate = [[NetworkReachability alloc] initWithDelegate:delegate];
#endif
	}

	NetworkBinding::~NetworkBinding()
	{
#if defined(OS_OSX)
		[networkDelegate release];
		networkDelegate=nil;
#else
		delete this->net_status;
#endif
	}
	void NetworkBinding::Shutdown()
	{
		PRINTD("NetworkBinding::Shutdown start");
		listeners.clear();
		PRINTD("NetworkBinding::Shutdown finish");
	}
	void NetworkBinding::_GetByHost(std::string hostname, SharedValue result)
	{
		AutoPtr<HostBinding> binding = new HostBinding(hostname);
		if (binding->IsInvalid())
		{
			throw ValueException::FromString("Could not resolve address");
		}
		result->SetObject(binding);
	}
	void NetworkBinding::GetHostByAddress(const ValueList& args, SharedValue result)
	{
		if (args.at(0)->IsObject())
		{
			SharedKObject obj = args.at(0)->ToObject();
			AutoPtr<IPAddressBinding> b = obj.cast<IPAddressBinding>();
			if (!b.isNull())
			{
				// in this case, they've passed us an IPAddressBinding
				// object, which we can just retrieve the ipaddress
				// instance and resolving using it
				IPAddress addr(b->GetAddress()->toString());
				AutoPtr<HostBinding> binding = new HostBinding(addr);
				if (binding->IsInvalid())
				{
					throw ValueException::FromString("Could not resolve address");
				}
				result->SetObject(binding);
				return;
			}
			else
			{
				SharedValue bo = obj->Get("toString");
				if (bo->IsMethod())
				{
					SharedKMethod m = bo->ToMethod();
					ValueList args;
					SharedValue tostr = m->Call(args);
					this->_GetByHost(tostr->ToString(),result);
					return;
				}
				throw ValueException::FromString("Unknown object passed");
			}
		}
		else if (args.at(0)->IsString())
		{
			// in this case, they just passed in a string so resolve as
			// normal
			this->_GetByHost(args.at(0)->ToString(),result);
		}
	}
	void NetworkBinding::GetHostByName(const ValueList& args, SharedValue result)
	{
		this->_GetByHost(args.at(0)->ToString(),result);
	}
	void NetworkBinding::CreateIPAddress(const ValueList& args, SharedValue result)
	{
		AutoPtr<IPAddressBinding> binding = new IPAddressBinding(args.at(0)->ToString());
		if (binding->IsInvalid())
		{
			throw ValueException::FromString("Invalid address");
		}
		result->SetObject(binding);
	}
	void NetworkBinding::CreateTCPSocket(const ValueList& args, SharedValue result)
	{
		//TODO: check for args
		AutoPtr<TCPSocketBinding> tcp = new TCPSocketBinding(host, args.at(0)->ToString(), args.at(1)->ToInt());
		result->SetObject(tcp);
	}
	void NetworkBinding::CreateIRCClient(const ValueList& args, SharedValue result)
	{
		AutoPtr<IRCClientBinding> irc = new IRCClientBinding(host);
		result->SetObject(irc);
	}
	void NetworkBinding::RemoveBinding(void* binding)
	{
		std::vector<SharedKObject>::iterator i = bindings.begin();
		while(i!=bindings.end())
		{
			SharedKObject b = (*i);
			if (binding == b.get())
			{
				bindings.erase(i);
				break;
			}
			i++;
		}
	}
	void NetworkBinding::CreateHTTPClient(const ValueList& args, SharedValue result)
	{
		// we hold the reference to this until we're done with it
		// which happense when the binding impl calls remove
		SharedKObject http = new HTTPClientBinding(host,modulePath);
		this->bindings.push_back(http);
		result->SetObject(http);
	}
	void NetworkBinding::CreateHTTPServer(const ValueList& args, SharedValue result)
	{
		// we hold the reference to this until we're done with it
		// which happense when the binding impl calls remove
		SharedKObject http = new HTTPServerBinding(host);
		this->bindings.push_back(http);
		result->SetObject(http);
	}
	void NetworkBinding::AddConnectivityListener(const ValueList& args, SharedValue result)
	{
		args.VerifyException("addConnectivityListener", "m");
		SharedKMethod target = args.at(0)->ToMethod();

		Listener listener = Listener();
		listener.id = this->next_listener_id++;
		listener.callback = target;
		this->listeners.push_back(listener);
		result->SetInt(listener.id);
	}

	void NetworkBinding::RemoveConnectivityListener(
		const ValueList& args,
		SharedValue result)
	{
		args.VerifyException("removeConnectivityListener", "n");
		int id = args.at(0)->ToInt();

		std::vector<Listener>::iterator it = this->listeners.begin();
		while (it != this->listeners.end())
		{
			if ((*it).id == id)
			{
				this->listeners.erase(it);
				result->SetBool(true);
				return;
			}
			it++;
		}
		result->SetBool(false);
	}

	bool NetworkBinding::HasNetworkStatusListeners()
	{
		return this->listeners.size() > 0;
	}

	void NetworkBinding::NetworkStatusChange(bool online)
	{
		PRINTD("ti.Network: Online status changed ==> " << online);
		this->Set("online", Value::NewBool(online));

		ValueList args = ValueList();
		args.push_back(Value::NewBool(online));
		std::vector<Listener>::iterator it = this->listeners.begin();
		while (it != this->listeners.end())
		{
			SharedKMethod callback = (*it++).callback;
			try
			{
				host->InvokeMethodOnMainThread(callback, args, false);
			}
			catch(ValueException& e)
			{
				SharedString ss = e.GetValue()->DisplayString();
				std::cerr << "ti.Network.NetworkStatusChange callback failed: "
				          << *ss << std::endl;
			}
		}
	}

	void NetworkBinding::FireOnlineStatusChange(const ValueList& args, SharedValue result)
	{
		if (args.at(0)->IsBool())
		{
			this->NetworkStatusChange(args.at(0)->ToBool());
		}
	}

	void NetworkBinding::EncodeURIComponent(const ValueList &args, SharedValue result)
	{
		if (args.at(0)->IsNull() || args.at(0)->IsUndefined())
		{
			result->SetString("");
		}
		else if (args.at(0)->IsString())
		{
			std::string src = args.at(0)->ToString();
			std::string sResult = URLUtils::EncodeURIComponent(src);
			result->SetString(sResult);
		}
		else if (args.at(0)->IsDouble())
		{
			std::stringstream str;
			str << args.at(0)->ToDouble();
			result->SetString(str.str().c_str());
		}
		else if (args.at(0)->IsBool())
		{
			std::stringstream str;
			str << args.at(0)->ToBool();
			result->SetString(str.str().c_str());
		}
		else if (args.at(0)->IsInt())
		{
			std::stringstream str;
			str << args.at(0)->ToInt();
			result->SetString(str.str().c_str());
		}
		else
		{
			throw ValueException::FromString("Could not encodeURIComponent with type passed");
		}
	}

	void NetworkBinding::DecodeURIComponent(const ValueList &args, SharedValue result)
	{
		if (args.at(0)->IsNull() || args.at(0)->IsUndefined())
		{
			result->SetString("");
		}
		else if (args.at(0)->IsString())
		{
			std::string src = args.at(0)->ToString();
			std::string sResult = URLUtils::DecodeURIComponent(src);
			result->SetString(sResult);
		}
		else
		{
			throw ValueException::FromString("Could not decodeURIComponent with type passed");
		}
	}
	
	void NetworkBinding::SetProxy(const ValueList& args, SharedValue result)
	{
		std::string hostname = args.at(0)->ToString();
		std::string port = args.at(1)->ToString();
		std::string username = args.at(2)->ToString();
		std::string password = args.at(3)->ToString();

#if defined(OS_WIN32)
		std::string http_proxy = "http://";
		http_proxy += username + ":" + password + "@";
		http_proxy += hostname + ":" + port;
		int i = ::_putenv_s("HTTP_PROXY", http_proxy.c_str());
		if(i != 0)
		{
			result->SetBool(false);
		}
#endif
		
		// this handles the updating of the reference count for the proxy shared ptr.
		proxy = new ti::Proxy(hostname, port, username,password);
		result->SetBool(true);
  	}

	void NetworkBinding::GetProxy(const ValueList& args, SharedValue result)
	{
		// setObject will return null if the proxy hasn't been configured, so we
		// don't need a null check here.
		result->SetObject(this->proxy);
	}

	Host* NetworkBinding::GetHost()
	{
		return this->host;
	}
}
