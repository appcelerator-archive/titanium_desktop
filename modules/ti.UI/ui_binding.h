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
		UIBinding(Host *host);
		virtual ~UIBinding();
		Host* GetHost();

		virtual void CreateMainWindow(WindowConfig*);
		virtual SharedUserWindow CreateWindow(WindowConfig*, SharedUserWindow& parent) = 0;
		virtual void ErrorDialog(std::string);

		std::vector<SharedUserWindow>& GetOpenWindows();
		void AddToOpenWindows(SharedUserWindow);
		void RemoveFromOpenWindows(SharedUserWindow);
		void ClearTray();
		void UnregisterTrayItem(TrayItem*);

		static UIBinding* GetInstance() { return instance; }

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

	protected:
		Host* host;

	private:
		static UIBinding* instance;
		std::vector<SharedUserWindow> openWindows;
		SharedKList openWindowList;

		void _GetOpenWindows(const ValueList& args, SharedValue result);
		void _CreateMenu(const ValueList& args, SharedValue result);
		void _CreateMenuItem(const ValueList& args, SharedValue result);
		void _CreateCheckMenuItem(const ValueList& args, SharedValue result);
		void _CreateSeparatorMenuItem(const ValueList& args, SharedValue result);
		void _CreateTrayMenu(const ValueList& args, SharedValue result);
		void _SetMenu(const ValueList& args, SharedValue result);
		void _GetMenu(const ValueList& args, SharedValue result);
		void _SetContextMenu(const ValueList& args, SharedValue result);
		void _GetContextMenu(const ValueList& args, SharedValue result);
		void _SetIcon(const ValueList& args, SharedValue result);
		void _AddTray(const ValueList& args, SharedValue result);
		void _ClearTray(const ValueList& args, SharedValue result);
		void _GetIdleTime(const ValueList& args, SharedValue result);

		virtual SharedMenu CreateMenu() = 0;
		virtual SharedMenuItem CreateMenuItem(std::string label, SharedKMethod eventListener, std::string iconURL) = 0;
		virtual SharedMenuItem CreateCheckMenuItem(std::string label, SharedKMethod callback) = 0;
		virtual SharedMenuItem CreateSeparatorMenuItem() = 0;
		virtual void SetMenu(SharedMenu) = 0;
		virtual void SetContextMenu(SharedMenu>) = 0;
		virtual void SetIcon(SharedString iconPath) = 0;
		virtual SharedPtr<TrayItem> CreateTrayItem(SharedString iconPath, SharedKMethod cb) = 0;

		/* OS X specific callbacks */
		void _SetDockIcon(const ValueList& args, SharedValue result);
		void _SetDockMenu(const ValueList& args, SharedValue result);
		void _SetBadge(const ValueList& args, SharedValue result);
		void _SetBadgeImage(const ValueList& args, SharedValue result);

		/* These have empty impls, because are OS X-only for now */
		virtual void SetDockIcon(SharedString icon_path) {}
		virtual void SetDockMenu(SharedMenuItem) {}
		virtual void SetBadge(SharedString badge_label) {}
		virtual void SetBadgeImage(SharedString badge_path) {}

		virtual long GetIdleTime() = 0;

	};
}

#endif
