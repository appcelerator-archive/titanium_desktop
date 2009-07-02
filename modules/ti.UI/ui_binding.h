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
	class UIBinding : public StaticBoundObject
	{

	public:
		static int CENTERED;
		static std::string FOCUSED;
		static std::string UNFOCUSED;
		static std::string OPEN;
		static std::string OPENED;
		static std::string CLOSE;
		static std::string CLOSED;
		static std::string HIDDEN;
		static std::string SHOWN;
		static std::string FULLSCREENED;
		static std::string UNFULLSCREENED;
		static std::string MAXIMIZED;
		static std::string MINIMIZED;
		static std::string RESIZED;
		static std::string MOVED;
		static std::string PAGE_INITIALIZED;
		static std::string PAGE_LOADED;
		static std::string CREATE;
		static std::string ACTIVATE;
		static std::string CLICKED;
		UIBinding(Host *host);

		virtual ~UIBinding();
		Host* GetHost();
		static UIBinding* GetInstance() { return instance; }
		virtual void CreateMainWindow(WindowConfig*);
		virtual SharedUserWindow CreateWindow(WindowConfig*, SharedUserWindow& parent) = 0;
		virtual void ErrorDialog(std::string);

		std::vector<SharedUserWindow>& GetOpenWindows();
		void AddToOpenWindows(SharedUserWindow);
		void RemoveFromOpenWindows(SharedUserWindow);
		void ClearTray();
		void UnregisterTrayItem(TrayItem*);
		static void SendEventToListeners(
			std::vector<SharedKMethod> eventListeners,
			std::string eventType,
			SharedKObject eventSource,
			SharedKObject event = new StaticBoundObject());
		static void SendEventToListener(SharedKMethod listener, SharedKObject event);

		void _GetOpenWindows(const ValueList& args, SharedValue result);
		void _CreateMenu(const ValueList& args, SharedValue result);
		void _CreateMenuItem(const ValueList& args, SharedValue result);
		void _CreateCheckMenuItem(const ValueList& args, SharedValue result);
		void _CreateSeparatorMenuItem(const ValueList& args, SharedValue result);
		SharedMenu __CreateMenu(const ValueList& args);
		SharedMenuItem __CreateMenuItem(const ValueList& args);
		SharedMenuItem __CreateCheckMenuItem(const ValueList& args);
		SharedMenuItem __CreateSeparatorMenuItem(const ValueList& args);
		void _SetMenu(const ValueList& args, SharedValue result);
		void _GetMenu(const ValueList& args, SharedValue result);
		void _SetContextMenu(const ValueList& args, SharedValue result);
		void _GetContextMenu(const ValueList& args, SharedValue result);
		void _SetIcon(const ValueList& args, SharedValue result);
		void _AddTray(const ValueList& args, SharedValue result);
		void _ClearTray(const ValueList& args, SharedValue result);
		void _GetIdleTime(const ValueList& args, SharedValue result);

		/* OS X specific callbacks */
		void _SetDockIcon(const ValueList& args, SharedValue result);
		void _SetDockMenu(const ValueList& args, SharedValue result);
		void _SetBadge(const ValueList& args, SharedValue result);
		void _SetBadgeImage(const ValueList& args, SharedValue result);

		virtual SharedMenu CreateMenu() = 0;
		virtual SharedMenuItem CreateMenuItem(std::string label, SharedKMethod eventListener, std::string iconURL) = 0;
		virtual SharedMenuItem CreateCheckMenuItem(std::string label, SharedKMethod callback) = 0;
		virtual SharedMenuItem CreateSeparatorMenuItem() = 0;
		virtual void SetMenu(SharedMenu) = 0;
		virtual void SetContextMenu(SharedMenu) = 0;
		virtual void SetIcon(SharedString iconPath) = 0;
		virtual SharedTrayItem AddTray(SharedString iconPath, SharedKMethod cb) = 0;
		virtual SharedMenu GetMenu() = 0;
		virtual SharedMenu GetContextMenu() = 0;
		virtual long GetIdleTime() = 0;

		/* These have empty impls, because are OS X-only for now */
		virtual void SetDockIcon(SharedString icon_path) {}
		virtual void SetDockMenu(SharedMenu) {}
		virtual void SetBadge(SharedString badgeLabel) {}
		virtual void SetBadgeImage(SharedString badgeImagePath) {}

	protected:
		static UIBinding* instance;
		Host* host;
		std::vector<SharedUserWindow> openWindows;
		SharedKList openWindowList;
		std::vector<SharedTrayItem> trayItems;
		std::string iconPath;
	};
}

#endif
