/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _WIN32_UI_BINDING_H_
#define _WIN32_UI_BINDING_H_

#include "../ui_module.h"
#include "win32_menu_item_impl.h"

#include "script_evaluator.h"

namespace ti
{
	class Win32UIBinding : public UIBinding
	{

	public:
		Win32UIBinding(Module* uiModule, Host *host);
		~Win32UIBinding();

		SharedUserWindow CreateWindow(WindowConfig*, SharedUserWindow& parent);
		void ErrorDialog(std::string);

		SharedPtr<MenuItem> CreateMenu(bool trayMenu);
		void SetMenu(SharedPtr<MenuItem>);
		void SetContextMenu(SharedPtr<MenuItem>);
		void SetIcon(SharedString icon_path);
		SharedPtr<TrayItem> AddTray(SharedString icon_path,
		                            SharedKMethod cb);

		long GetIdleTime();

		static HMENU getContextMenuInUseHandle() { return contextMenuInUseHandle; }
		static bool IsWindowsXP();

	private:
		void InitCurl(Module *module);
		
		SharedPtr<Win32MenuItemImpl> contextMenuInUse;
		ScriptEvaluator script_evaluator;
		static HMENU contextMenuInUseHandle;
		// Cookie and handle for WebKit Activation Context
		ULONG_PTR lpCookie; 
		HANDLE pActCtx;
	};
}

#endif
