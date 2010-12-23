/*
 * Copyright (c) 2009-2010 Appcelerator, Inc. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef UIWin_h
#define UIWin_h

#include "../UI.h"

#include <cairo/cairo.h>

#define WEB_INSPECTOR_MENU_ITEM_ID 7500
#define NEXT_ITEM_ID_BEGIN 7501

namespace Titanium {

class MenuWin;

class UIWin : public UI {
public:
	UIWin();
	~UIWin();

	AutoPtr<Menu> CreateMenu();
	AutoPtr<MenuItem> CreateMenuItem();
	AutoPtr<MenuItem> CreateSeparatorMenuItem();
	AutoPtr<MenuItem> CreateCheckMenuItem();
	void SetMenu(AutoPtr<Menu>);
	AutoPtr<TrayItem> AddTray(std::string& icon_path, KMethodRef cbSingleClick);
	void SetContextMenu(AutoPtr<Menu>);
	void SetIcon(std::string& iconPath);
	long GetIdleTime();

	AutoPtr<Menu> GetMenu();
	AutoPtr<Menu> GetContextMenu();
	std::string& GetIcon();
	static UINT nextItemId;

	static HICON LoadImageAsIcon(std::string& path, int sizeX, int sizeY);
	static HBITMAP LoadImageAsBitmap(std::string& path, int sizeX, int sizeY);
	static HICON BitmapToIcon(HBITMAP bitmap, int sizeX, int sizeY);
	static HBITMAP IconToBitmap(HICON icon, int sizeX, int sizeY);
	static HBITMAP LoadPNGAsBitmap(std::string& path, int sizeX, int sizeY);
	static cairo_surface_t* ScaleCairoSurface(
		cairo_surface_t *oldSurface, int newWidth, int newHeight);
	static void ReleaseImage(HANDLE);
	static void SetProxyForURL(std::string& url);
	static void ErrorDialog(std::string);

private:
	AutoPtr<MenuWin> menu;
	AutoPtr<MenuWin> contextMenu;
	std::string iconPath;
	static std::vector<HICON> loadedICOs;
	static std::vector<HBITMAP> loadedBMPs;
};

} // namespace Titanium

#endif
