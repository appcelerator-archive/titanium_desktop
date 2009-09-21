/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _NETWORK_BINDING_H_
#define _NETWORK_BINDING_H_

#include <kroll/kroll.h>


namespace ti
{
	class NetworkBinding;
	class NetworkStatus;
}

#if defined(OS_OSX)
#include "osx/network_status.h"
#elif defined(OS_LINUX)
#include "network_status.h"
#include "linux/dbus_network_status.h"
#elif defined(OS_WIN32)
#include "network_status.h"
#include "win32/icmp_network_status.h"
#endif

namespace ti
{
	class NetworkBinding : public StaticBoundObject
	{
	public:
		NetworkBinding(Host*,std::string);
		virtual ~NetworkBinding();

		bool HasNetworkStatusListeners();
		void NetworkStatusChange(bool online);
		Host* GetHost();
		void Shutdown();

	private:
		Host* host;
		std::string modulePath;
		SharedKObject global;

		struct Listener
		{
			SharedKMethod callback;
			long id;
		};
		std::vector<Listener> listeners;
		long next_listener_id;

#if defined(OS_OSX)
		NetworkReachability *networkDelegate;
#else 
		NetworkStatus *net_status;
#endif

		void CreateIPAddress(const ValueList& args, SharedValue result);
		void CreateTCPSocket(const ValueList& args, SharedValue result);
		void CreateIRCClient(const ValueList& args, SharedValue result);
		void CreateHTTPClient(const ValueList& args, SharedValue result);
		void CreateHTTPServer(const ValueList& args, SharedValue result);
		void CreateHTTPCookie(const ValueList& args, SharedValue result);
		void EncodeURIComponent(const ValueList &args, SharedValue result);
		void DecodeURIComponent(const ValueList &args, SharedValue result);
		void _GetByHost(std::string host, SharedValue result);
		void GetHostByName(const ValueList& args, SharedValue result);
		void GetHostByAddress(const ValueList& args, SharedValue result);
		void AddConnectivityListener(const ValueList& args, SharedValue result);
		void RemoveConnectivityListener(const ValueList& args, SharedValue result);
		void FireOnlineStatusChange(const ValueList& args, SharedValue result);
		void SetHTTPProxy(const ValueList& args, SharedValue result);
		void SetHTTPSProxy(const ValueList& args, SharedValue result);
		void GetHTTPProxy(const ValueList& args, SharedValue result);
		void GetHTTPSProxy(const ValueList& args, SharedValue result);
	};
}

#endif
