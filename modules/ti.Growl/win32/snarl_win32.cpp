/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */

#include "snarl_win32.h"
#include "SnarlInterface.h"
#include <kroll/win32/host.h>

using namespace ti;
using namespace kroll;

namespace ti
{
	UINT SnarlWin32::snarlWindowMessage = ::RegisterWindowMessageA("TitaniumSnarlMessage");
	std::map<long, KMethodRef> SnarlWin32::snarlCallbacks;

	SnarlWin32::SnarlWin32(KObjectRef global) :
		GrowlBinding(global)
	{
	}

	SnarlWin32::~SnarlWin32()
	{
	}

	bool SnarlWin32::IsRunning()
	{
		SnarlInterface snarlInterface;
		int major, minor;
		return snarlInterface.snGetVersion(&major, &minor);
	}

	void SnarlWin32::ShowNotification(std::string& title, std::string& description,
		std::string& iconURL, int timeout, KMethodRef callback)
	{
		SnarlInterface snarlInterface;

		std::wstring wideTitle = ::UTF8ToWide(title);
		std::wstring wideText = ::UTF8ToWide(description);
		std::wstring wideIconPath(L"");
		if (!iconURL.empty())
		{
			std::string iconPath = URLUtils::URLToPath(iconURL);
			wideIconPath.append(::UTF8ToWide(iconPath));
		}

		HWND replyWindow = Win32Host::Win32Instance()->AddMessageHandler(
			&SnarlWin32::MessageHandler);
		long id = snarlInterface.snShowMessage(wideTitle, wideText, timeout,
			wideIconPath, replyWindow, SnarlWin32::snarlWindowMessage);

		if (!callback.isNull())
		{
			SnarlWin32::snarlCallbacks[id] = callback;
		}
	}

	/*static*/
	bool SnarlWin32::MessageHandler(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		if (message != SnarlWin32::snarlWindowMessage)
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
}
