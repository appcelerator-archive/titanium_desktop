#include "StdAfx.h"
#include "main_dialog.h"

#include <string>

extern std::string GetModuleInfo(HMODULE hMod);
CMainDialog::CMainDialog(void)
{
	hModlibeay32 = NULL;
	hModssleay32 = NULL;
	hModzlib = NULL;
	hModlibxml2 = NULL;
}

CMainDialog::~CMainDialog(void)
{
}


LRESULT CMainDialog::OnInitDialog(UINT uMsg, WPARAM wparam, LPARAM lparam, BOOL &bHandled )
{
	// center the dialog on the screen
	CenterWindow();

	if ( hModlibeay32 )
	{
		std::string sOut = GetModuleInfo(hModlibeay32);
		if (!sOut.empty())
		{
			int charLen = MAX_PATH;
			TCHAR szTmp[MAX_PATH] = {0};
			if ( MultiByteToWideChar(CP_ACP, 0, sOut.c_str(), (int)sOut.length(), szTmp, charLen ) )
			{
				::SetDlgItemText(this->m_hWnd, IDC_EDIT1,szTmp);
			}
		}
	}
	else 
	{
		::SetDlgItemText(this->m_hWnd, IDC_EDIT1,_T("libeay32.dll not found in DLL search path"));
	}

	if ( hModssleay32 )
	{
		std::string sOut = GetModuleInfo(hModssleay32);
		if (!sOut.empty())
		{
			int charLen = MAX_PATH;
			TCHAR szTmp[MAX_PATH] = {0};
			if ( MultiByteToWideChar(CP_ACP, 0, sOut.c_str(), (int)sOut.length(), szTmp, charLen ) )
			{
				::SetDlgItemText(this->m_hWnd, IDC_EDIT2,szTmp);
			}
		}
	}
	else 
	{
		::SetDlgItemText(this->m_hWnd, IDC_EDIT3,_T("ssleay32.dll not found in DLL search path"));
	}

	if ( hModzlib )
	{
		std::string sOut = GetModuleInfo(hModzlib);
		if (!sOut.empty())
		{
			int charLen = MAX_PATH;
			TCHAR szTmp[MAX_PATH] = {0};
			if ( MultiByteToWideChar(CP_ACP, 0, sOut.c_str(), (int)sOut.length(), szTmp, charLen ) )
			{
				::SetDlgItemText(this->m_hWnd, IDC_EDIT3,szTmp);
			}
		}
	}
	else 
	{
		::SetDlgItemText(this->m_hWnd, IDC_EDIT3,_T("zlib2.dll not found in DLL search path"));
	}

	if ( hModlibxml2 )
	{
		std::string sOut = GetModuleInfo(hModlibxml2);
		if (!sOut.empty())
		{
			int charLen = MAX_PATH;
			TCHAR szTmp[MAX_PATH] = {0};
			if ( MultiByteToWideChar(CP_ACP, 0, sOut.c_str(), (int)sOut.length(), szTmp, charLen ) )
			{
				::SetDlgItemText(this->m_hWnd, IDC_EDIT4,szTmp);
			}
		}
	}
	else 
	{
		::SetDlgItemText(this->m_hWnd, IDC_EDIT4,_T("libxml2.dll not found in DLL search path"));
	}

	return TRUE;
}

LRESULT CMainDialog::CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
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

