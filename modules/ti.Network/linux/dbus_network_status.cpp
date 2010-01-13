/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */
#include "../network_status.h"

namespace ti
{

static DBusGConnection* bus = 0;
static DBusGProxy* wicdWiredProxy = 0;
static DBusGProxy* wicdwirelessProxy = 0;
static DBusGProxy* networkManagerProxy = 0;
static KMethod statusCheckWork(0);

void StatusCheckWork(const ValueList& args, KValueRef lresult)
{
	GError* error = 0;
	gchar* result = 0;
	bool foundNetworkManager = false;

	wicdWiredProxy = dbus_g_proxy_new_for_name(
		bus,
		"org.wicd.daemon",
		"/org/wicd/daemon",
		"org.wicd.daemon.wired");
	if (!dbus_g_proxy_call(
		wicdWiredProxy, "GetWiredIP", &error,
		G_TYPE_INVALID,
		G_TYPE_STRING, &result,
		G_TYPE_INVALID))
	{
		// 16 == interface not up
		if (error->code == 16)
			foundNetworkManager = true;
		g_error_free(error);
	}
	else if (result)
	{
		lresult->SetBool(true);
		return;
	}
	if (wicdWiredProxy)
		g_object_unref(wicdWiredProxy);

	wicdwirelessProxy = dbus_g_proxy_new_for_name(bus, "org.wicd.daemon",
		"/org/wicd/daemon", "org.wicd.daemon.wireless");
	error = 0;

	if (!dbus_g_proxy_call(wicdwirelessProxy, "GetWirelessIP", 
		&error, G_TYPE_INVALID, G_TYPE_STRING, &result, G_TYPE_INVALID))
	{
		// 16 == interface not up
		if (error->code == 16)
			foundNetworkManager = true;
		g_error_free(error);
	}
	else if (result)
	{
		lresult->SetBool(true);
		return;
	}

	if (!wicdwirelessProxy)
		g_object_unref(wicdwirelessProxy);

	networkManagerProxy = dbus_g_proxy_new_for_name(bus,
		"org.freedesktop.NetworkManager",
		"/org/freedesktop/NetworkManager",
		"org.freedesktop.DBus.Properties");
	error = 0;
	GValue nm_state_val = {0, };
	if (!dbus_g_proxy_call(networkManagerProxy, "Get", &error,
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
		foundNetworkManager = true;
	}
	if (networkManagerProxy)
		g_object_unref(networkManagerProxy);

	if (!foundNetworkManager)
		lresult->SetBool(true);
	else
		lresult->SetBool(false);
}


void NetworkStatus::InitializeLoop()
{
	statusCheckWork = new kroll::KFunctionPtrMethod(&StatusCheckWork);

	g_type_init();
	dbus_threads_init_default();

	GError* error = 0;
	>bus = dbus_g_bus_get(DBUS_BUS_SYSTEM, &error);

	if (!bus)
	{
		Logger::Get("NetworkStatus")->Error(
			"Could not connect to dbus: %s", error->message);
	}
}

void NetworkStatus::CleanupLoop()
{
	dbus_shutdown();

	if (wicdWirelessProxy)
		g_object_unref(wicdWirelessProxy);

	if (wicdWiredProxy)
		g_object_unref(wicdWiredProxy);
}

bool DBusNetworkStatus::GetStatus()
{
	if (!bus)
		return true;

	return RunOnMainThread(statusCheckWork, ValueList())->ToBool();
}
}
