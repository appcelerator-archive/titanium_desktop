/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#ifndef _OSX_MENU_ITEM_IMPL_H_
#define _OSX_MENU_ITEM_IMPL_H_

#include <Cocoa/Cocoa.h>
#include "../menu_item.h"

namespace ti
{

	class OSXMenuItem : public MenuItem
	{

	public:
		OSXMenuItem();
		virtual ~OSXMenuItem();

		void SetParent(OSXMenuItem* parent);
		OSXMenuItem* GetParent();

		SharedValue AddSeparator();
		SharedValue AddItem(SharedValue label,
		                    SharedValue callback,
		                    SharedValue icon_url);
		SharedValue AddSubMenu(SharedValue label,
		                       SharedValue icon_url);

		SharedValue AppendItem(OSXMenuItem* item);

		void Enable();
		void Disable();
		void SetLabel(std::string label);
		void SetIcon(std::string icon_path);

		NSMenuItem* CreateNative();
		void AttachMenu(NSMenu *);
		void Invoke();
		int GetChildCount();
		OSXMenuItem* GetChild(int c);
		bool IsEnabled();

	private:
		OSXMenuItem *parent; // NULL parent means this is top-level menu.
		std::vector<OSXMenuItem*> children;
		bool enabled;
	};

}

#endif
