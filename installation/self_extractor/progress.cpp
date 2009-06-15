/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

/**
 CREDITS:
 The actual core source code for zipping and unzipping comes from
 www.info-zip.org and www.gzip.org/zlib, by Jean-Loup Gailly and Mark
 Adler and others, and is freely available at the respective
 websites. unzip code repackaging by Lucian Wischik, June 2004 - July 2005
**/

#include <windows.h>
#include <commctrl.h>
#include <tchar.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <sstream>
#include "zip.h"
#include "unzip.h"
#include "updateicon.h"

BOOL CALLBACK UnzipDialogProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
BOOL UnzipWithProgress(const TCHAR *zipfn, const TCHAR *dest, HWND hprog);
void PumpMessages();
bool abort_p=false;
std::string exeName = "";

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE,LPSTR,int)
//int main (int argc, char **argv)
{
	InitCommonControls();
#pragma pack(push,1)
	struct TDlgItemTemplate {DWORD s,ex; short x,y,cx,cy; WORD id;};
	struct TDlgTemplate {DWORD s,ex; WORD cdit; short x,y,cx,cy;};
	struct TDlgItem1 {TDlgItemTemplate dli; WCHAR cls[7],tit[7]; WORD cdat;};
	struct TDlgItem2 {TDlgItemTemplate dli; WCHAR cls[18],tit[1]; WORD cdat;};
	struct TDlgData  {TDlgTemplate dlt; WORD m,c; WCHAR t[8]; WORD pt; WCHAR f[14]; TDlgItem1 i1; TDlgItem2 i2;};
	TDlgData dtp={{DS_MODALFRAME|DS_3DLOOK|DS_SETFONT|DS_CENTER|WS_POPUP|WS_CAPTION|WS_SYSMENU|WS_VISIBLE,0,2, 0,0,278,54},
                0,0,L"Zipping",8,L"MS Sans Serif",
                {{BS_PUSHBUTTON|WS_CHILD|WS_VISIBLE,0,113,32,50,14,IDCANCEL},L"BUTTON",L"Cancel",0},
                {{WS_CHILD|WS_VISIBLE,0,7,12,264,10,1},L"msctls_progress32",L"",0} };
#pragma pack(pop)
	return (int)DialogBoxIndirect(::GetModuleHandle(NULL),(DLGTEMPLATE*)&dtp,0,UnzipDialogProc);
}

std::string GetDirectory(std::string &path)
{
	size_t i = path.rfind("/");
	if (i != std::string::npos)
	{
		return path.substr(0,i);
	}
	else
	{
		i = path.rfind("\\");
		if (i != std::string::npos)
		{
			return path.substr(0,i);
		}
	}
	return ".\\";
}

bool FileExists(std::string &path)
{
	DWORD fileAttr = GetFileAttributes(path.c_str());
	if (0xFFFFFFFF == fileAttr) return false;
	return true;
}

void ReplaceAppIco(std::string dest, std::string exename)
{
	std::string ico =  dest + "\\Resources\\app.ico";
	std::string exe = dest + "\\" + exename;
	
	UpdateExeIcon(exe, ico);
}

BOOL CALLBACK UnzipDialogProc(HWND hwnd,UINT msg,WPARAM,LPARAM)
{
	switch(msg)
	{
		case WM_INITDIALOG:
		{
			SetWindowText(hwnd,"Extracting, one moment ...");
			PostMessage(hwnd,WM_USER,0,0);
			return TRUE;
		}
		case WM_COMMAND:
		{
			abort_p=true;
			return TRUE;
		}
		case WM_USER:
		{
			TCHAR filename[MAX_PATH];
			TCHAR tempdir[MAX_PATH];
			DWORD size;
			size = GetTempPath(MAX_PATH,(TCHAR*)&tempdir);
			tempdir[size]='\0';
			srand(GetTickCount());
			int r = rand();
			sprintf_s(tempdir,"%s\\ti%d",tempdir,r);
			size = GetModuleFileName(GetModuleHandle(NULL),filename,MAX_PATH);
			filename[size]='\0';
			if (!CreateDirectory(tempdir,NULL))
			{
				char msg[MAX_PATH];
				sprintf_s(msg,"Error creating temp directory: %s, error code: %d",tempdir,GetLastError());
				MessageBox(NULL,msg,"Application Error",MB_OK|MB_SYSTEMMODAL|MB_ICONERROR);
			}
			else
			{
				BOOL OK = UnzipWithProgress(filename,tempdir,GetDlgItem(hwnd,1));
				EndDialog(hwnd,IDOK);
				if (!OK)
				{
					MessageBox(NULL,"Application packaging error. No application archive was found.","Application Error",MB_OK|MB_SYSTEMMODAL|MB_ICONERROR);
				}
				else
				{
					//run the bundled file
					std::string fn = exeName;
					size_t i = fn.rfind("\\");
					if (i!=std::string::npos)
					{
						fn = fn.substr(i+1);
					}
					std::ostringstream ostr;
					ostr << tempdir << "\\" << fn;
					ostr << " --force-install";
					
					STARTUPINFO si;
					PROCESS_INFORMATION pi;
					ZeroMemory( &si, sizeof(si) );
					si.cb = sizeof(si);
					ZeroMemory( &pi, sizeof(pi) );
					if (!CreateProcess( NULL,   // No module name (use command line)
							(char*)ostr.str().c_str(), // Command line
							NULL,           // Process handle not inheritable
							NULL,           // Thread handle not inheritable
							FALSE,          // Set handle inheritance to FALSE
							0,              // No creation flags
							NULL,           // Use parent's environment block
							(char*)tempdir,	// Use parent's starting directory
							&si,            // Pointer to STARTUPINFO structure
							&pi )           // Pointer to PROCESS_INFORMATION structure
					)
					{
						LPVOID lpMsgBuf;
						LPVOID lpDisplayBuf;
						DWORD dw = GetLastError();

						FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER
							| FORMAT_MESSAGE_FROM_SYSTEM
							| FORMAT_MESSAGE_IGNORE_INSERTS, NULL, dw,
							MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
							(LPTSTR) &lpMsgBuf, 0, NULL );

						char errorMessage[512];
						sprintf(errorMessage, "Application error running process: %s %d: %s", ostr.str().c_str(), dw, lpMsgBuf);
						MessageBox(NULL,errorMessage, "Application error running process.",MB_OK|MB_SYSTEMMODAL|MB_ICONERROR);
					}
					else
					{
						// Wait until child process exits.
						WaitForSingleObject( pi.hProcess, INFINITE );

						// Close process and thread handles.
						CloseHandle( pi.hProcess );
						CloseHandle( pi.hThread );
					}
				}
			}
			RemoveDirectory(tempdir);
			return TRUE;
		}
	}
	return FALSE;
}

BOOL UnzipWithProgress(const TCHAR *zipfn, const TCHAR *dest, HWND hprog)
{
	HZIP hz = OpenZip(zipfn,0);
	ZIPENTRY ze;
	GetZipItem(hz,-1,&ze);
	int numentries=ze.index;
	// first we retrieve the total size of all zip items
	DWORD tot=0; for (int i=0; i<numentries; i++)
	{
		GetZipItem(hz,i,&ze);
		tot+=ze.unc_size;
	}
	DWORD countall=0; // this is our progress so far
	for (int i=0; i<numentries && !abort_p; i++)
	{
		GetZipItem(hz,i,&ze);
		
		// We'll unzip each file bit by bit, to a file on disk
		char fn[1024];
		wsprintf(fn,"%s\\%s",dest,ze.name);
		std::string dir = GetDirectory(std::string(fn));
		EnsureDirectory(0,dir.c_str());
		HANDLE hf = CreateFile(fn,GENERIC_WRITE,0,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
		char buf[16384]; // Each chunk will be 16k big. After each chunk, we show progress
		DWORD countfile=0;
		for (ZRESULT zr=ZR_MORE; zr==ZR_MORE && !abort_p; ) // nb. the global "abort_p" flag is set by the user clicking the Cancel button.
		{
			zr=UnzipItem(hz,i,buf,16384);
			unsigned long bufsize=16384;
			if (zr==ZR_OK) bufsize=ze.unc_size-countfile; // bufsize is how much we got this time
			DWORD writ;
			WriteFile(hf,buf,bufsize,&writ,0);
			countfile+=bufsize; // countfile counts how much of this file we've unzipped so far
			countall+=bufsize; // countall counts how much total we've unzipped so far
			// Now show progress, and let Windows catch up...
			int i = (int)(100.0*((double)countall)/((double)tot));
			SendMessage(hprog,PBM_SETPOS,i,0);
			PumpMessages();
		}
		CloseHandle(hf);
		std::string fileName = ze.name;
		
		if (fileName.find(".exe") != std::string::npos
			&& fileName.find("installer.exe") == std::string::npos
			&& fileName.find("/") == std::string::npos) {
			
			exeName = fileName;
		}
		else if (fileName == "Resources/app.ico") {
			ReplaceAppIco(dest, exeName);
			ReplaceAppIco(dest, "installer\\Installer.exe");
		}
		
		if (abort_p) DeleteFile(fn);
	}
	CloseZip(hz);

	return numentries > 0;
}

void PumpMessages()
{
	for (MSG msg;;)
	{
		BOOL res=PeekMessage(&msg,0,0,0,PM_REMOVE);
		if (!res || msg.message==WM_QUIT) return;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}
