#pragma once

class CMainDlg : public CDialogImpl<CMainDlg>
{
public:
	enum { IDD = IDD_DIALOG1 };

	BEGIN_MSG_MAP(CMainDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, CommandHandler)
		COMMAND_ID_HANDLER(IDCANCEL, CommandHandler)
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT uMsg, WPARAM wparam, LPARAM lparam, BOOL &bHandled );
	LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);


	CMainDlg(void);
	~CMainDlg(void);

	HMODULE hModlibeay32;
	HMODULE hModssleay32;
	HMODULE hModzlib;
	HMODULE hModlibxml2;
};
