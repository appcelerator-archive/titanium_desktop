/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#include <kroll/kroll.h>
#include "growl_binding.h"

namespace ti
{
	GrowlBinding::GrowlBinding(SharedKObject global) : global(global)
	{
		/**
		 * @tiapi(method=True,name=Notification.showNotification,since=0.2) show a growl notification
         * @tiarg(for=Notification.showNotification,name=title,type=string) title of the notification window (the first argument can also be an object with each argument as a property)
         * @tiarg(for=Notification.showNotification,name=description,type=string) title of the notification window
         * @tiarg(for=Notification.showNotification,name=icon,type=string,optional=False) icon path for the notification window
         * @tiarg(for=Notification.showNotification,name=timeout,type=string,optional=False) timeout to hide the window
		 */
		SetMethod("showNotification", &GrowlBinding::ShowNotification);
		/**
		 * @tiapi(method=True,name=Notification.isRunning,since=0.2) returns true if Growl is running
		 * @tiresult(for=Notification.isRunning,type=boolean) returns true if running
		 */
		SetMethod("isRunning", &GrowlBinding::IsRunning);
	}

	void GrowlBinding::ShowNotification(const ValueList& args, SharedValue result)
	{
		std::string title, description, iconURL = "";
		int notification_timeout = 3;
		SharedKMethod callback;

		if (args.size() >= 2) {
			title = args.at(0)->ToString();
			description = args.at(1)->ToString();

			if (args.size() >= 3 && args.at(2)->IsString()) {
				iconURL = args.at(2)->ToString();
			}
			if (args.size() >= 4 && args.at(3)->IsNumber()) {
				notification_timeout = args.at(3)->ToInt();
			}

			SharedKMethod callback;
			if (args.size() >= 5 && args.at(4)->IsMethod()) {
				callback = args.at(4)->ToMethod();
			}

			ShowNotification(title, description, iconURL, notification_timeout, callback);
		}
		else if (args.size() == 1  && args.at(0)->IsObject()) {
			SharedKObject options = args.at(0)->ToObject();

			SharedValue value = options->Get("title");
			if (value->IsUndefined()) {
				throw ValueException::FromString("Notification title was not set");
			}
			title = value->ToString();

			value = options->Get("description");
			if (value->IsUndefined()) {
				throw ValueException::FromString("Notification description was not set");
			}
			description = value->ToString();

			value = options->Get("iconURL");
			if (!value->IsUndefined()) {
				iconURL = value->ToString();
			}

			value = options->Get("timeout");
			if (!value->IsUndefined()) {
				notification_timeout = value->ToInt();
			}

			value = options->Get("callback");
			if (!value->IsUndefined()) {
				callback = value->ToMethod();
			}

			ShowNotification(title, description, iconURL, notification_timeout, callback);
		}
	}

	void GrowlBinding::IsRunning(const ValueList& args, SharedValue result)
	{
		result->SetBool(IsRunning());
	}

	GrowlBinding::~GrowlBinding()
	{
	}
}
