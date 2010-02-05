/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */
#include "win32_desktop.h"

#include <windows.h>
#include <commdlg.h>
#include <shellapi.h>
#include <shlobj.h>
#include <string>

using std::wstring;

namespace ti
{
	Win32Desktop::Win32Desktop()
	{
	}

	Win32Desktop::~Win32Desktop()
	{
	}

	bool Win32Desktop::OpenApplication(std::string &name)
	{
		// this can actually open applications or documents (wordpad, notepad, file-test.txt, etc.)
		wstring wideDir;
		wstring wideName(::UTF8ToWide(name));
		if (FileUtils::IsFile(wideName))
		{
			// start with the current working directory as the directory of the program
			wideDir.assign(::UTF8ToWide(FileUtils::GetDirectory(name)));
		}
		
		const wchar_t* dir = wideDir.size() > 0 ? wideDir.c_str() : NULL;
		long response = (long)ShellExecuteW(NULL, L"open",
			wideName.c_str(), NULL, dir,
			SW_SHOWNORMAL);
		
		return (response > 32);
	}

	bool Win32Desktop::OpenURL(std::string &url)
	{
		wstring wideURL(::UTF8ToWide(url));
		long response = (long)ShellExecuteW(NULL, L"open",
			wideURL.c_str(), NULL, NULL, SW_SHOWNORMAL);
		
		return (response > 32);
	}

	void Win32Desktop::TakeScreenshot(std::string &screenshotFile)
	{
		//ensure filename ends in bmp
		// do we have a file extension?
		if ( screenshotFile.rfind(".") == std::string::npos )
		{
			// only append the .bmp extension if we don't detect a file extension.
			screenshotFile.append(".bmp");
		}


		HWND desktop = GetDesktopWindow();
		RECT desktopRect;
		GetWindowRect(desktop, &desktopRect);

		int x = 0;
		int y = 0;
		int width = desktopRect.right;
		int height = desktopRect.bottom;

		// get a DC compat. w/ the screen
		HDC hdc = CreateCompatibleDC(0);
		HBITMAP screenshotBitmap = CreateCompatibleBitmap(GetDC(0), width, height);
		HBITMAP originalBmp = (HBITMAP) SelectObject(hdc, screenshotBitmap);

		// copy from the screen to my bitmap
		BitBlt(hdc, 0, 0, width, height, GetDC(0), x, y, SRCCOPY);

		wchar_t tmpDir[MAX_PATH];
		wchar_t tmpFile[MAX_PATH];
		GetTempPathW(sizeof(tmpDir), tmpDir);
		GetTempFileNameW(tmpDir, L"ti_", 0, tmpFile);

		std::wstring path(::UTF8ToWide(screenshotFile));
		bool success = SaveBMPFile(path.c_str(), screenshotBitmap, hdc, width, height);

		SelectObject(hdc, originalBmp);
		DeleteDC(hdc);
		DeleteObject(screenshotBitmap);

		if (!success)
			throw ValueException::FromString("Failed to save screenshot to file");
	}

	bool Win32Desktop::SaveBMPFile(const wchar_t* filename, HBITMAP bitmap, HDC bitmapDC, int width, int height) {
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
}
