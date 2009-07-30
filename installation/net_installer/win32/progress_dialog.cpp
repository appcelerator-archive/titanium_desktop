/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license. 
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#include "api/utils/utils.h"
#include "progress_dialog.h"
#include "Resource.h"

using std::string;
using std::wstring;

ProgressDialog::ProgressDialog()
{
	HRESULT hr = CoCreateInstance(
		CLSID_ProgressDialog,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_IProgressDialog, (void**) &dialog );

	if (SUCCEEDED(hr))
	{
		dialog->SetAnimation(GetModuleHandle(NULL), IDR_PROGRESS);
	}
}

ProgressDialog::~ProgressDialog()
{
	dialog->Release();
	CoUninitialize();
}

void ProgressDialog::SetCancelMessage(std::wstring message)
{
	dialog->SetCancelMsg (message.c_str(),NULL);
}

void ProgressDialog::SetTitle(std::wstring title)
{
	dialog->SetTitle(title.c_str());
}

void ProgressDialog::SetLineText(DWORD line, std::string message, bool compact)
{
	std::wstring wMessage = KrollUtils::UTF8ToWide(message);
	this->SetLineText(line, wMessage, compact);
}

void ProgressDialog::SetLineText(DWORD line, std::wstring message, bool compact)
{
	dialog->SetLine(line, message.c_str(), compact, NULL);
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
	DWORD flags = PROGDLG_NORMAL | PROGDLG_AUTOTIME | PROGDLG_NOMINIMIZE;
	HRESULT hr = dialog->StartProgressDialog(GetDesktopWindow(), NULL, flags, NULL);

	if (SUCCEEDED(hr))
	{
		dialog->Timer(PDTIMER_RESET, NULL);
		IOleWindow* pIWnd = NULL;
		if (dialog->QueryInterface(IID_IOleWindow, (void**)&pIWnd) == S_OK)
		{
			HWND hWnd = NULL; 
			if (pIWnd->GetWindow( &hWnd ) == S_OK)
			{
				// get center of screen
				HDC hScreenDC = CreateCompatibleDC(NULL);
				int screenWidth = GetDeviceCaps(hScreenDC, HORZRES);
				int screenHeight = GetDeviceCaps(hScreenDC, VERTRES);
				DeleteDC(hScreenDC);

				// get dialog size
				RECT dialogRect;
				GetWindowRect(hWnd, &dialogRect);

				// calculate center position for dialog and reposition
				int centerX = ( screenWidth - (dialogRect.right - dialogRect.left)) / 2;
				int centerY = ( screenHeight - (dialogRect.bottom - dialogRect.top)) / 2;
				SetWindowPos(hWnd, NULL, centerX, centerY-20, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
			}
		}
		pIWnd->Release();
	}
}

void ProgressDialog::Hide()
{
	dialog->StopProgressDialog();
}

