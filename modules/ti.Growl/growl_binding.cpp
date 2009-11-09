/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#include <kroll/kroll.h>
#include "growl_binding.h"

namespace ti
{
	GrowlBinding::GrowlBinding(KObjectRef global) :
		StaticBoundObject("Growl"),
		global(global)
	{
		// These methods aren't public (not documented)
		SetMethod("showNotification", &GrowlBinding::ShowNotification);
		SetMethod("isRunning", &GrowlBinding::IsRunning);
	}

	void GrowlBinding::ShowNotification(const ValueList& args, KValueRef result)
	{
		std::string title, description, iconURL = "";
		int notification_timeout = 3;
		KMethodRef callback;

		if (args.size() >= 2) {
			title = args.at(0)->ToString();
			description = args.at(1)->ToString();

			if (args.size() >= 3 && args.at(2)->IsString()) {
				iconURL = args.at(2)->ToString();
			}
			if (args.size() >= 4 && args.at(3)->IsNumber()) {
				notification_timeout = args.at(3)->ToInt();
			}

			KMethodRef callback;
			if (args.size() >= 5 && args.at(4)->IsMethod()) {
				callback = args.at(4)->ToMethod();
			}

			ShowNotification(title, description, iconURL, notification_timeout, callback);
		}
		else if (args.size() == 1  && args.at(0)->IsObject()) {
			KObjectRef options = args.at(0)->ToObject();

			KValueRef value = options->Get("title");
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

	void GrowlBinding::IsRunning(const ValueList& args, KValueRef result)
	{
		result->SetBool(IsRunning());
	}

	GrowlBinding::~GrowlBinding()
	{
	}
}
