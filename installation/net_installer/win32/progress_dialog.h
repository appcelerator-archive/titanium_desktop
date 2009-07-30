/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license. 
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#pragma once
#include <windows.h>
#include <shlobj.h>
#include <shlguid.h>
#include <string>

class ProgressDialog
{
	public:
	ProgressDialog();
	~ProgressDialog();

	void SetTitle(std::wstring title);
	void SetCancelMessage(std::wstring message);
	void SetLineText(DWORD line, std::string message, bool compact);
	void SetLineText(DWORD line, std::wstring message, bool compact);
	void Update(DWORD value, DWORD max);
	void Show();
	void Hide();
	bool IsCancelled();

	private:
	IProgressDialog* dialog;
};
