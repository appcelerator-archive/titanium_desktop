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

		SharedUserWindow CreateWindow(WindowConfig*, SharedUserWindow& parent);
		void ErrorDialog(std::string);

		SharedMenu CreateMenu();
		SharedMenuItem CreateMenuItem(std::string label, SharedKMethod eventListener, std::string iconURL);
		SharedMenuItem CreateCheckMenuItem(std::string label, SharedKMethod eventListener);
		SharedMenuItem CreateSeparatorMenuItem();
		SharedMenu GetMenu();
		SharedMenu GetContextMenu();
		SharedMenu GetDockMenu();
		NSMenu* GetNativeDockMenu();
		void SetMenu(SharedMenu);
		void SetContextMenu(SharedMenu);
		void SetDockMenu(SharedMenu);

		SharedTrayItem AddTray(SharedString icon_path, SharedKMethod cb);
		void SetIcon(SharedString icon_path);
		virtual void SetDockIcon(SharedString icon_path);
		virtual void SetBadge(SharedString badge_label);
		virtual void SetBadgeImage(SharedString badge_path);

		SharedPtr<OSXMenu> GetActiveMenu();
		void WindowFocused(SharedPtr<OSXUserWindow> window);
		void WindowUnfocused(SharedPtr<OSXUserWindow> window);
		void SetupMainMenu(bool force = false);
		void SetupAppMenuParts(NSMenu* mainMenu);
		void ReplaceMainMenu();
		NSMenu* GetDefaultMenu();

		long GetIdleTime();
		static NSImage* MakeImage(std::string);

	protected:
		NSMenu* defaultMenu;
		SharedPtr<OSXMenu> menu;
		NSMenu* nativeMenu;
		SharedPtr<OSXMenu> contextMenu;
		SharedPtr<OSXMenu> dockMenu;
		NSMenu* nativeDockMenu;
		NSView *savedDockView;
		NSObject* application;
		SharedPtr<OSXMenu> activeMenu;
		SharedPtr<OSXUserWindow> activeWindow;
		ScriptEvaluator* scriptEvaluator;

		void InstallMenu (OSXMenuItem*);
	};
}

#endif
