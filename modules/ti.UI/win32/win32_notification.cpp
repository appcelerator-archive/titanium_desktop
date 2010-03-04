/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2010 Appcelerator, Inc. All Rights Reserved.
 */

#include "../notification.h"
#include "SnarlInterface.h"

namespace ti
{

/*static*/
bool Notification::InitializeImpl()
{
	SnarlInterface snarlInterface;
	int major, minor;
	return snarlInterface.snGetVersion(&major, &minor);
}

/*static*/
void Notification::ShutdownImpl()
{
}

static UINT snarlWindowMessage = ::RegisterWindowMessageA("TitaniumSnarlMessage");
static std::map<long, KMethodRef> snarlCallbacks;
static bool MessageHandler(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message != snarlWindowMessage)
		return false;

	long id = (long) lParam;
	std::map<long, KMethodRef>::iterator i = snarlCallbacks.find(id);

	if (i != snarlCallbacks.end())
	{
		if (wParam == SnarlInterface::SNARL_NOTIFICATION_CLICKED)
		{
			RunOnMainThread(i->second, ValueList(), false);
		}
		else if (wParam == SnarlInterface::SNARL_NOTIFICATION_TIMED_OUT)
		{
			snarlCallbacks.erase(i);
		}
	}

	return true;
}

void Notification::CreateImpl()
{
	this->notification = -1;
}

void Notification::DestroyImpl()
{
}

bool Notification::ShowImpl()
{
	SnarlInterface snarlInterface;

	std::string iconPath;
	if (!iconURL.empty())
		iconPath = URLUtils::URLToPath(iconURL);

	HWND replyWindow = Host::GetInstance()->AddMessageHandler(
		&SnarlWin32::MessageHandler);
	long id = snarlInterface.snShowMessage(::UTF8ToWide(this->title),
		::UTF8ToWide(this->message), timeout, ::UTF8ToWide(iconPath),
		 replyWindow, snarlWindowMessage);

	if (!callback.isNull())
		SnarlWin32::snarlCallbacks[id] = callback;
}

bool Notification::HideImpl()
{
	if (this->notification == -1)
		return true;

	return snHideMessage(this->notification);
}
}
