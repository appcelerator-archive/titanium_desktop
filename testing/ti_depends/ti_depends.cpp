// TiDepends.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "ti_depends.h"
#include "main_dialog.h"
#include "verinfo.h"

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

HMODULE hModlibeay32;
HMODULE hModssleay32;
HMODULE hModzlib;
HMODULE hModlibxml2;
BOOL	bUseConsole = TRUE;	// use stdout by default

// Forward declarations of functions included in this code module:
BOOL				InitInstance(HINSTANCE, int);
string				GetModuleInfo(HMODULE hMod );

//INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	//TODO: add command line switches.
	UNREFERENCED_PARAMETER(hPrevInstance);

	if ( lpCmdLine != NULL )
	{
		// check to see if we have a switch that allows us to use the GUI,
		// otherwise we want to use the console.
		CString szCmdLine = lpCmdLine;	// use a CString because it works better with TCHARS

		if ( (szCmdLine.Find(_T("--gui") ) >= 0) || (szCmdLine.Find(_T("--GUI") ) >= 0))
		{
			bUseConsole = FALSE;
		}
	}

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	int nRet = 0;

	if ( bUseConsole ) 
	{
		AttachConsole(ATTACH_PARENT_PROCESS);

		ifstream conin("con"); //input/output to the newly allocated console
		ofstream conout("con");
		cout.rdbuf(conout.rdbuf()); //attach standard stream objects to allocated console
		cerr.rdbuf(conout.rdbuf()); //(if you don't use cerr and clog you can of course leave them out)
		clog.rdbuf(conout.rdbuf());
		cin.rdbuf(conin.rdbuf());

		if ( hModlibeay32 )
		{
			string sOut = GetModuleInfo(hModlibeay32);
			if (!sOut.empty())
				cout << sOut << endl;
		}

		if ( hModssleay32 )
		{
			string sOut = GetModuleInfo(hModssleay32);
			if (!sOut.empty())
				cout << sOut << endl;
		}

		if ( hModzlib )
		{
			string sOut = GetModuleInfo(hModzlib);
			if (!sOut.empty())
				cout << sOut << endl;
		}

		if ( hModlibxml2 )
		{
			string sOut = GetModuleInfo(hModlibxml2);
			if (!sOut.empty())
				cout << sOut << endl;
		}
		FreeConsole();
	}
	else 
	{
		CMainDialog dlgMain;

		dlgMain.hModlibeay32 = hModlibeay32;
		dlgMain.hModssleay32 = hModssleay32;
		dlgMain.hModzlib = hModzlib;
		dlgMain.hModlibxml2 = hModlibxml2;

		nRet = (int)dlgMain.DoModal();

	}

	return (int) nRet;
}



//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{

   hInst = hInstance; // Store instance handle in our global variable

   hModlibeay32 = LoadLibrary(L"libeay32");
   hModssleay32 = LoadLibrary(L"ssleay32");
   hModzlib = LoadLibrary(L"zlib1");
   hModlibxml2 = LoadLibrary(L"libxml2");

   return TRUE;
}

string GetModuleInfo(HMODULE hMod )
{
	string sRet;
	TCHAR   wszTmp[ 512 ] = {0};
	TCHAR wszBuf[MAX_PATH] = {0};

	if ( hMod )
	{
		DWORD dwRet = ::GetModuleFileName(hMod, wszBuf, MAX_PATH);
		if ( dwRet != 0)
		{
			CFileVersionInfo	fvi;
			if ( fvi.Open(wszBuf))
			{
				::wsprintf( wszTmp, 
					_T( "Loaded DLL '%s'  \t\tversion %d.%d.%d.%d" ),wszBuf, 
					fvi.GetFileVersionMajor(),
					fvi.GetFileVersionMinor(),
					fvi.GetFileVersionBuild(),
					fvi.GetFileVersionQFE() 
					);

				size_t wcharLen = wcslen(wszTmp);
				int charLen = MAX_PATH;
				char szTmp[MAX_PATH] = {0};
				if ( WideCharToMultiByte(CP_ACP, 0, wszTmp, (int)wcharLen, szTmp, charLen,NULL, NULL ) )
				{
					sRet = szTmp;
				}
				fvi.Close();
			}
			else 
			{
				::wsprintf( wszTmp, _T( "Loaded DLL '%s'  has no version infomation" ),wszBuf);

				size_t wcharLen = wcslen(wszTmp);
				int charLen = MAX_PATH;
				char szTmp[MAX_PATH] = {0};
				if ( WideCharToMultiByte(CP_ACP, 0, wszTmp, (int)wcharLen, szTmp, charLen,NULL, NULL ) )
				{
					sRet = szTmp;
				}
			}
		}
	}
	else 
	{
		sRet = "DLL not loaded";
	}

	return sRet;
}

// Message handler for about box.
//INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
//{
//	UNREFERENCED_PARAMETER(lParam);
//	switch (message)
//	{
//	case WM_INITDIALOG:
//		return (INT_PTR)TRUE;
//
//	case WM_COMMAND:
//		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
//		{
//			EndDialog(hDlg, LOWORD(wParam));
//			return (INT_PTR)TRUE;
//		}
//		break;
//	}
//	return (INT_PTR)FALSE;
//}
