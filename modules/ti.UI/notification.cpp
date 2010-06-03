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
	SetMethod("setTitle", &Notification::_SetTitle);
	SetMethod("setMessage", &Notification::_SetMessage);
	SetMethod("setIcon", &Notification::_SetIcon);
	SetMethod("setTimeout", &Notification::_SetTimeout);
	SetMethod("setDelay", &Notification::_SetTimeout);
	SetMethod("setCallback", &Notification::_SetCallback);
	SetMethod("show", &Notification::_Show);
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
