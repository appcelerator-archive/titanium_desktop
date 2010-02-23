/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license. 
 * Copyright (c) 2009-2010 Appcelerator, Inc. All Rights Reserved.
 */

#include <kroll/utils/utils.h>
#include "progress_dialog.h"

using std::string;
using std::wstring;

ProgressDialog::ProgressDialog() :
	dialogWindow(0)
{
	HRESULT hr = CoCreateInstance(
		CLSID_ProgressDialog,
		0,
		CLSCTX_INPROC_SERVER,
		IID_IProgressDialog, (void**) &dialog );
}

ProgressDialog::~ProgressDialog()
{
	dialog->Release();
	CoUninitialize();
}

void ProgressDialog::SetCancelMessage(const std::wstring& message)
{
	dialog->SetCancelMsg(message.c_str(), 0);
}

void ProgressDialog::SetTitle(const std::wstring& title)
{
	dialog->SetTitle(title.c_str());
}

void ProgressDialog::SetLineText(DWORD line, const std::wstring& message, bool compact)
{
	dialog->SetLine(line, message.c_str(), compact, 0);
}

void ProgressDialog::Update(DWORD value, DWORD max)
{
	dialog->SetProgress(value, max);
}

bool ProgressDialog::IsCancelled()
{
	return dialog->HasUserCancelled()==TRUE;
}

void ProgressDialog::Show()
{
	DWORD flags = PROGDLG_NORMAL | PROGDLG_NOMINIMIZE;
	HRESULT hr = dialog->StartProgressDialog(GetDesktopWindow(), 0, flags, 0);
	if (!SUCCEEDED(hr))
		return;

	dialog->Timer(PDTIMER_RESET, 0);
	IOleWindow* pIWnd = 0;
	if (dialog->QueryInterface(IID_IOleWindow, (void**)&pIWnd) != S_OK)
		return;

	HWND windowHandle;
	if (pIWnd->GetWindow(&windowHandle) != S_OK)
	{
		pIWnd->Release();
		return;
	}

	// Get the center of the screen.
	this->dialogWindow = windowHandle;
	HDC hScreenDC = CreateCompatibleDC(0);
	int screenWidth = GetDeviceCaps(hScreenDC, HORZRES);
	int screenHeight = GetDeviceCaps(hScreenDC, VERTRES);
	DeleteDC(hScreenDC);

	// Get the dialog size.
	RECT dialogRect;
	GetWindowRect(dialogWindow, &dialogRect);

	// Calculate center position for the dialog and reposition it.
	int centerX = ( screenWidth - (dialogRect.right - dialogRect.left)) / 2;
	int centerY = ( screenHeight - (dialogRect.bottom - dialogRect.top)) / 2;
	SetWindowPos(this->dialogWindow, 0, centerX, centerY-20, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

void ProgressDialog::Hide()
{
	dialog->StopProgressDialog();
}

