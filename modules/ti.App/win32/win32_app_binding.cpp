/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#include <kroll/kroll.h>
#include "../app_binding.h"
#include <windows.h>

namespace ti
{

void AppBinding::Restart(const ValueList& args, KValueRef result)
{
	Host* host = Host::GetInstance();
	std::wstring cmdline(::UTF8ToWide(host->GetApplication()->arguments.at(0)));

	STARTUPINFO si;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	PROCESS_INFORMATION pi;
	ZeroMemory(&pi, sizeof(pi));

	::CreateProcessW(NULL,
		(LPWSTR) cmdline.c_str(),
		NULL, /*lpProcessAttributes*/
		NULL, /*lpThreadAttributes*/
		FALSE, /*bInheritHandles*/
		NORMAL_PRIORITY_CLASS,
		NULL,
		NULL,
		&si,
		&pi);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	host->Exit(0);
}

void AppBinding::Setup() 
{
}

}
