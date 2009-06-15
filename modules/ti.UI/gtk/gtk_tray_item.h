/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */


#ifndef _GTK_TRAY_ITEM_H_
#define _GTK_TRAY_ITEM_H_

namespace ti
{
	class GtkTrayItem : public TrayItem
	{

	public:
		GtkTrayItem(SharedString icon_path,
		            SharedKMethod cb);
		~GtkTrayItem();

		void SetIcon(SharedString icon_path);
		void SetMenu(SharedPtr<MenuItem> menu);
		void SetHint(SharedString hint);
		void Remove();

		GtkStatusIcon* GetWidget();
		GtkWidget* GetMenuWidget();
		SharedPtr<GtkMenuItemImpl> GetMenu();

	protected:
		GtkStatusIcon* item;
		SharedPtr<GtkMenuItemImpl> menu;
		GtkWidget* menu_widget;
		SharedKMethod callback;
		bool active;
	};
}

#endif
