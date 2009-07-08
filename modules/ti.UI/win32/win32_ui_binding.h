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
		SharedMenuItem CreateMenuItem(
			std::string label, SharedKMethod callback, std::string iconURL);
		SharedMenuItem CreateSeparatorMenuItem();
		SharedMenuItem CreateCheckMenuItem(std::string label, SharedKMethod callback);
		void SetMenu(SharedMenu);
		SharedTrayItem AddTray(SharedString icon_path, SharedKMethod cb);
		void SetContextMenu(SharedMenu);
		void SetIcon(std::string& iconPath);
		long GetIdleTime();

		SharedMenu GetMenu();
		SharedMenu GetContextMenu();
		std::string& GetIcon();
		static UINT nextItemId;

		private:
		ScriptEvaluator evaluator;
		SharedPtr<Win32Menu> menu;
		SharedPtr<Win32Menu> contextMenu;
		std::string iconPath;

		// Cookie and handle for WebKit Activation Context
		ULONG_PTR lpCookie; 
		HANDLE pActCtx;

		void InitCurl(Module *module);
	};
}

#endif
