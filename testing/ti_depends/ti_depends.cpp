// TiDepends.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "ti_depends.h"
#include "MainDlg.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

HMODULE hModlibeay32;
HMODULE hModssleay32;
HMODULE hModzlib;
HMODULE hModlibxml2;

// Forward declarations of functions included in this code module:
BOOL				InitInstance(HINSTANCE, int);
//INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{

	//TODO: add command line switches.
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}


	CMainDlg dlgMain;

	dlgMain.hModlibeay32 = hModlibeay32;
	dlgMain.hModssleay32 = hModssleay32;
	dlgMain.hModzlib = hModzlib;
	dlgMain.hModlibxml2 = hModlibxml2;

	int nRet = (int)dlgMain.DoModal();

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
