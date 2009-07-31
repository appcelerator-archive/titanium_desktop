#pragma once

class CMainDialog : public CDialogImpl<CMainDialog>
{
public:
	enum { IDD = IDD_DIALOG1 };

	BEGIN_MSG_MAP(CMainDialog)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, CommandHandler)
		COMMAND_ID_HANDLER(IDCANCEL, CommandHandler)
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT uMsg, WPARAM wparam, LPARAM lparam, BOOL &bHandled );
	LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);


	CMainDialog(void);
	~CMainDialog(void);

	HMODULE hModlibeay32;
	HMODULE hModssleay32;
	HMODULE hModzlib;
	HMODULE hModlibxml2;
};
