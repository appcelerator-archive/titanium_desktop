/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#include "../ui_module.h"
#define _WINSOCKAPI_
#include <cstdlib>
#include <sstream>
#include <windows.h>

using std::vector;
namespace ti
{
	UINT Win32UIBinding::nextItemId = NEXT_ITEM_ID_BEGIN;
	vector<HICON> Win32UIBinding::loadedICOs;
	vector<HBITMAP> Win32UIBinding::loadedBMPs;

	Win32UIBinding::Win32UIBinding(Module *uiModule, Host *host) :
		UIBinding(host),
		menu(0),
		contextMenu(0),
		iconPath("")
	{
		// Initialize common controls so that our Win32 native
		// components look swanky.
		INITCOMMONCONTROLSEX InitCtrlEx;
		InitCtrlEx.dwSize = sizeof(INITCOMMONCONTROLSEX);
		InitCtrlEx.dwICC = 0x00004000; //ICC_STANDARD_CLASSES;
		InitCommonControlsEx(&InitCtrlEx);
		
		// Set the cert path for Curl so that HTTPS works properly.
		// We are using _puetenv here since WebKit uses getenv internally 
		// which is incompatible with the Win32 envvar API.
		std::wstring pemPath = ::UTF8ToWide(FileUtils::Join(
			Host::GetInstance()->GetApplication()->runtime->path.c_str(),
			"rootcert.pem", 0));
		std::wstring var = L"CURL_CA_BUNDLE_PATH=" + pemPath;
		_wputenv(var.c_str());

		// Hook app:// and ti:// URL support to WebKit
		setNormalizeURLCallback(NormalizeURLCallback);
		setURLToFileURLCallback(URLToFileURLCallback);
		setCanPreprocessCallback(CanPreprocessURLCallback);
		setPreprocessCallback(PreprocessURLCallback);
		setProxyCallback(ProxyForURLCallback);

		std::string cookieJarFilename(FileUtils::Join(
			Host::GetInstance()->GetApplication()->GetDataPath().c_str(),
			"cookies.dat", 0));
		setCookieJarFilename(cookieJarFilename.c_str());
	}
	
	Win32UIBinding::~Win32UIBinding()
	{
	}

	AutoMenu Win32UIBinding::CreateMenu()
	{
		return new Win32Menu();
	}

	AutoMenuItem Win32UIBinding::CreateMenuItem()
	{
		return new Win32MenuItem(MenuItem::NORMAL);
	}

	AutoMenuItem Win32UIBinding::CreateSeparatorMenuItem()
	{
		return new Win32MenuItem(MenuItem::SEPARATOR);
	}

	AutoMenuItem Win32UIBinding::CreateCheckMenuItem()
	{
		return new Win32MenuItem(MenuItem::CHECK);
	}

	void Win32UIBinding::SetMenu(AutoMenu newMenu)
	{
		this->menu = newMenu.cast<Win32Menu>();
	}

	void Win32UIBinding::SetContextMenu(AutoMenu newMenu)
	{
		this->contextMenu = newMenu.cast<Win32Menu>();
	}

	void Win32UIBinding::SetIcon(std::string& iconPath)
	{
		if (!FileUtils::IsFile(iconPath))
		{
			this->iconPath = "";
		}
		else
		{
			this->iconPath = iconPath;
		}
	}

	AutoPtr<TrayItem> Win32UIBinding::AddTray(std::string& iconPath, KMethodRef cbSingleClick)
	{
		AutoPtr<TrayItem> trayItem = new Win32TrayItem(iconPath, cbSingleClick);
		return trayItem;
	}

	long Win32UIBinding::GetIdleTime()
	{
		LASTINPUTINFO lii;
		memset(&lii, 0, sizeof(lii));

		lii.cbSize = sizeof(lii);
		::GetLastInputInfo(&lii);

		DWORD currentTickCount = GetTickCount();
		long idleTicks = currentTickCount - lii.dwTime;

		return (int)idleTicks;
	}

	AutoMenu Win32UIBinding::GetMenu()
	{
		return this->menu;
	}

	AutoMenu Win32UIBinding::GetContextMenu()
	{
		return this->contextMenu;
	}

	std::string& Win32UIBinding::GetIcon()
	{
		return this->iconPath;
	}

	/*static*/
	HBITMAP Win32UIBinding::LoadImageAsBitmap(std::string& path, int sizeX, int sizeY)
	{
		UINT flags = LR_DEFAULTSIZE | LR_LOADFROMFILE |
			LR_LOADTRANSPARENT | LR_CREATEDIBSECTION;

		std::wstring widePath(::UTF8ToWide(path));
		const char* ext = path.c_str() + path.size() - 4;
		HBITMAP h = 0;
		if (_stricmp(ext, ".ico") == 0)
		{
			HICON hicon = (HICON) LoadImageW(NULL, widePath.c_str(), IMAGE_ICON,
				sizeX, sizeY, LR_LOADFROMFILE);
			h = Win32UIBinding::IconToBitmap(hicon, sizeX, sizeY);
			DestroyIcon(hicon);
		}
		else if (_stricmp(ext, ".bmp") == 0)
		{
			h = (HBITMAP) LoadImageW(
				NULL, widePath.c_str(), IMAGE_BITMAP, sizeX, sizeY, flags);
		}
		else if (_stricmp(ext, ".png") == 0)
		{
			h = LoadPNGAsBitmap(path, sizeX, sizeY);
		}
		else
		{
			throw ValueException::FromFormat("Unsupported image file: %s", path);
		}

		loadedBMPs.push_back(h);
		return h;
	}

	/*static*/
	HICON Win32UIBinding::LoadImageAsIcon(std::string& path, int sizeX, int sizeY)
	{
		UINT flags = LR_DEFAULTSIZE | LR_LOADFROMFILE |
			LR_LOADTRANSPARENT | LR_CREATEDIBSECTION;

		const char* ext = path.c_str() + path.size() - 4;
		std::wstring widePath(::UTF8ToWide(path));
		HICON h = 0;
		if (_stricmp(ext, ".ico") == 0)
		{
			h = (HICON) LoadImageW(0, widePath.c_str(),
				IMAGE_ICON, sizeX, sizeY, LR_LOADFROMFILE);
		}
		else if (_stricmp(ext, ".bmp") == 0)
		{
			HBITMAP bitmap = (HBITMAP) LoadImageW(0, widePath.c_str(),
				IMAGE_BITMAP, sizeX, sizeY, flags);
			h = Win32UIBinding::BitmapToIcon(bitmap, sizeX, sizeY);
			DeleteObject(bitmap);
		}
		else if (_stricmp(ext, ".png") == 0)
		{
			HBITMAP bitmap = LoadPNGAsBitmap(path, sizeX, sizeY);
			h = Win32UIBinding::BitmapToIcon(bitmap, sizeX, sizeY);
			DeleteObject(bitmap);
		}
		else
		{
			throw ValueException::FromFormat("Unsupported image file: %s", path);
		}

		loadedICOs.push_back(h);
		return (HICON) h;
	}
	
	/*static*/
	HICON Win32UIBinding::BitmapToIcon(HBITMAP bitmap, int sizeX, int sizeY)
	{
		if (!bitmap)
			return 0;

		HBITMAP bitmapMask = CreateCompatibleBitmap(GetDC(0), sizeX, sizeY);
		ICONINFO iconInfo = {0};
		iconInfo.fIcon = TRUE;
		iconInfo.hbmMask = bitmapMask;
		iconInfo.hbmColor = bitmap;
		HICON icon = CreateIconIndirect(&iconInfo);
		DeleteObject(bitmapMask);
		
		return icon;
	}

	/*static*/
	HBITMAP Win32UIBinding::IconToBitmap(HICON icon, int sizeX, int sizeY)
	{
		if (!icon)
			return 0;

		HDC hdc = GetDC(NULL);
		HDC hdcmem = CreateCompatibleDC(hdc);
		HBITMAP bitmap = CreateCompatibleBitmap(hdc, sizeX, sizeY);
		HBITMAP holdbitmap = (HBITMAP) SelectObject(hdcmem, bitmap);

		RECT rect = { 0, 0, sizeX, sizeY };
		SetBkColor(hdcmem, RGB(255, 255, 255));
		ExtTextOut(hdcmem, 0, 0, ETO_OPAQUE, &rect, NULL, 0, NULL);
		DrawIconEx(hdcmem, 0, 0, icon, sizeX, sizeY, 0, NULL, DI_NORMAL);

		SelectObject(hdc, holdbitmap);
		DeleteDC(hdcmem);

		return bitmap;
	}

	/*static*/
	HBITMAP Win32UIBinding::LoadPNGAsBitmap(std::string& path, int sizeX, int sizeY)
	{
		std::string systemPath(UTF8ToSystem(path));
		cairo_surface_t* pngSurface =
			cairo_image_surface_create_from_png(systemPath.c_str());
		
		cairo_t* pngcr = cairo_create(pngSurface);
		if (cairo_status(pngcr) != CAIRO_STATUS_SUCCESS)
			return 0;

		BITMAPINFO bitmapInfo;
		memset(&bitmapInfo, 0, sizeof(bitmapInfo));
		bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bitmapInfo.bmiHeader.biWidth = sizeX;
		bitmapInfo.bmiHeader.biHeight = -sizeY; // Bottom-up
		bitmapInfo.bmiHeader.biPlanes = 1;
		bitmapInfo.bmiHeader.biBitCount = 32;
		bitmapInfo.bmiHeader.biCompression = BI_RGB;
		bitmapInfo.bmiHeader.biSizeImage = 0;
		bitmapInfo.bmiHeader.biXPelsPerMeter = 1000;
		bitmapInfo.bmiHeader.biYPelsPerMeter = bitmapInfo.bmiHeader.biXPelsPerMeter;
		bitmapInfo.bmiHeader.biClrUsed = 0;
		bitmapInfo.bmiHeader.biClrImportant = 0;

		void* pixels = NULL;
		HDC hdc = ::GetDC(NULL);
		HBITMAP out = CreateDIBSection(hdc, &bitmapInfo,
			 DIB_RGB_COLORS, &pixels, NULL, 0);
		::ReleaseDC(NULL, hdc);

		BITMAP info;
		::GetObjectW(out, sizeof(info), &info);
		cairo_surface_t* outSurface = cairo_image_surface_create_for_data(
			(unsigned char*) pixels, CAIRO_FORMAT_ARGB32,
			sizeX, sizeY, info.bmWidthBytes);
		cairo_surface_t* scaledSurface = ScaleCairoSurface(pngSurface, sizeX, sizeY);

		cairo_t *cr = cairo_create(outSurface);

		cairo_save(cr);
		cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
		cairo_paint(cr);
		cairo_restore(cr);

		cairo_set_source_surface(cr, scaledSurface, 0, 0);
		cairo_rectangle(cr, 0, 0, sizeX, sizeY);
		cairo_fill(cr);

		cairo_destroy(cr);
		cairo_surface_destroy(outSurface);
		cairo_surface_destroy(scaledSurface);
		cairo_destroy(pngcr);
		cairo_surface_destroy(pngSurface);

		return out;
	}

	cairo_surface_t* Win32UIBinding::ScaleCairoSurface(
		cairo_surface_t* oldSurface, int newWidth, int newHeight)
	{
		cairo_matrix_t scaleMatrix;
		cairo_matrix_init_scale(&scaleMatrix,
			(double) cairo_image_surface_get_width(oldSurface) / (double) newWidth,
			(double) cairo_image_surface_get_height(oldSurface) / (double) newHeight);

		cairo_pattern_t* surfacePattern = cairo_pattern_create_for_surface(oldSurface);
		cairo_pattern_set_matrix(surfacePattern, &scaleMatrix);
		cairo_pattern_set_filter(surfacePattern, CAIRO_FILTER_BEST);

		cairo_surface_t* newSurface = cairo_surface_create_similar(
			oldSurface, CAIRO_CONTENT_COLOR_ALPHA, newWidth, newHeight);
		cairo_t* cr = cairo_create(newSurface);
		cairo_set_source(cr, surfacePattern);

		/* To avoid getting the edge pixels blended with 0 alpha, which would 
		 * occur with the default EXTEND_NONE. Use EXTEND_PAD for 1.2 or newer (2) */
		cairo_pattern_set_extend(cairo_get_source(cr), CAIRO_EXTEND_REFLECT);

		 /* Replace the destination with the source instead of overlaying */
		cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);

		/* Do the actual drawing */
		cairo_paint(cr);
		cairo_destroy(cr);

		return newSurface;
	 }


	/*static*/
	void Win32UIBinding::ReleaseImage(HANDLE handle)
	{
		vector<HICON>::iterator i = loadedICOs.begin();
		while (i != loadedICOs.end()) {
			if (*i == handle) {
				DestroyIcon(*i);
				return;
			} else {
				i++;
			}
		}

		vector<HBITMAP>::iterator i2 = loadedBMPs.begin();
		while (i2 != loadedBMPs.end()) {
			if (*i2 == handle) {
				::DeleteObject(*i2);
				return;
			} else {
				i2++;
			}
		}

	}

	/*static*/
	void Win32UIBinding::SetProxyForURL(std::string& url)
	{
		SharedPtr<Proxy> proxy(ProxyConfig::GetProxyForURL(url));
		if (!proxy.isNull())
		{
			std::stringstream proxyEnv;
			if (proxy->type == HTTP)
				proxyEnv << "http_proxy=http://";
			else if (proxy->type = HTTPS)
				proxyEnv << "HTTPS_PROXY=https://";

			if (!proxy->username.empty() || !proxy->password.empty())
				proxyEnv << proxy->username << ":" << proxy->password << "@";

			proxyEnv << proxy->host;

			if (proxy->port != 0)
				proxyEnv << ":" << proxy->port;

			std::wstring proxyEnvStr(::UTF8ToWide(proxyEnv.str()));
			_wputenv(proxyEnvStr.c_str());
		}
	}

	/*static*/
	void Win32UIBinding::ErrorDialog(std::string msg)
	{
		std::wstring msgW = ::UTF8ToWide(msg);
		MessageBox(NULL, msgW.c_str(), L"Application Error", 
			MB_OK | MB_ICONERROR | MB_SYSTEMMODAL);
		UIBinding::ErrorDialog(msg);
	}

}
