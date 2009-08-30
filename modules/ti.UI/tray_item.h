/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#ifndef _TRAY_ITEM_H_
#define _TRAY_ITEM_H_

#include <kroll/kroll.h>
#include "menu_item.h"

namespace ti
{
	class TrayItem : public KEventObject
	{

	public:
		TrayItem(std::string& iconURL);
		~TrayItem();

		virtual void SetIcon(std::string& iconPath) = 0;
		virtual void SetMenu(AutoMenu menu) = 0;
		virtual void SetHint(std::string& hint) = 0;
		virtual void Remove() = 0;

		void _SetIcon(const ValueList& args, SharedValue result);
		void _SetMenu(const ValueList& args, SharedValue result);
		void _SetHint(const ValueList& args, SharedValue result);
		void _GetIcon(const ValueList& args, SharedValue result);
		void _GetMenu(const ValueList& args, SharedValue result);
		void _GetHint(const ValueList& args, SharedValue result);
		void _Remove(const ValueList& args, SharedValue result);

	protected:
		AutoMenu menu;
		std::string iconURL;
		std::string iconPath;
		std::string hint;

	};
}

#endif
