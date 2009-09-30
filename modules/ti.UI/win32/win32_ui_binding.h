/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#ifndef _WIN32_UI_BINDING_H_
#define _WIN32_UI_BINDING_H_
#include "../ui_module.h"

#define WEB_INSPECTOR_MENU_ITEM_ID 7500
#define NEXT_ITEM_ID_BEGIN 7501

namespace ti
{
	class Win32UIBinding : public UIBinding
	{

		public:
		Win32UIBinding(Module* uiModule, Host *host);
		~Win32UIBinding();

		AutoUserWindow CreateWindow(WindowConfig*, AutoUserWindow& parent);
		void ErrorDialog(std::string);

		AutoMenu CreateMenu();
		AutoMenuItem CreateMenuItem();
		AutoMenuItem CreateSeparatorMenuItem();
		AutoMenuItem CreateCheckMenuItem();
		void SetMenu(AutoMenu);
		AutoTrayItem AddTray(std::string& icon_path, SharedKMethod cbSingleClick);
		void SetContextMenu(AutoMenu);
		void SetIcon(std::string& iconPath);
		long GetIdleTime();

		AutoMenu GetMenu();
		AutoMenu GetContextMenu();
		std::string& GetIcon();
		static UINT nextItemId;

		static HICON LoadImageAsIcon(std::string& path, int sizeX=0, int sizeY=0);
		static HBITMAP LoadImageAsBitmap(std::string& path, int sizeX=0, int sizeY=0);
		static HICON BitmapToIcon(HBITMAP bitmap, int sizeX, int sizeY);
		static HBITMAP IconToBitmap(HICON icon, int sizeX, int sizeY);
		static HBITMAP LoadPNGAsBitmap(std::string& path, int sizeX, int sizeY);
		static cairo_surface_t* ScaleCairoSurface(
			cairo_surface_t *oldSurface, int newWidth, int newHeight);
		static void ReleaseImage(HANDLE);
		static void SetProxyForURL(std::string& url);

		private:
		AutoPtr<Win32Menu> menu;
		AutoPtr<Win32Menu> contextMenu;
		std::string iconPath;

		// Cookie and handle for WebKit Activation Context
		ULONG_PTR lpCookie; 
		HANDLE pActCtx;
		static std::vector<HICON> loadedICOs;
		static std::vector<HBITMAP> loadedBMPs;
	};
}

#endif
