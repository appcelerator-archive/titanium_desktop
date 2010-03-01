/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license. 
 * Copyright (c) 2009-2010 Appcelerator, Inc. All Rights Reserved.
 */

#include <windows.h>
#include <shlobj.h>
#include <shlguid.h>
#include <string>

class ProgressDialog
{
public:
	ProgressDialog();
	~ProgressDialog();

	void SetTitle(const std::wstring& title);
	void SetCancelMessage(const std::wstring& message);
	void SetLineText(DWORD line, const std::wstring& message, bool compact);
	void Update(DWORD value, DWORD max);
	void Show();
	void Hide();
	bool IsCancelled();
	HWND GetWindowHandle() { return this->dialogWindow; }

private:
	IProgressDialog* dialog;
	HWND dialogWindow;
};
