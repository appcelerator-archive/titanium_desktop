/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _UI_BINDING_H_
#define _UI_BINDING_H_

#include <kroll/kroll.h>
#include "menu_item.h"

namespace ti
{
	class UIBinding : public KAccessorObject
	{

	public:
		UIBinding(Host *host);
		virtual ~UIBinding();
		Host* GetHost();
		virtual void CreateMainWindow(WindowConfig*);
		virtual AutoUserWindow CreateWindow(WindowConfig*, AutoUserWindow& parent) = 0;

		std::vector<AutoUserWindow>& GetOpenWindows();
		void AddToOpenWindows(AutoUserWindow);
		void RemoveFromOpenWindows(AutoUserWindow);
		void ClearTray();
		void UnregisterTrayItem(TrayItem*);
		void _GetOpenWindows(const ValueList& args, KValueRef result);
		void _GetMainWindow(const ValueList& args, KValueRef result);
		void _CreateMenu(const ValueList& args, KValueRef result);
		void _CreateMenuItem(const ValueList& args, KValueRef result);
		void _CreateCheckMenuItem(const ValueList& args, KValueRef result);
		void _CreateSeparatorMenuItem(const ValueList& args, KValueRef result);
		AutoMenu __CreateMenu(const ValueList& args);
		AutoMenuItem __CreateMenuItem(const ValueList& args);
		AutoMenuItem __CreateCheckMenuItem(const ValueList& args);
		AutoMenuItem __CreateSeparatorMenuItem(const ValueList& args);
		void _SetMenu(const ValueList& args, KValueRef result);
		void _GetMenu(const ValueList& args, KValueRef result);
		void _SetContextMenu(const ValueList& args, KValueRef result);
		void _GetContextMenu(const ValueList& args, KValueRef result);
		void _SetIcon(const ValueList& args, KValueRef result);
		void _SetIcon(std::string iconURL);
		void _AddTray(const ValueList& args, KValueRef result);
		void _ClearTray(const ValueList& args, KValueRef result);
		void _GetIdleTime(const ValueList& args, KValueRef result);
		
		/* OS X specific callbacks */
		void _SetDockIcon(const ValueList& args, KValueRef result);
		void _SetDockMenu(const ValueList& args, KValueRef result);
		void _SetBadge(const ValueList& args, KValueRef result);
		void _SetBadgeImage(const ValueList& args, KValueRef result);

		virtual AutoMenu CreateMenu() = 0;
		virtual AutoMenuItem CreateMenuItem() = 0;;
		virtual AutoMenuItem CreateCheckMenuItem() = 0;
		virtual AutoMenuItem CreateSeparatorMenuItem() = 0;
		virtual void SetMenu(AutoMenu) = 0;
		virtual void SetContextMenu(AutoMenu) = 0;
		virtual void SetIcon(std::string& iconPath) = 0;
		virtual AutoTrayItem AddTray(std::string& iconPath, KMethodRef cbSingleClick) = 0;
		virtual AutoMenu GetMenu() = 0;
		virtual AutoMenu GetContextMenu() = 0;
		virtual long GetIdleTime() = 0;

		/* These have empty impls, because are OS X-only for now */
		virtual void SetDockIcon(std::string& icon_path) {}
		virtual void SetDockMenu(AutoMenu) {}
		virtual void SetBadge(std::string& badgeLabel) {}
		virtual void SetBadgeImage(std::string& badgeImagePath) {}

		static int CENTERED;
		static void ErrorDialog(std::string);
		static inline UIBinding* GetInstance() { return instance; }

	protected:
		static UIBinding* instance;
		Host* host;
		AutoUserWindow mainWindow;
		std::vector<AutoUserWindow> openWindows;
		std::vector<AutoTrayItem> trayItems;
		std::string iconURL;

		static void Log(Logger::Level level, std::string& message);
	};
}

#endif
