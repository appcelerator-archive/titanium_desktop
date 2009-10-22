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
		GtkTrayItem(std::string& iconURL, KMethodRef cb);
		~GtkTrayItem();

		void SetIcon(std::string& iconPath);
		void SetMenu(AutoMenu menu);
		void SetHint(std::string& hint);
		void Remove();

		GtkStatusIcon* GetWidget();
		AutoPtr<GtkMenu> GetMenu();
		KMethodRef GetCallback();

	protected:
		GtkStatusIcon* item;
		AutoPtr<GtkMenu> menu;
		KMethodRef callback;
		bool active;
	};
}

#endif
