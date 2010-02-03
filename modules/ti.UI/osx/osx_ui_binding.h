/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#ifndef _OSX_UI_BINDING_H_
#define _OSX_UI_BINDING_H_
#include <kroll/kroll.h>
#include "../ui_module.h"
namespace ti
{
	class OSXMenuItem;
	class OSXUIBinding : public UIBinding
	{

		public:
		OSXUIBinding(Host *host);
		~OSXUIBinding();

		AutoMenu CreateMenu();
		AutoMenuItem CreateMenuItem();
		AutoMenuItem CreateCheckMenuItem();
		AutoMenuItem CreateSeparatorMenuItem();
		AutoMenu GetMenu();
		AutoMenu GetContextMenu();
		AutoMenu GetDockMenu();
		NSMenu* GetNativeDockMenu();
		void SetMenu(AutoMenu);
		void SetContextMenu(AutoMenu);
		void SetDockMenu(AutoMenu);

		AutoTrayItem AddTray(std::string& icon_path, KMethodRef cb);
		void SetIcon(std::string& iconPath);
		virtual void SetDockIcon(std::string& iconPath);
		virtual void SetBadge(std::string& badgeLabel);
		virtual void SetBadgeImage(std::string& badgeImage);

		AutoPtr<OSXMenu> GetActiveMenu();
		void WindowFocused(AutoPtr<OSXUserWindow> window);
		void WindowUnfocused(AutoPtr<OSXUserWindow> window);
		void SetupMainMenu(bool force = false);
		void SetupAppMenuParts(NSMenu* mainMenu);
		void ReplaceMainMenu();
		NSMenu* GetDefaultMenu();
		long GetIdleTime();

		static NSImage* MakeImage(std::string&);
		static void ErrorDialog(std::string);

	protected:
		NSMenu* defaultMenu;
		AutoPtr<OSXMenu> menu;
		NSMenu* nativeMenu;
		AutoPtr<OSXMenu> contextMenu;
		AutoPtr<OSXMenu> dockMenu;
		NSMenu* nativeDockMenu;
		NSView *savedDockView;
		NSObject* application;
		AutoPtr<OSXMenu> activeMenu;
		AutoPtr<OSXUserWindow> activeWindow;

		void InstallMenu (OSXMenuItem*);
	};
}

#endif
