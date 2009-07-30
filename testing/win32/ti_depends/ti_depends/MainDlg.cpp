#include "StdAfx.h"
#include "MainDlg.h"

CMainDlg::CMainDlg(void)
{
	hModlibeay32 = NULL;
	hModssleay32 = NULL;
	hModzlib = NULL;
	hModlibxml2 = NULL;
}

CMainDlg::~CMainDlg(void)
{
}


LRESULT CMainDlg::OnInitDialog(UINT uMsg, WPARAM wparam, LPARAM lparam, BOOL &bHandled )
{
	// center the dialog on the screen
	CenterWindow();

	if ( hModlibeay32 )
	{
		TCHAR szBuf[MAX_PATH];
		DWORD dwRet = ::GetModuleFileName(hModlibeay32, szBuf, MAX_PATH);
		if ( dwRet != 0)
		{
			::SetDlgItemText(this->m_hWnd, IDC_EDIT1,szBuf);
		}
	}
	if ( hModssleay32 )
	{
		TCHAR szBuf[MAX_PATH];
		DWORD dwRet = ::GetModuleFileName(hModssleay32, szBuf, MAX_PATH);
		if ( dwRet != 0)
		{
			::SetDlgItemText(this->m_hWnd, IDC_EDIT2,szBuf);
		}
	}
	if ( hModzlib )
	{
		TCHAR szBuf[MAX_PATH];
		DWORD dwRet = ::GetModuleFileName(hModzlib, szBuf, MAX_PATH);
		if ( dwRet != 0)
		{
			::SetDlgItemText(this->m_hWnd, IDC_EDIT3,szBuf);
		}
	}
	if ( hModlibxml2 )
	{
		TCHAR szBuf[MAX_PATH];
		DWORD dwRet = ::GetModuleFileName(hModlibxml2, szBuf, MAX_PATH);
		if ( dwRet != 0)
		{
			::SetDlgItemText(this->m_hWnd, IDC_EDIT4,szBuf);
		}
	}

	return TRUE;
}

LRESULT CMainDlg::CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	BOOL bRet = FALSE;

	switch(wID)
	{
	case IDOK:
	case IDCANCEL:
		bHandled = TRUE;
		bRet = EndDialog(wID);
	}

	return bRet;
}

