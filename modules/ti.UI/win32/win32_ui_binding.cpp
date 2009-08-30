/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#include "../ui_module.h"
#define _WINSOCKAPI_
#include <cstdlib>

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
		// which is incompatible with the  Win32 envvar API.
		std::string pemPath = FileUtils::Join(
			uiModule->GetPath().c_str(), "cacert.pem", NULL);
		std::string var = "CURL_CA_BUNDLE_PATH=" + pemPath;
		_putenv(var.c_str());

		// Hook app:// and ti:// URL support to WebKit
		setNormalizeURLCallback(NormalizeURLCallback);
		setURLToFileURLCallback(URLToFileURLCallback);
	}
	
	Win32UIBinding::~Win32UIBinding()
	{
	}

	AutoUserWindow Win32UIBinding::CreateWindow(
		WindowConfig* config,
		AutoUserWindow& parent)
	{
		UserWindow* w = new Win32UserWindow(config, parent);
		return w->GetAutoPtr();
	}

	void Win32UIBinding::ErrorDialog(std::string msg)
	{
		std::wstring msgW = UTF8ToWide(msg);
		MessageBox(NULL, msgW.c_str(), L"Application Error", MB_OK|MB_ICONERROR|MB_SYSTEMMODAL);
		UIBinding::ErrorDialog(msg);
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

	AutoPtr<TrayItem> Win32UIBinding::AddTray(std::string& iconPath, SharedKMethod cbSingleClick)
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
		std::string lcpath = path;
		std::transform(lcpath.begin(), lcpath.end(), lcpath.begin(), tolower);
		std::string ext = lcpath.substr(lcpath.size() - 4, 4);
		UINT flags = LR_DEFAULTSIZE | LR_LOADFROMFILE |
			LR_LOADTRANSPARENT | LR_CREATEDIBSECTION;


		HBITMAP h = 0;
		if (ext ==  ".ico") {
			HICON hicon = (HICON) LoadImageA(
				NULL, path.c_str(), IMAGE_ICON, sizeX, sizeY, LR_LOADFROMFILE);
			h = Win32UIBinding::IconToBitmap(hicon, sizeX, sizeY);
			DestroyIcon(hicon);

		} else if (ext == ".bmp") {
			h = (HBITMAP) LoadImageA(
				NULL, path.c_str(), IMAGE_BITMAP, sizeX, sizeY, flags);

		} else if (ext == ".png") {
			h = LoadPNGAsBitmap(path, sizeX, sizeY);
		}

		loadedBMPs.push_back(h);
		return h;
	}

	/*static*/
	HICON Win32UIBinding::LoadImageAsIcon(std::string& path, int sizeX, int sizeY)
	{
		std::string lcpath = path;
		std::transform(lcpath.begin(), lcpath.end(), lcpath.begin(), tolower);
		std::string ext = lcpath.substr(lcpath.size() - 4, 4);
		UINT flags = LR_DEFAULTSIZE | LR_LOADFROMFILE |
			LR_LOADTRANSPARENT | LR_CREATEDIBSECTION;

		HICON h = 0;
		if (ext ==  ".ico") {
			h = (HICON) LoadImageA(
				NULL, path.c_str(), IMAGE_ICON, sizeX, sizeY, LR_LOADFROMFILE);

		} else if (ext == ".bmp") {
			HBITMAP bitmap = (HBITMAP) LoadImageA(
				NULL, path.c_str(), IMAGE_BITMAP, sizeX, sizeY, flags);
			h = Win32UIBinding::BitmapToIcon(bitmap, sizeX, sizeY);
			DeleteObject(bitmap);

		} else if (ext == ".png") {
			HBITMAP bitmap = LoadPNGAsBitmap(path, sizeX, sizeY);
			h = Win32UIBinding::BitmapToIcon(bitmap, sizeX, sizeY);
			DeleteObject(bitmap);
		}

		loadedICOs.push_back(h);
		return (HICON) h;
	}
	
	/*static*/
	HICON Win32UIBinding::BitmapToIcon(HBITMAP bitmap, int sizeX, int sizeY)
	{
		if (!bitmap)
			return 0;

		if (sizeX == 0)
			sizeX = GetSystemMetrics(SM_CYSMICON);
		if (sizeY == 0)
			sizeY = GetSystemMetrics(SM_CYSMICON);

		HBITMAP bitmapMask = CreateCompatibleBitmap(GetDC(NULL), sizeX, sizeY);
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

		if (sizeX == 0)
			sizeX = GetSystemMetrics(SM_CYSMICON);
		if (sizeY == 0)
			sizeY = GetSystemMetrics(SM_CYSMICON);

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
		if (sizeX == 0)
			sizeX = GetSystemMetrics(SM_CYSMICON);
		if (sizeY == 0)
			sizeY = GetSystemMetrics(SM_CYSMICON);

		cairo_surface_t* insurface = cairo_image_surface_create_from_png(path.c_str());
		cairo_t* pngcr = cairo_create(insurface);
		if (cairo_status(pngcr) != CAIRO_STATUS_SUCCESS) {
			return 0;
		}

		cairo_surface_t* pngsurface = ScaleCairoSurface(insurface, sizeX, sizeY);

		HDC hdc = GetDC(NULL);
		HDC hdcmem = CreateCompatibleDC(hdc);
		HBITMAP bitmap = CreateCompatibleBitmap(hdc, sizeX, sizeY);
		HBITMAP holdbitmap = (HBITMAP) SelectObject(hdcmem, bitmap);

		cairo_surface_t* w32surface = cairo_win32_surface_create(hdcmem);
		cairo_t *cr = cairo_create(w32surface);
		cairo_set_source_surface(cr, pngsurface, 0, 0);
		cairo_rectangle(cr, 0, 0, sizeX, sizeY);
		cairo_fill(cr);

		cairo_destroy(cr);
		cairo_surface_destroy(w32surface);
		cairo_surface_destroy(pngsurface);
		cairo_destroy(pngcr);
		cairo_surface_destroy(insurface);
		
		SelectObject(hdc, holdbitmap);
		DeleteDC(hdcmem);
		return bitmap;
	}

	cairo_surface_t* Win32UIBinding::ScaleCairoSurface(
		cairo_surface_t *oldSurface, int newWidth, int newHeight)
	{
		int oldWidth = cairo_image_surface_get_width(oldSurface);
		int oldHeight = cairo_image_surface_get_height(oldSurface);

		cairo_surface_t *newSurface = cairo_surface_create_similar(
			oldSurface, CAIRO_CONTENT_COLOR_ALPHA, newWidth, newHeight);
		cairo_t *cr = cairo_create(newSurface);
		
		/* Scale *before* setting the source surface (1) */
		cairo_scale(cr, (double) newWidth / oldWidth, (double) newHeight / oldHeight);
		cairo_set_source_surface(cr, oldSurface, 0, 0);
		
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
		SharedPtr<Proxy> proxy = ProxyConfig::GetProxyForURL(url);
		if (!proxy.isNull())
		{
			printf("setting proxy\n");
			// We make a copy of the URI here so that we can  modify it 
			// without worrying about changing a potentially global one.
			Poco::URI proxyURI(*proxy->info);
			Poco::URI uri = Poco::URI(url);
			proxyURI.setScheme(uri.getScheme());

			std::string proxyEnv;
			if (proxyURI.getScheme() == "http")
			{
				printf("setting http_proxy=%s\n", proxyURI.toString().c_str());
				proxyEnv.append("http_proxy=");
			}
			else if (proxyURI.getScheme() == "https")
			{
				proxyEnv.append("HTTPS_PROXY=");
			}
			proxyEnv.append(proxyURI.toString());
			_putenv(proxyEnv.c_str());
		}
	}

}
