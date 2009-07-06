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
		OSXTrayItem(SharedString iconPath, SharedKMethod cb);
		virtual ~OSXTrayItem();

		void SetIcon(SharedString iconPath);
		void SetMenu(SharedMenu menu);
		void SetHint(SharedString hint);
		void Remove();
		void InvokeCallback();

	private:
		NSMenu* nativeMenu;
		SharedPtr<OSXMenu> menu;
		SharedKMethod callback;
		NSStatusItem* nativeItem;
	};
}

#endif

