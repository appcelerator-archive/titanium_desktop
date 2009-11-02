/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */

#include "../growl_binding.h"
#include "libnotify_binding.h"
#include <libnotify/notify.h>
#include <cstring>

namespace ti
{

	LibNotifyBinding::LibNotifyBinding(KObjectRef global) :
		GrowlBinding(global)
	{
		std::string& appName = Host::GetInstance()->GetApplication()->name;
		notify_init(appName);
	}

	LibNotifyBinding::~LibNotifyBinding()
	{
	}

	bool LibNotifyBinding::IsRunning()
	{
		return notify_is_initted();
	}

	void LibNotifyBinding::ShowNotification(
		std::string& title,
		std::string& description,
		std::string& iconURL,
		int notification_timeout,
		KMethodRef callback)
	{
		std::string iconPath = "";
		if (!iconURL.empty())
		{
			iconPath = URLUtils::URLToPath(iconURL);
		}

		NotifyNotification* n = notify_notification_new(
			title.c_str(), 
			description.c_str(),
			iconPath.c_str(),
			NULL);

		notify_notification_set_timeout(n, notification_timeout * 1000);
		notify_notification_show(n, NULL);
		g_object_unref(G_OBJECT(n));
	}
}
