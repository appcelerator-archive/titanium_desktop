/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */

#include "../growl_binding.h"
#include "libnotify_binding.h"
#include <libnotify/notify.h>
#include <cstring>

namespace ti {
	LibNotifyBinding::LibNotifyBinding(SharedKObject global) : GrowlBinding(global)
	{
		notify_init(LibNotifyBinding::GetAppName().c_str());
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
		SharedKMethod callback)
	{

		const char *icon_path = NULL;
		SharedString icon_path_str;
		if (iconURL != "")
		{
			icon_path_str = LibNotifyBinding::GetResourcePath(iconURL.c_str());
			icon_path = (*icon_path_str).c_str();
		}

		NotifyNotification* n = notify_notification_new(
			title.c_str(), 
			description.c_str(),
			icon_path,
			NULL);

		notify_notification_set_timeout(n, notification_timeout * 1000);
		notify_notification_show(n, NULL);
		g_object_unref(G_OBJECT(n));
	}


	SharedString LibNotifyBinding::GetResourcePath(const char *URL)
	{
		if (URL == NULL || !strcmp(URL, ""))
			return SharedString(NULL);

		SharedValue meth_val = global->GetNS("App.appURLToPath");
		if (!meth_val->IsMethod())
			return SharedString(NULL);

		SharedKMethod meth = meth_val->ToMethod();
		ValueList args;
		args.push_back(Value::NewString(URL));
		SharedValue out_val = meth->Call(args);

		if (out_val->IsString())
		{
			return SharedString(new std::string(out_val->ToString()));
		}
		else
		{
			return SharedString(NULL);
		}
	}

	std::string LibNotifyBinding::GetAppName()
	{
		SharedValue meth_val = global->GetNS("App.getName");
		if (!meth_val->IsMethod())
			return "";

		SharedKMethod meth = meth_val->ToMethod();
		SharedValue out_val = meth->Call(ValueList());
		if (out_val->IsString())
		{
			return std::string(out_val->ToString());
		}
		else
		{
			return "";
		}
	}

}
