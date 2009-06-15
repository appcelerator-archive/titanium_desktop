/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#ifndef TI_OSX_TRAY_ITEM_H_
#define TI_OSX_TRAY_ITEM_H_

#include <Cocoa/Cocoa.h>
#include <kroll/kroll.h>
#include "../tray_item.h"
#include "../menu_item.h"

namespace ti
{
	class OSXTrayItem: public TrayItem
	{
	public:
		OSXTrayItem(SharedString iconPath, SharedKMethod cb);
		virtual ~OSXTrayItem();

		void SetIcon(SharedString iconPath);
		void SetMenu(SharedPtr<MenuItem> menu);
		void SetHint(SharedString hint);
		void Remove();
		
		void Invoke();

	private:
		SharedKMethod callback;
		id delegate;
	};
}

#endif

