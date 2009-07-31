/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#include "win32_menu_item_impl.h"

#include <windows.h>
#include <shellapi.h>

#include "../ui_module.h"

#define STUB() printf("Method is still a stub, %s:%i\n", __FILE__, __LINE__)

namespace ti
{
	int Win32MenuItemImpl::currentUID = TI_MENU_ITEM_ID_BEGIN + 1;
	HMENU Win32MenuItemImpl::defaultContextMenu = NULL;

	static int webInspectorMenuItemID = 0;

	std::vector<Win32MenuItemImpl::NativeMenuItem *> itemsWithCallbacks;

	Win32MenuItemImpl::Win32MenuItemImpl(Win32MenuItemImpl* _parent) : parent(_parent)
	{

	}

	Win32MenuItemImpl::~Win32MenuItemImpl()
	{
	}

	void Win32MenuItemImpl::SetParent(Win32MenuItemImpl* parent)
	{
		this->parent = parent;
	}
	Win32MenuItemImpl* Win32MenuItemImpl::GetParent()
	{
		return this->parent;
	}

	SharedValue Win32MenuItemImpl::AddSeparator()
	{
		Win32MenuItemImpl* item = new Win32MenuItemImpl(this);
		item->MakeSeparator();
		this->children.push_back(item);

		return MenuItem::AddToListModel(item);
	}
	SharedValue Win32MenuItemImpl::AddItem(SharedValue label,
						SharedValue callback,
						SharedValue icon_url)
	{
		Win32MenuItemImpl* item = new Win32MenuItemImpl(this);
		item->MakeItem(label, callback, icon_url);
		this->children.push_back(item);

		return MenuItem::AddToListModel(item);
	}
	SharedValue Win32MenuItemImpl::AddSubMenu(SharedValue label,
						   SharedValue icon_url)
	{
		Win32MenuItemImpl* item = new Win32MenuItemImpl(this);
		item->MakeSubMenu(label, icon_url);
		this->children.push_back(item);

		return MenuItem::AddToListModel(item);
	}

	HMENU Win32MenuItemImpl::GetDefaultContextMenu()
	{
		if(Host::GetInstance()->IsDebugMode() && defaultContextMenu == NULL)
		{
			defaultContextMenu = CreatePopupMenu();
			RealizeWebInspectorMenuItem(defaultContextMenu);
		}

		return defaultContextMenu;
	}

	HMENU Win32MenuItemImpl::GetMenu()
	{
		if (this->parent == NULL) // top-level
		{
			NativeMenuItem* menu_item = this->Realize(NULL, false);

			// add web inspector menu item if needed
			if(Host::GetInstance()->IsDebugMode())
			{
				this->RealizeWebInspectorMenuItem(menu_item->menu);
			}

			return menu_item->menu;
		}
		else
		{
			// For now we do not support using a submenu as a menu,
			// as that makes determining parent-child relationships
			// really hard, so just return NULL and check above.
			return NULL;
		}
	}

	HMENU Win32MenuItemImpl::GetMenuBar()
	{
		if (this->parent == NULL) // top level
		{
			NativeMenuItem* menu_item = this->Realize(NULL, true);
			return menu_item->menu;
		}
		else
		{
			// For now we do not support using a submenu as a menu,
			// as that makes determining parent-child relationships
			// really hard, so just return NULL and check above.
			return NULL;
		}
	}

	void Win32MenuItemImpl::ClearRealization(HMENU parent_menu)
	{
		if(parent_menu == NULL)
		{
			// nothing to do
			return;
		}

		std::vector<NativeMenuItem*>::iterator i;

		// Find the instance which is contained in parent_menu or,
		// if we are the root, find the instance which uses this
		// menu to contain it's children.
		for (i = this->instances.begin(); i != this->instances.end(); i++)
		{
			if ((*i)->parent_menu == parent_menu || (this->parent == NULL && (*i)->menu == parent_menu))
				break;
		}

		// Could not find an instance which uses the menu.
		if (i == this->instances.end()) return;

		// Erase all children which use
		// the sub-menu as their parent.
		std::vector<Win32MenuItemImpl*>::iterator c;
		for (c = this->children.begin(); c != this->children.end(); c++)
		{
			(*c)->ClearRealization((*i)->menu);
		}

		this->instances.erase(i); // Erase the instance
	}

	Win32MenuItemImpl::NativeMenuItem* Win32MenuItemImpl::Realize(NativeMenuItem* parent_menu_item, bool is_menu_bar)
	{
		NativeMenuItem* menu_item = this->MakeNativeMenuItem(parent_menu_item, is_menu_bar);

		/* Realize this widget's children */
		if (this->IsSubMenu() || this->parent == NULL)
		{
			std::vector<Win32MenuItemImpl*>::iterator i = this->children.begin();
			while (i != this->children.end())
			{
				NativeMenuItem* child_menu_item = (*i)->Realize(menu_item, false);

				i++;
			}
		}

		this->instances.push_back(menu_item);
		return menu_item;
	}

	/*static*/
	void Win32MenuItemImpl::RealizeWebInspectorMenuItem(HMENU hMenu)
	{
		if(webInspectorMenuItemID == 0)
		{
			webInspectorMenuItemID = nextMenuUID();
		}

		if(GetMenuItemCount(hMenu) > 0)
		{
			AppendMenu(hMenu, MF_SEPARATOR, 1, "Separator");
		}

		std::string label("Show Inspector");
		AppendMenu(hMenu, MF_STRING, webInspectorMenuItemID, (LPCTSTR) label.c_str());
	}

	// this method creates the native menu objects for *this* menu item
	Win32MenuItemImpl::NativeMenuItem* Win32MenuItemImpl::MakeNativeMenuItem(NativeMenuItem* parent_menu_item, bool is_menu_bar)
	{
		NativeMenuItem* menu_item = new NativeMenuItem();

		if(parent_menu_item)
		{
			menu_item->parent_menu = parent_menu_item->menu;
		}

		const char* label = this->GetLabel();
		const char* iconUrl = this->GetIconURL();
		SharedString iconPath = UIModule::GetResourcePath(iconUrl);
		SharedValue callbackVal = this->RawGet("callback");

		if (this->parent == NULL) // top-level
		{
			if (is_menu_bar)
				menu_item->menu = CreateMenu();
			else
				menu_item->menu = CreatePopupMenu();
		}
		else if(this->IsSeparator())
		{
			AppendMenu(menu_item->parent_menu, MF_SEPARATOR, 1, "Separator");
		}
		else if(this->IsItem())
		{
			menu_item->menuItemID = nextMenuUID();
			AppendMenu(menu_item->parent_menu, MF_STRING, menu_item->menuItemID, (LPCTSTR) label);

			if (callbackVal->IsMethod())
			{
				menu_item->callback = callbackVal->ToMethod();

				itemsWithCallbacks.push_back(menu_item);
			}
		}
		else if(this->IsSubMenu())
		{
			menu_item->menu = CreatePopupMenu();
			AppendMenu(menu_item->parent_menu, MF_STRING | MF_POPUP, (UINT_PTR) menu_item->menu, (LPCTSTR) label);
		}
		else
		{
			throw ValueException::FromString("Unknown menu item type requested");
		}

		return menu_item;
	}

	SharedValue Win32MenuItemImpl::GetIconPath(const char *url)
	{
		STUB();
		return NULL;
	}

	void Win32MenuItemImpl::Enable()
	{
		for(std::vector<NativeMenuItem*>::iterator
			iter = this->instances.begin();
			iter != this->instances.end();
			iter++)
		   {
				NativeMenuItem* menu_item = (*iter);
				if (menu_item != NULL)
				{
					EnableMenuItem(menu_item->parent_menu, menu_item->menuItemID, MF_ENABLED);
				}
			} 
	}

	void Win32MenuItemImpl::Disable()
	{
		for(std::vector<NativeMenuItem*>::iterator
			iter = this->instances.begin();
			iter != this->instances.end();
			iter++)
		{
			NativeMenuItem* menu_item = (*iter);
			if (menu_item != NULL)
			{
				EnableMenuItem(menu_item->parent_menu, menu_item->menuItemID, MF_GRAYED);
			}
		}
	}

	void Win32MenuItemImpl::SetLabel(std::string label)
	{
		if(! this->parent || this->IsSeparator())
		{
			// top level menu doesn't have a label
			return;
		}
		std::vector<NativeMenuItem*>::iterator i = this->instances.begin();
		while (i != this->instances.end())
		{
			NativeMenuItem* menu_item = (*i);

			if (menu_item != NULL)
			{
				this->SetLabel(label, menu_item);
			}
			i++;
		}
	}
	void Win32MenuItemImpl::SetLabel(std::string label, NativeMenuItem* menu_item)
	{
		// TODO after modifying the menu, we need to call DrawMenuBar() on the window that contains this menu
		if(menu_item->menu)
		{
			// this is a sub menu
			ModifyMenu(menu_item->parent_menu, (UINT_PTR) menu_item->menu, MF_POPUP | MF_STRING, (UINT_PTR) menu_item->menu, label.c_str());
		}
		else
		{
			// this is a menu item
			ModifyMenu(menu_item->parent_menu, menu_item->menuItemID, MF_BYCOMMAND | MF_STRING, menu_item->menuItemID, label.c_str());
		}
	}

	void Win32MenuItemImpl::SetIcon(std::string icon_path)
	{
		STUB();
	}


	/*static*/
	bool Win32MenuItemImpl::invokeCallback(int menuItemUID)
	{
		for(size_t i = 0; i < itemsWithCallbacks.size(); i++)
		{
			Win32MenuItemImpl::NativeMenuItem* item = itemsWithCallbacks[i];

			if(item->menuItemID == menuItemUID)
			{
				KMethod* cb = (KMethod*) item->callback;

				// TODO: Handle exceptions in some way
				try
				{
					ValueList args;
					cb->Call(args);
				}
				catch(...)
				{
					Logger::GetRootLogger()->Error("Menu callback failed");
				}

				return true;
			}
		}

		return false;
	}

	/*static*/
	LRESULT CALLBACK Win32MenuItemImpl::handleMenuClick(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		if(message == WM_COMMAND)
		{
			int wmId = LOWORD(wParam);

			if(wmId == webInspectorMenuItemID)
			{
				Win32UserWindow *wuw = Win32UserWindow::FromWindow(hWnd);

				if(wuw)
				{
					wuw->ShowWebInspector();
					return true;
				}
				else
				{
					return false;
				}
			}
			else
			{
				return invokeCallback(wmId);
			}
		}

		return FALSE;
	}

}
