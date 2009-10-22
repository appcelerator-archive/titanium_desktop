/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */
#include "../network_binding.h"

namespace ti
{
	DBusNetworkStatus::DBusNetworkStatus(ti::NetworkBinding* binding) :
		NetworkStatus(binding),
		bus(NULL),
		wicd_wired_proxy(NULL),
		wicd_wireless_proxy(NULL),
		nm_proxy(NULL)
	{
		this->SetMethod("_DBusStatus", &DBusNetworkStatus::_DBusStatus);
	}

	void DBusNetworkStatus::InitializeLoop()
	{
		g_type_init();
		dbus_threads_init_default();

		GError *error = NULL;
		this->bus = dbus_g_bus_get(DBUS_BUS_SYSTEM, &error);

		if (this->bus == NULL)
		{
			std::cerr << "Could not connect to DBUS: "
			          << error->message << std::endl;
		}
	}

	void DBusNetworkStatus::CleanupLoop()
	{
		dbus_shutdown();

		if (this->wicd_wireless_proxy != NULL)
			g_object_unref(this->wicd_wireless_proxy);

		if (this->wicd_wired_proxy != NULL)
			g_object_unref(this->wicd_wired_proxy);
	}

	void DBusNetworkStatus::_DBusStatus(const ValueList& args, KValueRef lresult)
	{
		GError *error = NULL;
		gchar* result = NULL;
		bool found_nm = false;

		this->wicd_wired_proxy = dbus_g_proxy_new_for_name(
			this->bus,
			"org.wicd.daemon",
			"/org/wicd/daemon",
			"org.wicd.daemon.wired");
		if (!dbus_g_proxy_call(
			wicd_wired_proxy, "GetWiredIP", &error,
			G_TYPE_INVALID,
			G_TYPE_STRING, &result,
			G_TYPE_INVALID))
		{
			// 16 == interface not up
			if (error->code == 16)
				found_nm = true;
			g_error_free(error);
		}
		else if (result != NULL)
		{
			lresult->SetBool(true);
			return;
		}
		if (this->wicd_wired_proxy != NULL)
			g_object_unref(this->wicd_wired_proxy);

		this->wicd_wireless_proxy = dbus_g_proxy_new_for_name(
			this->bus,
			"org.wicd.daemon",
			"/org/wicd/daemon",
			"org.wicd.daemon.wireless");
		error = NULL;
		if (!dbus_g_proxy_call(wicd_wireless_proxy, "GetWirelessIP", &error, G_TYPE_INVALID,
			                   G_TYPE_STRING, &result, G_TYPE_INVALID))
		{
			// 16 == interface not up
			if (error->code == 16)
				found_nm = true;
			g_error_free(error);
		}
		else if (result != NULL)
		{
			lresult->SetBool(true);
			return;
		}
		if (this->wicd_wireless_proxy != NULL)
			g_object_unref(this->wicd_wireless_proxy);

		this->nm_proxy = dbus_g_proxy_new_for_name(
			this->bus,
			"org.freedesktop.NetworkManager",
			"/org/freedesktop/NetworkManager",
			"org.freedesktop.DBus.Properties");
		error = NULL;
		GValue nm_state_val = {0, };
		if (!dbus_g_proxy_call(
			this->nm_proxy, "Get", &error,
			G_TYPE_STRING, "org.freedesktop.NetworkManager",
			G_TYPE_STRING, "state",
			G_TYPE_INVALID,
			G_TYPE_VALUE, &nm_state_val,
			G_TYPE_INVALID))
		{
			g_error_free(error);
		}
		else if (g_value_get_uint(&nm_state_val) == 3)
		{
			lresult->SetBool(true);
			return;
		}
		else
		{
			found_nm = true;
		}
		if (this->nm_proxy != NULL)
			g_object_unref(this->nm_proxy);

		if (!found_nm)
			lresult->SetBool(true);
		else
			lresult->SetBool(false);

	}

	bool DBusNetworkStatus::GetStatus()
	{
		if (this->bus == NULL)
			return true;

		KMethodRef meth = this->Get("_DBusStatus")->ToMethod();
		KValueRef r = binding->GetHost()->InvokeMethodOnMainThread(meth, ValueList());
		return r->ToBool();
	}
}
