/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2010 Appcelerator, Inc. All Rights Reserved.
 */

#include "notification.h"

namespace ti
{

Notification::Notification() : KAccessorObject("Notification"),
	timeout(-1)
{
	/**
	 * @tiapi(method=True,name=Notification.Notification.setTitle,since=0.9)
	 * @tiapi Set the notification title
	 * @tiarg[String, title] notification title text
	 */
	SetMethod("setTitle", &Notification::_SetTitle);

	/**
	 * @tiapi(method=True,name=Notification.Notification.SetMessage,since=0.9)
	 * @tiapi Set the notification message
	 * @tiarg[String, message] notification message text
	 */
	SetMethod("setMessage", &Notification::_SetMessage);

	/**
	 * @tiapi(method=True,name=Notification.Notification.setIcon,since=0.9)
	 * @tiapi Set the notification icon image
	 * @tiarg[String, icon] path to the icon image
	 */
	SetMethod("setIcon", &Notification::_SetIcon);

	/**
	 * @tiapi(method=True,name=Notification.Notification.setTimeout,platforms=win32|linux,since=0.9)
	 * @tiapi Set the timeout before the notification expires.
	 * @tiarg[Integer, timeout] Timeout in seconds. (-1 = use system default, 0 = never expire)
	 */
	SetMethod("setTimeout", &Notification::_SetTimeout);
	SetMethod("setDelay", &Notification::_SetTimeout);

	/**
	 * @tiapi(method=True,name=Notification.Notification.setCallback,since=0.9)
	 * @tiapi Set a callback function fired when notification is clicked. NOTE: not all platforms support this feature
	 * @tiarg[Function,callback] function to execute when notification is clicked
	 */
	SetMethod("setCallback", &Notification::_SetCallback);

	/**
	 * @tiapi(method=True,name=Notification.Notification.show,since=0.9)
	 * @tiapi Display the notification
	 * @tiresult[Boolean] True if successfully displayed notification
	 */
	SetMethod("show", &Notification::_Show);

	/**
	 * @tiapi(method=True,name=Notification.Notification.hide,since=0.9)
	 * @tiapi Hide the notification
	 * @tiresult[Boolean] True if successfully hidden the notification
	 */
	SetMethod("hide", &Notification::_Hide);

	SetBool("nativeNotifications", Notification::InitializeImpl());
	this->CreateImpl();
}

Notification::~Notification()
{
	this->DestroyImpl();
}

void Notification::_SetTitle(const ValueList& args, KValueRef result)
{
	args.VerifyException("setTitle", "s");
	this->title = args.GetString(0);
}

void Notification::_SetMessage(const ValueList& args, KValueRef result)
{
	args.VerifyException("setMessage", "s");
	this->message = args.GetString(0);
}

void Notification::_SetIcon(const ValueList& args, KValueRef result)
{
	args.VerifyException("setIcon", "s");
	this->iconURL = args.GetString(0);
}

void Notification::_SetTimeout(const ValueList& args, KValueRef result)
{
	args.VerifyException("setTimeout", "i");
	this->timeout = args.GetInt(0);
}

void Notification::_SetCallback(const ValueList& args, KValueRef result)
{
	args.VerifyException("setCallback", "m");
	this->clickedCallback = args.GetMethod(0);
}

void Notification::_Show(const ValueList& args, KValueRef result)
{
	result->SetBool(this->ShowImpl());
}

void Notification::_Hide(const ValueList& args, KValueRef result)
{
	result->SetBool(this->HideImpl());
}

void Notification::Configure(KObjectRef properties)
{
	this->title = properties->GetString("title");
	this->message = properties->GetString("message");
	this->iconURL = properties->GetString("icon");
	this->timeout = properties->GetInt("timeout", -1);
	this->clickedCallback = properties->GetMethod("callback");
}

}
