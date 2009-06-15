/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#ifndef _TI_WIN32_MENU_ITEM_IMPL_H_
#define _TI_WIN32_MENU_ITEM_IMPL_H_

#include "../menu_item.h"
#include <kroll/base.h>

#define TI_MENU_ITEM_ID_BEGIN 7500
#define TI_TRAY_CLICKED 7499

namespace ti
{

	class Win32MenuItemImpl : public MenuItem
	{
	public:
		Win32MenuItemImpl(Win32MenuItemImpl* _parent = NULL);
		virtual ~Win32MenuItemImpl();

		void SetParent(Win32MenuItemImpl* parent);
		Win32MenuItemImpl* GetParent();

		SharedValue AddSeparator();
		SharedValue AddItem(SharedValue label,
		                    SharedValue callback,
		                    SharedValue icon_url);
		SharedValue AddSubMenu(SharedValue label,
		                       SharedValue icon_url);

		SharedValue GetIconPath(const char *url);

		void Enable();
		void Disable();
		void SetLabel(std::string label);
		void SetIcon(std::string icon_path);

		HMENU GetMenu();
		HMENU GetMenuBar();
		void ClearRealization(HMENU parent_menu);

		struct NativeMenuItem
		{
			NativeMenuItem()
				 : menuItemID(0),
				   menu(NULL),
				   callback(NULL),
				   parent_menu(NULL) { }
			int menuItemID; // This item's menu item ID (only 'normal' menu items)
			HMENU menu; // This item's menu handle (only top-level menu and submenu items)
			SharedKMethod callback; // This item's callback
			HMENU parent_menu; // This item's parent's menu handle
		};

		static int nextMenuUID() {
			return currentUID++;
		}

		static HMENU GetDefaultContextMenu();
		static bool invokeCallback(int menuItemUID);
		static LRESULT CALLBACK handleMenuClick(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	private:
		Win32MenuItemImpl *parent;
		std::vector<Win32MenuItemImpl*> children;
		std::vector<NativeMenuItem*> instances;

		NativeMenuItem* Realize(NativeMenuItem* parent_menu_item, bool is_menu_bar);
		static void RealizeWebInspectorMenuItem(HMENU hMenu);
		NativeMenuItem* MakeNativeMenuItem(NativeMenuItem* parent_menu_item, bool is_menu_bar);
		void SetLabel(std::string label, NativeMenuItem* menu_item);

		static int currentUID;
		static HMENU defaultContextMenu;
	};

}

#endif /* _TI_WIN32_MENU_ITEM_IMPL_H_ */
