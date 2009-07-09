/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#ifndef _WIN32_UI_BINDING_H_
#define _WIN32_UI_BINDING_H_
#include "../ui_module.h"

#define TI_TRAY_CLICKED 7499
#define WEB_INSPECTOR_MENU_ITEM_ID 7500
#define NEXT_ITEM_ID_BEGIN 7501

namespace ti
{
	class Win32UIBinding : public UIBinding
	{

		public:
		Win32UIBinding(Module* uiModule, Host *host);
		~Win32UIBinding();
		static bool IsWindowsXP();

		SharedUserWindow CreateWindow(WindowConfig*, SharedUserWindow& parent);
		void ErrorDialog(std::string);

		SharedMenu CreateMenu();
		SharedMenuItem CreateMenuItem();
		SharedMenuItem CreateSeparatorMenuItem();
		SharedMenuItem CreateCheckMenuItem();
		void SetMenu(SharedMenu);
		SharedTrayItem AddTray(std::string& icon_path, SharedKMethod cb);
		void SetContextMenu(SharedMenu);
		void SetIcon(std::string& iconPath);
		long GetIdleTime();

		SharedMenu GetMenu();
		SharedMenu GetContextMenu();
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

		private:
		ScriptEvaluator evaluator;
		SharedPtr<Win32Menu> menu;
		SharedPtr<Win32Menu> contextMenu;
		std::string iconPath;

		// Cookie and handle for WebKit Activation Context
		ULONG_PTR lpCookie; 
		HANDLE pActCtx;
		static std::vector<HICON> loadedICOs;
		static std::vector<HBITMAP> loadedBMPs;

		void InitCurl(Module *module);
	};
}

#endif
