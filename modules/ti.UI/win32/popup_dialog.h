/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */
#ifndef TI_POPUP_DIALOG_H_
#define TI_POPUP_DIALOG_H_
#define MAX_INPUT_LENGTH 1024

namespace ti {

	class Win32PopupDialog {
	public:
		Win32PopupDialog(HWND windowHandle);
		virtual ~Win32PopupDialog();

		void SetShowInputText(bool showInputText)
		{
			this->showInputText = showInputText;
		}
		void SetTitle(std::string title) { this->title = title; }
		void SetMessage(std::string message) { this->message = message; }
		void SetInputText(std::string inputText) { this->inputText = inputText; }
		std::string GetInputText() { return this->inputText; }
		void SetShowCancelButton(bool showCancelButton)
		{
			this->showCancelButton = showCancelButton;
		}
		
		int CountMatches(std::string& message, const char *token);

		int Show();
	private:
		HWND windowHandle;

		bool showInputText;
		std::string title;
		std::string message;
		std::string inputText;
		bool showCancelButton;
		int result;

		BOOL ShowMessageBox(HWND hwnd);

		static std::map<DWORD, Win32PopupDialog*> popups;

		static void HandleOKClick(HWND hDlg);
		static INT_PTR CALLBACK CALLBACK Callback(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam);
	};

}

#endif /* TI_POPUP_DIALOG_H_ */
