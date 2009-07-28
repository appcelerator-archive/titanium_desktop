/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */

#include "snarl_win32.h"
#include "SnarlInterface.h"

using namespace ti;
using namespace kroll;

namespace ti {

	SnarlWin32::SnarlWin32(SharedKObject global) : GrowlBinding(global) {

	}

	SnarlWin32::~SnarlWin32() {
		// TODO Auto-generated destructor stub
	}

	bool SnarlWin32::IsRunning()
	{
		SnarlInterface snarlInterface;

		int major, minor;
		return snarlInterface.snGetVersion(&major, &minor);
	}

	void SnarlWin32::ShowNotification(std::string& title, std::string& description,
		std::string& iconURL, int notification_delay, SharedKMethod callback)
	{
		SnarlInterface::SNARLSTRUCT snarlStruct;
		snarlStruct.cmd = SnarlInterface::SNARL_SHOW;

		std::wstring wtitle(title.begin(), title.end());
		std::wstring wdesc(description.begin(), description.end());

		int len;
		char* buf = SnarlInterface::convertToMultiByte(wtitle, &len);
		strncpy(snarlStruct.title, buf, SnarlInterface::SNARL_STRING_LENGTH - 1);
		snarlStruct.title[len] = 0;
		delete[] buf;

		buf = SnarlInterface::convertToMultiByte(wdesc, &len);
		strncpy(snarlStruct.text, buf, SnarlInterface::SNARL_STRING_LENGTH - 1);
		snarlStruct.text[len] = 0;
		delete[] buf;

		if (!iconURL.empty())
		{
			std::string iconPath = URLUtils::URLToPath(iconURL);
			std::wstring wicon(iconPath.begin(), iconPath.end());
			buf = SnarlInterface::convertToMultiByte(wicon, &len);
			strncpy(snarlStruct.icon, buf, SnarlInterface::SNARL_STRING_LENGTH - 1);
			snarlStruct.icon[len] = 0;
			delete[] buf;
		}

		snarlStruct.timeout = notification_delay;

		if (!callback.isNull()) {
			static const UINT SnarlClickCallback = ::RegisterWindowMessage("SnarlGlobalMessage");

			// retrieve the window handle using the native void* interface
			HWND windowHandle = (HWND) Host::GetInstance()->GetGlobalObject()->GetNS("UI.mainWindow.windowHandle")->ToVoidPtr();
			snarlStruct.lngData2 = reinterpret_cast<long>(windowHandle);
			snarlStruct.id = SnarlClickCallback;

			// add a callback handler through the binding framework
			ValueList args;
			SharedValue messageValue = Value::NewDouble(SnarlClickCallback);
			SharedValue callbackValue = Value::NewMethod(callback);
			args.push_back(messageValue);
			args.push_back(callbackValue);
			Host::GetInstance()->GetGlobalObject()->CallNS("UI.mainWindow.addMessageHandler", args);
		}

		SnarlInterface::send(snarlStruct);
	}
}
