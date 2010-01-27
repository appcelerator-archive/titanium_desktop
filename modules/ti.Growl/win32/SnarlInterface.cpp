/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#include "SnarlInterface.h"

SnarlInterface::SnarlInterface()
{
	SNARL_GLOBAL_MESSAGE = "SnarlGlobalMessage";
}

SnarlInterface::~SnarlInterface()
{

}

long SnarlInterface::snShowMessage(std::wstring title, std::wstring text, 
	long timeout, std::wstring iconPath, HWND hWndReply, long uReplyMsg)
{
	SNARLSTRUCT snarlStruct;
	snarlStruct.cmd = SNARL_SHOW;

	int len;
	char* buf = convertToMultiByte(title, &len);
	strncpy(snarlStruct.title, buf, SNARL_STRING_LENGTH - 1);
	snarlStruct.title[len] = 0;
	delete[] buf;

	buf = convertToMultiByte(text, &len);
	strncpy(snarlStruct.text, buf, SNARL_STRING_LENGTH - 1);
	snarlStruct.text[len] = 0;
	delete[] buf;

	buf = convertToMultiByte(iconPath, &len);
	strncpy(snarlStruct.icon, buf, SNARL_STRING_LENGTH - 1);
	snarlStruct.icon[len] = 0;
	delete[] buf;

	snarlStruct.timeout = timeout;
	snarlStruct.lngData2 = reinterpret_cast<long>(hWndReply);
	snarlStruct.id = uReplyMsg;
	return send(snarlStruct);
}

long SnarlInterface::snShowMessageEx(std::wstring title, std::wstring text,
	long timeout, std::wstring iconPath, HWND hWndReply, long uReplyMsg,
	std::wstring soundPath, std::wstring msgClass)
{
	SNARLSTRUCTEX snarlStruct;
	snarlStruct.cmd = SNARL_EX_SHOW;

	int len;
	char* buf = convertToMultiByte(title, &len);
	strncpy(snarlStruct.title, buf, SNARL_STRING_LENGTH - 1);
	snarlStruct.title[len] = 0;
	delete[] buf;

	buf = convertToMultiByte(text, &len);
	strncpy(snarlStruct.text, buf, SNARL_STRING_LENGTH - 1);
	snarlStruct.text[len] = 0;
	delete[] buf;

	buf = convertToMultiByte(iconPath, &len);
	strncpy(snarlStruct.icon, buf, SNARL_STRING_LENGTH - 1);
	snarlStruct.icon[len] = 0;
	delete[] buf;

	buf = convertToMultiByte(msgClass, &len);
	strncpy(snarlStruct.snarlClass, buf, SNARL_STRING_LENGTH - 1);
	snarlStruct.snarlClass[len] = 0;
	delete[] buf;

	snarlStruct.timeout = timeout;
	snarlStruct.lngData2 = reinterpret_cast<long>(hWndReply);
	snarlStruct.id = uReplyMsg;
	return send(snarlStruct);
}

bool SnarlInterface::snHideMessage(long id)
{
	SNARLSTRUCT snarlStruct;
	snarlStruct.id = id;
	snarlStruct.cmd = SNARL_HIDE;
	return static_cast<bool>(send(snarlStruct));
}

bool SnarlInterface::snIsMessageVisible(long id)
{
	SNARLSTRUCT snarlStruct;
	snarlStruct.id = id;
	snarlStruct.cmd = SNARL_IS_VISIBLE;
	return static_cast<bool>(send(snarlStruct));
}

bool SnarlInterface::snUpdateMessage(long id, std::wstring title, 
	std::wstring text, std::wstring icon)
{
	SNARLSTRUCT snarlStruct;
	snarlStruct.id = id;
	snarlStruct.cmd = SNARL_UPDATE;

	int len;
	char* buf = convertToMultiByte(title, &len);
	strncpy(snarlStruct.title, buf, SNARL_STRING_LENGTH - 1);
	snarlStruct.title[len] = 0;
	delete[] buf;

	buf = convertToMultiByte(text, &len);
	strncpy(snarlStruct.text, buf, SNARL_STRING_LENGTH - 1);
	snarlStruct.text[len] = 0;
	delete[] buf;

	buf = convertToMultiByte(icon, &len);
	strncpy(snarlStruct.icon, buf, SNARL_STRING_LENGTH - 1);
	snarlStruct.icon[len] = 0;
	delete[] buf;

	return static_cast<bool>(send(snarlStruct));
}

bool SnarlInterface::snRegisterConfig(HWND hWnd, std::wstring appName,
	long replyMsg)
{
	SNARLSTRUCT snarlStruct;
	snarlStruct.cmd = SNARL_REGISTER_CONFIG_WINDOW;
	snarlStruct.lngData2 = reinterpret_cast<long>(hWnd);
	snarlStruct.id = replyMsg;

	int len;
	char* buf = convertToMultiByte(appName, &len);
	strncpy(snarlStruct.title, buf, SNARL_STRING_LENGTH - 1);
	snarlStruct.title[len] = 0;
	delete[] buf;

	return static_cast<bool>(send(snarlStruct));
}

bool SnarlInterface::snRegisterConfig2(HWND hWnd, std::wstring appName,
	long replyMsg, std::wstring icon)
{
	SNARLSTRUCT snarlStruct;

	snarlStruct.cmd = SNARL_REGISTER_CONFIG_WINDOW_2;
	snarlStruct.lngData2 = reinterpret_cast<long>(hWnd);
	snarlStruct.id = replyMsg;

	int len;
	char* buf = convertToMultiByte(appName, &len);
	strncpy(snarlStruct.title, buf, SNARL_STRING_LENGTH - 1);
	snarlStruct.title[len] = 0;
	delete[] buf;

	buf = convertToMultiByte(icon, &len);
	strncpy(snarlStruct.icon, buf, SNARL_STRING_LENGTH - 1);
	snarlStruct.icon[len] = 0;
	delete[] buf;

	return static_cast<bool>(send(snarlStruct));
}

bool SnarlInterface::snRevokeConfig(HWND hWnd)
{
	SNARLSTRUCT snarlStruct;
	snarlStruct.cmd = SNARL_REVOKE_CONFIG_WINDOW;
	snarlStruct.lngData2 = reinterpret_cast<long>(hWnd);
	return static_cast<bool>(send(snarlStruct));
}

bool SnarlInterface::snRegisterAlert(std::wstring appName, std::wstring alertName)
{
	SNARLSTRUCT snarlStruct;
	snarlStruct.cmd = SNARL_REGISTER_ALERT;

	int len;
	char* buf = convertToMultiByte(appName, &len);
	strncpy(snarlStruct.title, buf, SNARL_STRING_LENGTH - 1);
	snarlStruct.title[len] = 0;
	delete[] buf;

	buf = convertToMultiByte(alertName, &len);
	strncpy(snarlStruct.text, buf, SNARL_STRING_LENGTH - 1);
	snarlStruct.text[len] = 0;
	delete[] buf;

	return static_cast<bool>(send(snarlStruct));
}

bool SnarlInterface::snGetVersion(int* major, int* minor)
{
	SNARLSTRUCT snarlStruct;
	snarlStruct.cmd = SNARL_GET_VERSION;
	long versionInfo = send(snarlStruct);
	if (versionInfo) {
		int maj = static_cast<int>(HIWORD(versionInfo));
		*major = maj;
		int min = static_cast<int>(LOWORD(versionInfo));
		*minor = min;
		return true;
	}
	return false;
}

long SnarlInterface::snGetVersionEx()
{
	SNARLSTRUCT snarlStruct;
	snarlStruct.cmd = SNARL_GET_VERSION_EX;
	return send(snarlStruct);
}

HWND SnarlInterface::snGetSnarlWindow()
{
	return FindWindowA(NULL, "Snarl");
}

long SnarlInterface::snGetGlobalMsg()
{
	return RegisterWindowMessageA(SNARL_GLOBAL_MESSAGE.c_str());
}

std::string SnarlInterface::snGetIconsPath()
{
	return snGetAppPath() + "\\etc\\icons";
}

std::string SnarlInterface::snGetAppPath()
{
	HKEY hKey;
	DWORD cb;
	std::string retVal;

	if (RegOpenKeyA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\Snarl.exe", &hKey)) {
		return "";
	}

	if (!RegQueryValueEx(hKey, NULL, NULL, NULL, NULL, &cb)) {
		if (cb) {
			char *val = new char[cb + 1];
			if (!RegQueryValueEx(hKey, NULL, NULL, NULL, (LPBYTE)val, &cb)) {
				retVal = getPath(val);
				delete[] val;
			}
		}
	}
	RegCloseKey(hKey);
	if (retVal[retVal.length() - 1] != '\\')
		retVal += "\\";

	return retVal;
}

std::string SnarlInterface::getPath(std::string path)
{
	if (path == "")
		return path;

	if (path[path.length() - 1] == '\\') {
		return path.substr(0, path.length() - 1);
	}
	else {
		return path.substr(0, path.find_last_of("\\"));
	}
}

LPSTR SnarlInterface::convertToMultiByte(std::wstring str, int *len)
{
	*len = WideCharToMultiByte(CP_UTF8, 0, str.c_str(), str.length(), NULL, 0, NULL, NULL);
	char* buf = new char[*len + 1];
	WideCharToMultiByte(CP_UTF8, 0, str.c_str(), str.length(), buf, *len, NULL, NULL);

	return buf;
}

std::wstring SnarlInterface::convertTowstring(LPSTR str)
{
	std::wstring wstr;
	int length = lstrlenA(str) + 1;
	wchar_t* szTextW = new wchar_t[length];
	MultiByteToWideChar(0, 0, str, -1, szTextW, length);
	wstr = szTextW;
	delete[] szTextW;
	return wstr;
}

template <class T>
long SnarlInterface::send(T snarlStruct)
{
	HWND hWnd = snGetSnarlWindow();
	if (IsWindow(hWnd))
	{
		DWORD lr;
		COPYDATASTRUCT cds;
		cds.dwData = 2;
		cds.cbData = sizeof(snarlStruct);
		cds.lpData = &snarlStruct;
		SendMessageTimeout(hWnd, WM_COPYDATA, 0, (LPARAM)&cds, SMTO_ABORTIFHUNG, 100, &lr);
		if (lr)
		{
			return lr;
		}
	}
	return 0;
}
