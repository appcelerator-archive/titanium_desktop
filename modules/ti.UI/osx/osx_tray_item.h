/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#ifndef TI_OSX_TRAY_ITEM_H_
#define TI_OSX_TRAY_ITEM_H_
namespace ti
{
	class OSXTrayItem: public TrayItem
	{
	public:
		OSXTrayItem(std::string& iconURL, KMethodRef cb);
		virtual ~OSXTrayItem();

		void SetIcon(std::string& iconPath);
		void SetMenu(AutoMenu menu);
		void SetHint(std::string& hint);
		void Remove();
		void InvokeCallback();

	private:
		NSMenu* nativeMenu;
		AutoPtr<OSXMenu> menu;
		KMethodRef callback;
		NSStatusItem* nativeItem;
	};
}

#endif

