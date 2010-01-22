/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2010 Appcelerator, Inc. All Rights Reserved.
 */
#include <kroll/kroll.h>
#include <Poco/Environment.h>
#include <windows.h>
#include <commdlg.h>
#include <shellapi.h>
#include <shlobj.h>
#include "platform_binding.h"

namespace ti
{

static std::string GetFailureReason(long code)
{
	switch (code)
	{
		case 0:
		case SE_ERR_OOM:
			return "out of memory";
		case SE_ERR_FNF:
			return  "file not found";
		case SE_ERR_PNF:
			return "path not found";
		case ERROR_BAD_FORMAT:
			return "bad format error";
		case SE_ERR_ACCESSDENIED:
			return "access denied";
		case SE_ERR_ASSOCINCOMPLETE:
			return "association incomplete";
		case SE_ERR_DDEBUSY:
			return "DDE busy";
		case SE_ERR_DDEFAIL:
			return "DDE failed";
		case SE_ERR_DDETIMEOUT:
			return "DDE timed out";
		case SE_ERR_DLLNOTFOUND:
			return "DLL not found";
		case SE_ERR_NOASSOC:
			return "no application associated";
		case SE_ERR_SHARE:
			return "Sharing violation";
	}
	return "unknown";
}

static bool SaveBMPFile(const wchar_t* filename, HBITMAP bitmap, HDC bitmapDC, 
	int width, int height)
{
	bool Success = false;
	HDC SurfDC = NULL;						// GDI-compatible device context for the surface
	HBITMAP OffscrBmp = NULL;				// bitmap that is converted to a DIB
	HDC OffscrDC = NULL;					// offscreen DC that we can select OffscrBmp into
	LPBITMAPINFO lpbi = NULL;				// bitmap format info; used by GetDIBits
	LPVOID lpvBits = NULL;					// pointer to bitmap bits array
	HANDLE BmpFile = INVALID_HANDLE_VALUE;	// destination .bmp file
	BITMAPFILEHEADER bmfh;					// .bmp file header

	// We need an HBITMAP to convert it to a DIB:
	if ((OffscrBmp = CreateCompatibleBitmap(bitmapDC, width, height)) == NULL)
	{
		return false;
	}

	// The bitmap is empty, so let's copy the contents of the surface to it.
	// For that we need to select it into a device context. We create one.
	if ((OffscrDC = CreateCompatibleDC(bitmapDC)) == NULL)
	{
		return false;
	}

	// Select OffscrBmp into OffscrDC:
	HBITMAP OldBmp = (HBITMAP)SelectObject(OffscrDC, OffscrBmp);

	// Now we can copy the contents of the surface to the offscreen bitmap:
	BitBlt(OffscrDC, 0, 0, width, height, bitmapDC, 0, 0, SRCCOPY);

	// GetDIBits requires format info about the bitmap. We can have GetDIBits
	// fill a structure with that info if we pass a NULL pointer for lpvBits:
	// Reserve memory for bitmap info (BITMAPINFOHEADER + largest possible
	// palette):
	if ((lpbi = (LPBITMAPINFO)(new char[sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD)])) == NULL)
	{
		return false;
	}


	ZeroMemory(&lpbi->bmiHeader, sizeof(BITMAPINFOHEADER));
	lpbi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);

	// Get info but first de-select OffscrBmp because GetDIBits requires it:
	SelectObject(OffscrDC, OldBmp);
	if (!GetDIBits(OffscrDC, OffscrBmp, 0, height, NULL, lpbi, DIB_RGB_COLORS))
	{
		return false;
	}

	// Reserve memory for bitmap bits:
	if ((lpvBits = new char[lpbi->bmiHeader.biSizeImage]) == NULL)
	{
		return false;
	}

	// Have GetDIBits convert OffscrBmp to a DIB (device-independent bitmap):
	if (!GetDIBits(OffscrDC, OffscrBmp, 0, height, lpvBits, lpbi, DIB_RGB_COLORS))
	{
		return false;
	}

	// Create a file to save the DIB to:
	if ((BmpFile = ::CreateFileW(filename, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL,NULL)) == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	DWORD Written;    // number of bytes written by WriteFile

	// Write a file header to the file:
	bmfh.bfType = 19778;        // 'BM'
	// bmfh.bfSize = ???        // we'll write that later
	bmfh.bfReserved1 = bmfh.bfReserved2 = 0;
	// bmfh.bfOffBits = ???     // we'll write that later
	if (!WriteFile(BmpFile, &bmfh, sizeof(bmfh), &Written, NULL))
	{
		return false;
	}

	if (Written < sizeof(bmfh))
	{
		return false;
	}

	// Write BITMAPINFOHEADER to the file:
	if (!WriteFile(BmpFile, &lpbi->bmiHeader, sizeof(BITMAPINFOHEADER), &Written, NULL))
	{
		return false;
	}

	if (Written < sizeof(BITMAPINFOHEADER))
	{
		return false;
	}

	// Calculate size of palette:
	int PalEntries;
	// 16-bit or 32-bit bitmaps require bit masks:
	if (lpbi->bmiHeader.biCompression == BI_BITFIELDS)
	{
		PalEntries = 3;
	}
	else
	{
		// bitmap is palettized?
		PalEntries = (lpbi->bmiHeader.biBitCount <= 8) ?
		// 2^biBitCount palette entries max.:
		(int)(1 << lpbi->bmiHeader.biBitCount)
		// bitmap is TrueColor -> no palette:
		: 0;
	}

	// If biClrUsed use only biClrUsed palette entries:
	if(lpbi->bmiHeader.biClrUsed)
	{
		PalEntries = lpbi->bmiHeader.biClrUsed;
	}

	// Write palette to the file:
	if(PalEntries){
		if (!WriteFile(BmpFile, &lpbi->bmiColors, PalEntries * sizeof(RGBQUAD), &Written, NULL))
		{
			return false;
		}

		if (Written < PalEntries * sizeof(RGBQUAD))
		{
			return false;
		}
	}

	// The current position in the file (at the beginning of the bitmap bits)
	// will be saved to the BITMAPFILEHEADER:
	//bmfh.bfOffBits = GetFilePointer(BmpFile);
	bmfh.bfOffBits = SetFilePointer(BmpFile, 0, 0, FILE_CURRENT);

	// Write bitmap bits to the file:
	if (!WriteFile(BmpFile, lpvBits, lpbi->bmiHeader.biSizeImage, &Written, NULL))
	{
		return false;
	}

	if (Written < lpbi->bmiHeader.biSizeImage)
	{
		return false;
	}

	// The current pos. in the file is the final file size and will be saved:
	//bmfh.bfSize = GetFilePointer(BmpFile);
	bmfh.bfSize = SetFilePointer(BmpFile, 0, 0, FILE_CURRENT);

	// We have all the info for the file header. Save the updated version:
	SetFilePointer(BmpFile, 0, 0, FILE_BEGIN);
	if (!WriteFile(BmpFile, &bmfh, sizeof(bmfh), &Written, NULL))
	{
		return false;
	}

	if (Written < sizeof(bmfh))
	{
		return false;
	}

	return true;
}

int PlatformBinding::GetProcessorCountImpl()
{
	SYSTEM_INFO systemInfo;
	ZeroMemory(&systemInfo, sizeof(SYSTEM_INFO));
	GetSystemInfo(&systemInfo) ;
	return systemInfo.dwNumberOfProcessors;
}

std::string PlatformBinding::GetVersionImpl()
{
	return Poco::Environment::osVersion();
}

bool PlatformBinding::OpenApplicationImpl(const std::string& name)
{
	std::wstring wideName(::UTF8ToWide(name));
	long response = reinterpret_cast<long>(ShellExecuteW(
		0, L"open", wideName.c_str(), 0, 0, SW_SHOWNORMAL));

	if (response <= 32)
	{
		std::string reason(GetFailureReason(response));
		throw ValueException::FromFormat("openApplication failed: %s", reason.c_str());
		return false;
	}

	return true;
}

bool PlatformBinding::OpenURLImpl(const std::string& name)
{
	return this->OpenApplicationImpl(name);
}

void PlatformBinding::TakeScreenshotImpl(const std::string& targetFile)
{
	// Ensure filename ends in .bmp.
	// TODO: This seems broken.
	std::string screenshotFile = targetFile;
	if (screenshotFile.rfind(".") == std::string::npos)
		screenshotFile.append(".bmp");

	HWND desktop = GetDesktopWindow();
	RECT desktopRect;
	GetWindowRect(desktop, &desktopRect);

	int x = 0;
	int y = 0;
	int width = desktopRect.right;
	int height = desktopRect.bottom;

	// get a DC compat. w/ the screen
	HDC hdc = CreateCompatibleDC(0);

	// make a bmp in memory to store the capture in
	HBITMAP hBmp = CreateCompatibleBitmap(GetDC(0), width, height);

	// join em up
	SelectObject(hdc, hBmp);

	// copy from the screen to my bitmap
	BitBlt(hdc, 0, 0, width, height, GetDC(0), x, y, SRCCOPY);

	wchar_t tmpDir[MAX_PATH];
	wchar_t tmpFile[MAX_PATH];
	GetTempPathW(sizeof(tmpDir), tmpDir);
	GetTempFileNameW(tmpDir, L"ti_", 0, tmpFile);

	if (SaveBMPFile(tmpFile, hBmp, hdc, width, height))
	{
		std::wstring wideScreenshotFile(::UTF8ToWide(screenshotFile));
		CopyFileW(tmpFile, wideScreenshotFile.c_str(), FALSE);
	}

	// free the bitmap memory
	DeleteObject(hBmp);
}

}
