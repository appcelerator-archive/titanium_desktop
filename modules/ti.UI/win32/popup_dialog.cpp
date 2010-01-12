/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */
#include "../ui_module.h"
#include <math.h>
#define ID_INPUT_FIELD 101

namespace ti
{
	std::map<DWORD, Win32PopupDialog*> Win32PopupDialog::popups;

	class DialogTemplate
	{
	public:
		LPCDLGTEMPLATE Template()
		{
			return (LPCDLGTEMPLATE)&v[0];
		}

		void AlignToDword()
		{
			if (v.size() % 4) Write(NULL, 4 - (v.size() % 4));
		}

		void Write(LPCVOID pvWrite, DWORD cbWrite)
		{
			v.insert(v.end(), cbWrite, 0);
			if (pvWrite)
			{
				CopyMemory(&v[v.size() - cbWrite], pvWrite, cbWrite);
			}
		}

		template<typename T> void Write(T t)
		{
			Write(&t, sizeof(T));
		}

		void WriteString(LPCWSTR psz)
		{
			Write(psz, (lstrlenW(psz) + 1) * sizeof(WCHAR));
		}

	private:
		std::vector<BYTE> v;
	};

	Win32PopupDialog::Win32PopupDialog(HWND windowHandle) :
		windowHandle(windowHandle),
		showInputText(false),
		showCancelButton(false),
		result(IDNO)
	{
	}

	Win32PopupDialog::~Win32PopupDialog()
	{
	}

	int Win32PopupDialog::Show()
	{
		popups[GetCurrentThreadId()] = this;
		ShowMessageBox(windowHandle);

		return result;
	}

	/*static*/
	void Win32PopupDialog::HandleOKClick(HWND dialog)
	{
		TCHAR textEntered[MAX_INPUT_LENGTH];
		GetDlgItemText(dialog, ID_INPUT_FIELD, textEntered, MAX_INPUT_LENGTH);

		Win32PopupDialog* popupDialog = popups[GetCurrentThreadId()];
		if (popupDialog)
		{
			std::wstring textEnteredW = textEntered;
			std::string textEnteredU = ::WideToUTF8(textEnteredW);
			popupDialog->inputText.clear();
			popupDialog->inputText.append(textEnteredU);
			popupDialog->result = IDOK;
		}
		else
		{
			std::cerr << "unable to find popup dialog for current thread" << std::endl;
		}
	}

	/*static*/
	INT_PTR CALLBACK Win32PopupDialog::Callback(HWND dialog, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
			case WM_INITDIALOG:
			{
				Win32PopupDialog* popupDialog = popups[GetCurrentThreadId()];
				HWND parentWindow = (popupDialog == NULL ? NULL : popupDialog->windowHandle);
				if (parentWindow == NULL)
				{
					parentWindow = GetDesktopWindow();
				}

				RECT parentRect, dialogRect;
				GetWindowRect(parentWindow, &parentRect);
				GetWindowRect(dialog, &dialogRect);
				int x = parentRect.left + (parentRect.right - parentRect.left) / 2;
				int y = parentRect.top + (parentRect.bottom - parentRect.top) / 2;
				x -= (dialogRect.right - dialogRect.left) / 2;
				y -= (dialogRect.bottom - dialogRect.top) / 2;
				
				SetWindowPos(dialog, HWND_TOP, x, y, 0, 0, SWP_NOSIZE);

				if (popupDialog->showInputText)
				{
					if (GetDlgCtrlID((HWND) wParam) != ID_INPUT_FIELD)
					{
						HWND inputText = GetDlgItem(dialog, ID_INPUT_FIELD);
						SetFocus(inputText);
						SendMessage(inputText, EM_SETSEL,
							0, popupDialog->inputText.length());
						return FALSE;
					}
				}
				
				return TRUE;
			}
			case WM_COMMAND:
				if(GET_WM_COMMAND_ID(wParam, lParam) == IDOK)
				{
					Win32PopupDialog::HandleOKClick(dialog);
					EndDialog(dialog, 0);
				}
				else if(GET_WM_COMMAND_ID(wParam, lParam) == IDCANCEL)
				{
					EndDialog(dialog, 0);
				}
				break;
		}

		return FALSE;
	}

	int Win32PopupDialog::CountMatches(std::string& message, const char *token)
	{
		int matches = 0;
		size_t pos = 0, tokenSize = strlen(token);
		while (pos != std::string::npos) 
		{
			pos = message.find(token, pos);
			if (pos != std::string::npos)
			{
				matches++;
				pos += tokenSize;
			}
		}
		return matches;
	}
	
	BOOL Win32PopupDialog::ShowMessageBox(HWND hwnd)
	{
		BOOL fSuccess = FALSE;
		HDC hdc = GetDC(hwnd);

		if (hdc)
		{
			NONCLIENTMETRICSW nonClientMetrics = { sizeof(NONCLIENTMETRICSW) };
			if (SystemParametersInfoW(
				SPI_GETNONCLIENTMETRICS, 0, &nonClientMetrics, 0))
			{
				DialogTemplate tmp;
				std::wstring wideTitle(::UTF8ToWide(title));
				std::wstring wideInputText;
				if (this->showInputText)
					wideInputText.assign(::UTF8ToWide(inputText));

				int controlCount = 2;	// at minimum, static label and OK button
				if (this->showCancelButton)
					controlCount++;
				if (this->showInputText)
					controlCount++;

				int messageLines = 0;
				int newlines = CountMatches(message, "\n");
				int tabs = CountMatches(message, "\t");
				int spaces = CountMatches(message, " ");

				if (newlines == 0 && tabs == 0 && spaces == 0)
				{
					std::string tempMessage(message);
					if (tempMessage.length() != 0)
					{
						int insertAt = 60;
						int count = 0;

						std::string::iterator it = tempMessage.begin();
						for (; it < tempMessage.end(); it++)
						{
							count++;
							if (count == 60)
							{	
								count = 0;
								message.insert(insertAt, "\n");
								insertAt += 60;
							}
						}
					}
				}
				
				std::wstring wideMessage(::UTF8ToWide(message));
				messageLines = message.length() / 60;
				messageLines += ((int) ceil((double)tabs / 14));
				messageLines += newlines;
				
				if (tabs == 0 || newlines == 0)
					messageLines++;

				int labelHeight = 14;
				int width = 200;
				int margin = 10;
				int buttonWidth = 50;
				int buttonHeight = 14;
				int inputHeight = 14;
				int messageHeight = (messageLines * 12) + (messageLines * margin);
				int height = messageHeight + 56;
				
				if (!this->showInputText)
				{
					height -= (inputHeight + margin);
				}

				// Write out the extended dialog template header
				tmp.Write<WORD>(1); // dialog version
				tmp.Write<WORD>(0xFFFF); // extended dialog template
				tmp.Write<DWORD>(0); // help ID
				tmp.Write<DWORD>(0); // extended style
				tmp.Write<DWORD>(WS_CAPTION | WS_BORDER | DS_ABSALIGN | DS_SETFONT);
				tmp.Write<WORD>(controlCount); // number of controls
				tmp.Write<WORD>(32); // X
				tmp.Write<WORD>(32); // Y
				tmp.Write<WORD>(width); // width
				tmp.Write<WORD>(height); // height
				tmp.WriteString(L""); // no menu
				tmp.WriteString(L""); // default dialog class
				//tmp.WriteString(pszTitle); // title
				tmp.WriteString(wideTitle.c_str()); // title

				// Next comes the font description.
				// See text for discussion of fancy formula.
				
				if (nonClientMetrics.lfMessageFont.lfHeight < 0)
				{
					int dpi = GetDeviceCaps(hdc, LOGPIXELSY);
					nonClientMetrics.lfMessageFont.lfHeight =
						-MulDiv(nonClientMetrics.lfMessageFont.lfHeight, 72, dpi);
				}
				tmp.Write<WORD>((WORD)nonClientMetrics.lfMessageFont.lfHeight); // point
				tmp.Write<WORD>((WORD)nonClientMetrics.lfMessageFont.lfWeight); // weight
				tmp.Write<BYTE>(nonClientMetrics.lfMessageFont.lfItalic); // Italic
				tmp.Write<BYTE>(nonClientMetrics.lfMessageFont.lfCharSet); // CharSet
				tmp.WriteString(nonClientMetrics.lfMessageFont.lfFaceName);

				// First control - static label
				tmp.AlignToDword();
				tmp.Write<DWORD>(0); // help id
				tmp.Write<DWORD>(0); // window extended style
				tmp.Write<DWORD>(WS_CHILD | WS_VISIBLE | SS_LEFT); // style
				tmp.Write<WORD>(margin); // x
				tmp.Write<WORD>(margin); // y
				tmp.Write<WORD>(width - (2 * margin)); // width
				//tmp.Write<WORD>(labelHeight); // height
				tmp.Write<WORD>(messageHeight); // height
				tmp.Write<DWORD>(-1); // control ID
				tmp.Write<DWORD>(0x0082FFFF); // static
				//tmp.Write<DWORD>(SS_LEFT);
				tmp.WriteString(wideMessage.c_str()); // text
				tmp.Write<WORD>(0); // no extra data

				// Second control - the OK button.
				tmp.AlignToDword();
				tmp.Write<DWORD>(0); // help id
				tmp.Write<DWORD>(0); // window extended style
				tmp.Write<DWORD>(WS_CHILD | WS_VISIBLE | WS_GROUP | WS_TABSTOP | BS_DEFPUSHBUTTON); // style
				tmp.Write<WORD>(width - margin - buttonWidth); // x
				tmp.Write<WORD>(height - margin - buttonHeight); // y
				tmp.Write<WORD>(buttonWidth); // width
				tmp.Write<WORD>(buttonHeight); // height
				tmp.Write<DWORD>(IDCANCEL); // control ID
				tmp.Write<DWORD>(0x0080FFFF); // button class atom
				tmp.WriteString(L"Cancel"); // text
				tmp.Write<WORD>(0); // no extra data

				if (this->showCancelButton)
				{
					// The Cancel button
					tmp.AlignToDword();
					tmp.Write<DWORD>(0); // help id
					tmp.Write<DWORD>(0); // window extended style
					tmp.Write<DWORD>(WS_CHILD | WS_VISIBLE | WS_GROUP | WS_TABSTOP | BS_DEFPUSHBUTTON); // style
					tmp.Write<WORD>(width - 2 * margin - 2 * buttonWidth); // x
					tmp.Write<WORD>(height - margin - buttonHeight); // y
					tmp.Write<WORD>(buttonWidth); // width
					tmp.Write<WORD>(buttonHeight); // height
					tmp.Write<DWORD>(IDOK); // control ID
					tmp.Write<DWORD>(0x0080FFFF); // button class atom
					tmp.WriteString(L"OK"); // text
					tmp.Write<WORD>(0); // no extra data
				}

				if (this->showInputText)
				{
					// The input field
					tmp.AlignToDword();
					tmp.Write<DWORD>(0); // help id
					tmp.Write<DWORD>(0); // window extended style
					tmp.Write<DWORD>(ES_LEFT | WS_BORDER | WS_TABSTOP | WS_CHILD | WS_VISIBLE); // style
					tmp.Write<WORD>(margin); // x
					tmp.Write<WORD>(margin + labelHeight + margin); // y
					tmp.Write<WORD>(width - (2 * margin)); // width
					tmp.Write<WORD>(inputHeight); // height
					tmp.Write<DWORD>(ID_INPUT_FIELD); // control ID
					tmp.Write<DWORD>(0x0081FFFF); // edit  class atom
					tmp.WriteString(wideInputText.c_str()); // text
					tmp.Write<WORD>(0); // no extra data
				}

				// Template is ready - go display it.
				fSuccess = DialogBoxIndirect(
					GetModuleHandle(NULL),
					tmp.Template(),
					hwnd,
					&Win32PopupDialog::Callback) >= 0;
			}
			ReleaseDC(NULL, hdc); // fixed 11 May
		}
		return fSuccess;
	}
}
