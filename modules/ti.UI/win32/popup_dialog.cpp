/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */
#include "../ui_module.h"
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

	Win32PopupDialog::Win32PopupDialog(HWND _windowHandle) :
		windowHandle(_windowHandle),
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
	void Win32PopupDialog::HandleOKClick(HWND hDlg)
	{
		TCHAR textEntered[MAX_INPUT_LENGTH];
		GetDlgItemText(hDlg, ID_INPUT_FIELD, textEntered, MAX_INPUT_LENGTH);

		Win32PopupDialog* popupDialog = popups[GetCurrentThreadId()];
		if (popupDialog)
		{
			std::wstring textEnteredW = textEntered;
			std::string textEnteredU = WideToUTF8(textEnteredW);
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
	INT_PTR CALLBACK Win32PopupDialog::Callback(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (iMsg)
		{
			case WM_INITDIALOG:
			{
				Win32PopupDialog* popupDialog = popups[GetCurrentThreadId()];
				HWND hwndOwner = (popupDialog == NULL ? NULL : popupDialog->windowHandle);
				if (hwndOwner == NULL)
				{
					hwndOwner = GetDesktopWindow();
				}

				RECT rcOwner;
				RECT rcDlg;
				RECT rc;
				GetWindowRect(hwndOwner, &rcOwner);
				GetWindowRect(hDlg, &rcDlg);
				CopyRect(&rc, &rcOwner);

				// Offset the owner and dialog box rectangles so that right and bottom
				// values represent the width and height, and then offset the owner again
				// to discard space taken up by the dialog box.
				OffsetRect(&rcDlg, -rcDlg.left, -rcDlg.top);
				OffsetRect(&rc, -rc.left, -rc.top);
				OffsetRect(&rc, -rcDlg.right, -rcDlg.bottom);

				SetWindowPos(hDlg,
							HWND_TOP,
							rcOwner.left + (rc.right / 2),
							rcOwner.top + (rc.bottom / 2),
							0, 0,							// Ignores size arguments.
							SWP_NOSIZE);

				return TRUE;
			}
			case WM_COMMAND:
				if(GET_WM_COMMAND_ID(wParam, lParam) == IDOK)
				{
					Win32PopupDialog::HandleOKClick(hDlg);
					EndDialog(hDlg, 0);
				}
				else if(GET_WM_COMMAND_ID(wParam, lParam) == IDCANCEL)
				{
					EndDialog(hDlg, 0);
				}
				break;
		}

		return FALSE;
	}

	int Win32PopupDialog::ParseMessage(std::string stringMsg, std::string stringSearch)
	{
		int NumberOfOccurrences = 0;
		size_t pos = 0; 
		while (pos != std::string::npos) 
		{
			pos = stringMsg.find(stringSearch, pos);
			if (pos!=std::string::npos)
			{
				NumberOfOccurrences++;
				pos = pos+stringSearch.size();
			}
		}
		return NumberOfOccurrences;
	}
	
	BOOL Win32PopupDialog::ShowMessageBox(HWND hwnd)
	{
		BOOL fSuccess = FALSE;
		HDC hdc = GetDC(hwnd);

		if (hdc)
		{
			NONCLIENTMETRICSW ncm = { sizeof(ncm) };
			if (SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, 0, &ncm, 0)) {
				DialogTemplate tmp;
				std::wstring ws;

				int controlCount = 2;	// at minimum, static label and OK button
				if(this->showCancelButton) controlCount++;
				if(this->showInputText) controlCount++;

				int NumberOfReturns = 0;
				int NumberOfTabs = 0;
				int NumberOfSpace = 0;
				int messageLines = 0;
				int messageHeight = 0;
		
				NumberOfReturns = ParseMessage(message, "\n");
				NumberOfTabs    = ParseMessage(message, "\t");
				NumberOfSpace   = ParseMessage(message, " ");

				if( (NumberOfReturns == 0) && (NumberOfTabs == 0) && (NumberOfSpace == 0) )
				{
					std::string tempMessage = message;
					if(tempMessage.length() != 0)
					{
						int insertAt = 60;
						int count = 0;

						std::string::iterator it;
						for(it=tempMessage.begin();it < tempMessage.end(); it++)
						{
							count++;
							if(count == 60)
							{	
								count = 0;
								message.insert(insertAt,"\n");
								insertAt = insertAt + 60;
							}
						}
					}
				}

				if( (NumberOfReturns != 0) && (NumberOfTabs != 0) )
				{
					messageLines = (message.length()/60) + NumberOfReturns + (NumberOfTabs/14) + 1;
					messageHeight = (messageLines * 12);
				}
				else if(NumberOfReturns != 0)
				{
					messageLines = (message.length()/60) + NumberOfReturns + 1;
					messageHeight = (messageLines * 12);

				}
				else if(NumberOfTabs != 0)
				{
					messageLines = (message.length()/60) + (NumberOfTabs/14) + 1;
					messageHeight = (messageLines * 12);
				}
				else
				{
					messageLines = message.length()/60;
					messageHeight = (messageLines * 12);
				}

				int labelHeight = 14;
				int width = 400;
				int height = messageHeight +  56; //ButtonHeight+Space; //90;
				int margin = 10;
				int buttonWidth = 50;
				int buttonHeight = 14;
				int inputHeight = 14;

				if(! this->showInputText)
				{
					height -= (inputHeight + margin);
				}

				// Write out the extended dialog template header
				tmp.Write<WORD>(1); // dialog version
				tmp.Write<WORD>(0xFFFF); // extended dialog template
				tmp.Write<DWORD>(0); // help ID
				tmp.Write<DWORD>(0); // extended style
				tmp.Write<DWORD>(WS_CAPTION | DS_ABSALIGN | DS_FIXEDSYS | DS_SETFONT | DS_MODALFRAME);	// DS_FIXEDSYS removes the close decoration
				tmp.Write<WORD>(controlCount); // number of controls
				tmp.Write<WORD>(32); // X
				tmp.Write<WORD>(32); // Y
				tmp.Write<WORD>(width); // width
				tmp.Write<WORD>(height); // height
				tmp.WriteString(L""); // no menu
				tmp.WriteString(L""); // default dialog class
				//tmp.WriteString(pszTitle); // title
				tmp.WriteString(ws.assign(title.begin(), title.end()).c_str()); // title

				// Next comes the font description.
				// See text for discussion of fancy formula.
				if (ncm.lfMessageFont.lfHeight < 0)
				{
					ncm.lfMessageFont.lfHeight = -MulDiv(ncm.lfMessageFont.lfHeight, 72, GetDeviceCaps(hdc, LOGPIXELSY));
				}
				tmp.Write<WORD>((WORD)ncm.lfMessageFont.lfHeight); // point
				tmp.Write<WORD>((WORD)ncm.lfMessageFont.lfWeight); // weight
				tmp.Write<BYTE>(ncm.lfMessageFont.lfItalic); // Italic
				tmp.Write<BYTE>(ncm.lfMessageFont.lfCharSet); // CharSet
				tmp.WriteString(ncm.lfMessageFont.lfFaceName);

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
				tmp.WriteString(ws.assign(message.begin(), message.end()).c_str()); // text
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
				tmp.Write<DWORD>(IDOK); // control ID
				tmp.Write<DWORD>(0x0080FFFF); // button class atom
				tmp.WriteString(L"OK"); // text
				tmp.Write<WORD>(0); // no extra data

				if(this->showCancelButton)
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
					tmp.Write<DWORD>(IDCANCEL); // control ID
					tmp.Write<DWORD>(0x0080FFFF); // button class atom
					tmp.WriteString(L"Cancel"); // text
					tmp.Write<WORD>(0); // no extra data
				}

				if(this->showInputText)
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
					tmp.Write<DWORD>(0x0081FFFF); // edit class atom
					tmp.WriteString(ws.assign(inputText.begin(), inputText.end()).c_str()); // text
					tmp.Write<WORD>(0); // no extra data
				}

				// Template is ready - go display it.
				fSuccess = DialogBoxIndirect(GetModuleHandle(NULL), tmp.Template(), hwnd, &Win32PopupDialog::Callback) >= 0;
			}
			ReleaseDC(NULL, hdc); // fixed 11 May
		}
		return fSuccess;
	}
}
