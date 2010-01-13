/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */
#include <kroll/kroll.h>
#include <sstream>

#include "network_status.h"
#include "network_binding.h"
#include "tcp_socket_binding.h"
#include "ipaddress_binding.h"
#include "host_binding.h"
#include "irc/irc_client_binding.h"
#include "http/http_client_binding.h"
#include "http/http_server_binding.h"

#include <Poco/Mutex.h>

namespace ti
{
	NetworkBinding::NetworkBinding(Host* host) :
		StaticBoundObject("Network"),
		host(host),
		global(host->GetGlobalObject())
	{
		KValueRef online = Value::NewBool(true);

		/**
		 * @tiapi(property=True,name=Network.online,since=0.2)
		 * @tiapi Whether or not the system is connected to the internet
		 * @tiresult[Boolean] True if the system is connected to the internet, false if otherwise
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
		 * @tiapi(method=True,name=Network.createHTTPCookie,since=0.7) Creates a new HTTPCookie object
		 * @tiresult(for=Network.createHTTPCookie,type=Network.HTTPCookie) a HTTPCookie object
		 */
		this->SetMethod("createHTTPCookie",&NetworkBinding::CreateHTTPCookie);
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
		 * @tiapi(method=True,name=Network.addConnectivityListener,since=0.2)
		 * @tiapi Adds a connectivity change listener that fires when the system
		 * @tiapi connects or disconnects from the internet
		 * @tiarg(for=Network.addConnectivityListener,type=Function,name=listener) 
		 * @tiarg A callback method to be fired when the system connects or disconnects from the internet
		 * @tiresult(for=Network.addConnectivityListener,type=Number) a callback id for the event
		 */
		this->SetMethod("addConnectivityListener",&NetworkBinding::AddConnectivityListener);
		/**
		 * @tiapi(method=True,name=Network.removeConnectivityListener,since=0.2) Removes a connectivity change listener
		 * @tiarg(for=Network.removeConnectivityListener,type=Number,name=id) the callback id of the method
		 */
		this->SetMethod("removeConnectivityListener",&NetworkBinding::RemoveConnectivityListener);

		/**
		 * @tiapi(method=True,name=Network.setHTTPProxy,since=0.7)
		 * @tiapi Override application proxy autodetection with a proxy URL.
		 * @tiarg[String, hostname] The full proxy hostname.
		 */
		/**
		 * @tiapi(method=True,name=Network.setProxy,since=0.2, deprecated=True)
		 * @tiapi Override HTTP application proxy autodetection with a proxy URL.
		 * @tiarg[String, hostname] The full proxy hostname.
		 */
		this->SetMethod("setHTTPProxy", &NetworkBinding::SetHTTPProxy);
		this->SetMethod("setProxy", &NetworkBinding::SetHTTPProxy);

		/**
		 * @tiapi(method=True,name=Network.getHTTPProxy,since=0.7) 
		 * @tiapi Return the proxy override, if one is set.
		 * @tiresult[String|null] The full proxy override URL or null if none is set.
		 */
		/**
		 * @tiapi(method=True,name=Network.getHTTPProxy,since=0.2,deprecated=True) 
		 * @tiapi Return the HTTP proxy override, if one is set.
		 * @tiresult[String|null] The full proxy override URL or null if none is set.
		 */
		this->SetMethod("getHTTPProxy", &NetworkBinding::GetHTTPProxy);
		this->SetMethod("getProxy", &NetworkBinding::GetHTTPProxy);

		/**
		 * @tiapi(method=True,name=Network.setHTTPSProxy,since=0.7)
		 * @tiapi Override application proxy autodetection with a proxy URL.
		 * @tiarg[String, hostname] The full proxy hostname.
		 */
		this->SetMethod("setHTTPSProxy", &NetworkBinding::SetHTTPSProxy);

		/**
		 * @tiapi(method=True,name=Network.getHTTPProxy,since=0.7)
		 * @tiapi Return the proxy override, if one is set.
		 * @tiresult[String|null] The full proxy override URL or null if none is set.
		 */
		this->SetMethod("getHTTPSProxy", &NetworkBinding::GetHTTPSProxy);

		// NOTE: this is only used internally and shouldn't be published
		this->SetMethod("FireOnlineStatusChange",&NetworkBinding::FireOnlineStatusChange);

		this->netStatus = new NetworkStatus(this);
		this->netStatus->Start();
	}

	NetworkBinding::~NetworkBinding()
	{
		delete this->netStatus;
	}

	void NetworkBinding::Shutdown()
	{
		listeners.clear();
	}

	void NetworkBinding::_GetByHost(std::string hostname, KValueRef result)
	{
		AutoPtr<HostBinding> binding = new HostBinding(hostname);
		if (binding->IsInvalid())
		{
			throw ValueException::FromString("Could not resolve address");
		}
		result->SetObject(binding);
	}

	void NetworkBinding::GetHostByAddress(const ValueList& args, KValueRef result)
	{
		if (args.at(0)->IsObject())
		{
			KObjectRef obj = args.at(0)->ToObject();
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
				KValueRef bo = obj->Get("toString");
				if (bo->IsMethod())
				{
					KMethodRef m = bo->ToMethod();
					ValueList args;
					KValueRef tostr = m->Call(args);
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

	void NetworkBinding::GetHostByName(const ValueList& args, KValueRef result)
	{
		this->_GetByHost(args.at(0)->ToString(),result);
	}

	void NetworkBinding::CreateIPAddress(const ValueList& args, KValueRef result)
	{
		AutoPtr<IPAddressBinding> binding = new IPAddressBinding(args.at(0)->ToString());
		if (binding->IsInvalid())
		{
			throw ValueException::FromString("Invalid address");
		}
		result->SetObject(binding);
	}

	void NetworkBinding::CreateTCPSocket(const ValueList& args, KValueRef result)
	{
		args.VerifyException("createTCPSocket", "sn");
		result->SetObject(new TCPSocketBinding(host,
			args.GetString(0), args.GetInt(1)));
	}

	void NetworkBinding::CreateIRCClient(const ValueList& args, KValueRef result)
	{
		AutoPtr<IRCClientBinding> irc = new IRCClientBinding(host);
		result->SetObject(irc);
	}

	void NetworkBinding::CreateHTTPClient(const ValueList& args, KValueRef result)
	{
		result->SetObject(new HTTPClientBinding(host));
	}

	void NetworkBinding::CreateHTTPServer(const ValueList& args, KValueRef result)
	{
		result->SetObject(new HTTPServerBinding(host));
	}

	void NetworkBinding::CreateHTTPCookie(const ValueList& args, KValueRef result)
	{
		result->SetObject(new HTTPCookie());
	}

	void NetworkBinding::AddConnectivityListener(const ValueList& args, KValueRef result)
	{
		args.VerifyException("addConnectivityListener", "m");
		KMethodRef target = args.at(0)->ToMethod();

		static long nextListenerId = 0;
		Listener listener = Listener();
		listener.id = nextListenerId++;
		listener.callback = target;
		this->listeners.push_back(listener);
		result->SetInt(listener.id);
	}

	void NetworkBinding::RemoveConnectivityListener(
		const ValueList& args, KValueRef result)
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
		static Logger* log = Logger::Get("NetworkStatus");
		log->Debug("ti.Network: Online status changed ==> %i", online);
		this->Set("online", Value::NewBool(online));

		ValueList args = ValueList();
		args.push_back(Value::NewBool(online));
		std::vector<Listener>::iterator it = this->listeners.begin();
		while (it != this->listeners.end())
		{
			KMethodRef callback = (*it++).callback;
			try
			{
				RunOnMainThread(callback, args, false);
			}
			catch(ValueException& e)
			{
				SharedString ss = e.GetValue()->DisplayString();
				log->Error("Network.NetworkStatus callback failed: %s", ss->c_str());
			}
		}
	}

	void NetworkBinding::FireOnlineStatusChange(const ValueList& args, KValueRef result)
	{
		if (args.at(0)->IsBool())
		{
			this->NetworkStatusChange(args.at(0)->ToBool());
		}
	}

	void NetworkBinding::EncodeURIComponent(const ValueList &args, KValueRef result)
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

	void NetworkBinding::DecodeURIComponent(const ValueList &args, KValueRef result)
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

	static SharedProxy ArgumentsToProxy(const ValueList& args, const std::string& scheme)
	{
		if (args.at(0)->IsNull())
			return 0;

		std::string entry(args.GetString(0));
		if (entry.empty())
			return 0;

		// Do not pass the third argument entryScheme, because it overrides
		// any scheme set in the proxy string.
		return ProxyConfig::ParseProxyEntry(entry, scheme, std::string());
	}

	void NetworkBinding::SetHTTPProxy(const ValueList& args, KValueRef result)
	{
		args.VerifyException("setHTTPProxy", "s|0 ?s s s");
		SharedProxy proxy(ArgumentsToProxy(args, "http"));
		ProxyConfig::SetHTTPProxyOverride(proxy);
	}

	void NetworkBinding::GetHTTPProxy(const ValueList& args, KValueRef result)
	{
		SharedProxy proxy = ProxyConfig::GetHTTPProxyOverride();

		if (proxy.isNull())
			result->SetNull();
		else
			result->SetString(proxy->ToString().c_str());
	}

	void NetworkBinding::SetHTTPSProxy(const ValueList& args, KValueRef result)
	{
		args.VerifyException("setHTTPSProxy", "s|0 ?s s s");
		SharedProxy proxy(ArgumentsToProxy(args, "https"));
		ProxyConfig::SetHTTPSProxyOverride(proxy);
	}

	void NetworkBinding::GetHTTPSProxy(const ValueList& args, KValueRef result)
	{
		SharedProxy proxy = ProxyConfig::GetHTTPSProxyOverride();

		if (proxy.isNull())
			result->SetNull();
		else
			result->SetString(proxy->ToString().c_str());
	}

	Host* NetworkBinding::GetHost()
	{
		return this->host;
	}
}
