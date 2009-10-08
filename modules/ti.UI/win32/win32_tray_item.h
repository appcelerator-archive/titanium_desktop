/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#ifndef TI_WIN32_TRAY_ITEM_H_
#define TI_WIN32_TRAY_ITEM_H_
#include <windows.h>
#include <shellapi.h>
namespace ti
{
	class Win32TrayItem: public TrayItem
	{		
		public:
		Win32TrayItem(std::string& iconURL, SharedKMethod cbSingleClick);
		virtual ~Win32TrayItem();
		void SetIcon(std::string& iconPath);
		void SetMenu(AutoMenu menu);
		void SetHint(std::string& hint);
		void Remove();
		void ShowTrayMenu();
		void HandleRightClick();
		void HandleLeftClick();
		void HandleDoubleLeftClick();
		UINT GetId();
		static bool MessageHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		
		static LRESULT CALLBACK DoubleClickTimerProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

		private:
		HMENU oldNativeMenu;
		NOTIFYICONDATA* trayIconData;
		static std::vector<AutoPtr<Win32TrayItem> > trayItems;
		static UINT trayClickedMessage;
		bool is_double_clicked;
	};
}
#endif /* TI_WIN32_TRAY_ITEM_H_ */
