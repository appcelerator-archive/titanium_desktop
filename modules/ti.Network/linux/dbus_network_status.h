/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _DBUS_NETWORK_STATUS_H_
#define _DBUS_NETWORK_STATUS_H_

#include <dbus/dbus.h>
#include <dbus/dbus-glib.h>
#include <gdk/gdk.h>
namespace ti
{
	class DBusNetworkStatus : public NetworkStatus
	{
		public:
		DBusNetworkStatus(NetworkBinding* binding);

		void Start();
		void Shutdown(bool async=false);
		void Job();

		private:
		DBusGConnection* bus;
		DBusGProxy* wicd_wired_proxy;
		DBusGProxy* wicd_wireless_proxy;
		DBusGProxy* nm_proxy;

		virtual void InitializeLoop();
		virtual bool GetStatus();
		virtual void CleanupLoop();
		void _DBusStatus(const ValueList& args, KValueRef result);
	};
}

#endif
