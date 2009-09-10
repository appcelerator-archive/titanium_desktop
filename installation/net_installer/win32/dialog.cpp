#include "dialog.h"
#include "api/utils/utils.h"
#include <stdio.h>

namespace ti
{
	static void* SetWindowUserData(HWND hwnd, void* user_data)
	{
		return reinterpret_cast<void*> (SetWindowLongPtr(hwnd, GWLP_USERDATA,
				reinterpret_cast<LONG_PTR> (user_data)));
	}

	static void* GetWindowUserData(HWND hwnd)
	{
		return reinterpret_cast<void*> (GetWindowLongPtr(hwnd, GWLP_USERDATA));
	}

	Dialog::Dialog ()
	{
		// subclasses must call Create so the virtual table is setup correctly
	}

	void Dialog::Create(int id)
	{
		this->window = CreateDialogParam(
				::GetModuleHandle(NULL),
				MAKEINTRESOURCE(id),
				0,
				Dialog::DialogProc,
				(LPARAM)this);

		SetWindowUserData(this->window, this);
	}
	
	void Dialog::Initialize(HWND window)
	{
		// Center the dialog
		HDC hScreenDC = CreateCompatibleDC(NULL);
		int screenWidth = GetDeviceCaps(hScreenDC, HORZRES);
		int screenHeight = GetDeviceCaps(hScreenDC, VERTRES);
		DeleteDC(hScreenDC);
		RECT dialogRect;
		GetWindowRect(window, &dialogRect);
		int centerX = ( screenWidth - (dialogRect.right - dialogRect.left)) / 2;
		int centerY = ( screenHeight - (dialogRect.bottom - dialogRect.top)) / 2;
		SetWindowPos(window, NULL, centerX, centerY-20, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	}
	
	void Dialog::Destroy()
	{
		DestroyWindow(this->window);
	}

	/*static*/
	BOOL CALLBACK Dialog::DialogProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		int controlID = LOWORD(wParam);
		int command = HIWORD (wParam);
		
		switch (message)
		{
			case WM_INITDIALOG: {
				Dialog* dialog = reinterpret_cast<Dialog *>(lParam);
				dialog->Initialize(hWnd);
				return TRUE;
			}

			case WM_COMMAND:
				if (command == BN_CLICKED) {
					Dialog* dialog = reinterpret_cast<Dialog*>(GetWindowUserData(hWnd));
					dialog->ButtonClicked(controlID);
				}
				return TRUE;

			case WM_DESTROY:
				PostQuitMessage(0);
				return TRUE;

			case WM_CLOSE: {
				Dialog* dialog = reinterpret_cast<Dialog*>(GetWindowUserData(hWnd));
				dialog->Destroy();
				return TRUE;
			}
		}
		return FALSE;
	}
}
