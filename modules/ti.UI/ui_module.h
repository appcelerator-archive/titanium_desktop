/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */
#ifndef _UI_MODULE_H_
#define _UI_MODULE_H_

#include <kroll/kroll.h>

namespace ti {
	class UIBinding;
	class Menu;
	class MenuItem;
	class TrayItem;
	class UserWindow;

	typedef SharedPtr<UserWindow> SharedUserWindow;
	typedef SharedPtr<UIBinding> SharedUIBinding;
	typedef SharedPtr<Menu> SharedMenu;
	typedef SharedPtr<MenuItem> SharedMenuItem;
	typedef SharedPtr<TrayItem> SharedTrayItem;
}

#include <JavaScriptCore/JSObjectRef.h>
#include <JavaScriptCore/JSStringRef.h>
#include <JavaScriptCore/JSContextRef.h>

#include <iostream>
#include "javascript_module.h"
#include "window_config.h"
#include "user_window.h"
#include "menu.h"
#include "menu_item.h"
#include "tray_item.h"
#include "ui_binding.h"

#ifdef OS_LINUX
#include "gtk/ui_module_gtk.h"
#include "gtk/script_evaluator.h"

#elif defined(OS_OSX)
#include "osx/ui_module_osx.h"

#elif defined(OS_WIN32)
#include "win32/ui_module_win32.h"
#endif

namespace ti {

	class UIModule : public kroll::Module
	{
		KROLL_MODULE_CLASS(UIModule)

		public:

		/*
		 * Function: GetResourcePath
		 * Get the real path to a resource given an app:// URL. This
		 * will access the appropriate function in the global object.
		 * This will also just return the URL if the URL passed is
		 * not app://
		 *
		 * Arguments:
		 *  url - the app:// URL to resolve (can also be http://, etc)
		 * Returns: The path to the resource on this system or a
		 *          NULL SharedString on failure.
		 */
		static SharedString GetResourcePath(const char *URL);

		/**
		 * can be called to determine if the URI passed is a local file
		 * URI vs remote scheme (such as HTTP).
		 *
		 */
		static bool IsResourceLocalFile(std::string string);

		static UIModule* GetInstance() { return instance_; }
		SharedUIBinding GetUIBinding() { return uiBinding; }
		void Exiting(int exitcode);
		void Start();

		protected:
		DISALLOW_EVIL_CONSTRUCTORS(UIModule);
		static UIModule* instance_;
		SharedUIBinding uiBinding;

	};
}

#endif
