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
		Win32PopupDialog(HWND _windowHandle);
		virtual ~Win32PopupDialog();

		void SetShowInputText(bool flag) { this->showInputText = flag; }
		void SetTitle(std::string _title) { this->title = _title; }
		void SetMessage(std::string _message) { this->message = _message; }
		void SetInputText(std::string _inputText) { this->inputText = _inputText; }
		std::string GetInputText() { return this->inputText; }
		void SetShowCancelButton(bool flag) { this->showCancelButton = flag; }
		int ParseMessage(std::string stringMsg, std::string stringSearch);

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
