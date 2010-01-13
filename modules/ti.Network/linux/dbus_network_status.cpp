/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */
#include "../network_status.h"
#include <dbus/dbus.h>
#include <dbus/dbus-glib.h>
#include <gdk/gdk.h>

namespace ti
{

static DBusGConnection* bus = 0;
static DBusGProxy* wicdWiredProxy = 0;
static DBusGProxy* wicdWirelessProxy = 0;
static DBusGProxy* networkManagerProxy = 0;
static KMethodRef statusCheckWork(0);

static KValueRef StatusCheckWork(const ValueList& args)
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
		return Value::NewBool(true);
	}
	if (wicdWiredProxy)
		g_object_unref(wicdWiredProxy);

	wicdWirelessProxy = dbus_g_proxy_new_for_name(bus, "org.wicd.daemon",
		"/org/wicd/daemon", "org.wicd.daemon.wireless");
	error = 0;

	if (!dbus_g_proxy_call(wicdWirelessProxy, "GetWirelessIP", 
		&error, G_TYPE_INVALID, G_TYPE_STRING, &result, G_TYPE_INVALID))
	{
		// 16 == interface not up
		if (error->code == 16)
			foundNetworkManager = true;
		g_error_free(error);
	}
	else if (result)
	{
		return Value::NewBool(true);
	}

	if (!wicdWirelessProxy)
		g_object_unref(wicdWirelessProxy);

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
		return Value::NewBool(true);
	}
	else
	{
		foundNetworkManager = true;
	}
	if (networkManagerProxy)
		g_object_unref(networkManagerProxy);

	if (!foundNetworkManager)
		return Value::NewBool(true);
	else
		return Value::NewBool(false);
}


void NetworkStatus::InitializeLoop()
{
	statusCheckWork = new kroll::KFunctionPtrMethod(&StatusCheckWork);

	g_type_init();
	dbus_threads_init_default();

	GError* error = 0;
	bus = dbus_g_bus_get(DBUS_BUS_SYSTEM, &error);

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

bool NetworkStatus::GetStatus()
{
	if (!bus)
		return true;

	return RunOnMainThread(statusCheckWork, ValueList())->ToBool();
}
}
