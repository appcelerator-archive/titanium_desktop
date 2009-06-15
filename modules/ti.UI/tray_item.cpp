/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#include <kroll/kroll.h>
#include "ui_module.h"

namespace ti
{

	TrayItem::TrayItem() : StaticBoundObject()
	{
		/**
		 * @tiapi(method=True,name=UI.Tray.setIcon,since=0.2) Sets a TrayItem's icon
		 * @tiarg(for=UI.Tray.setIcon,name=icon,type=string,optional=True) path to the icon or null to unset
		 */
		this->SetMethod("setIcon", &TrayItem::_SetIcon);
		/**
		 * @tiapi(method=True,name=UI.Tray.setMenu,since=0.2) Sets a TrayItem's menu
		 * @tiarg(for=UI.Tray.setMenu,name=menu,type=object,optional=True) a MenuItem value or null to unset
		 */
		this->SetMethod("setMenu", &TrayItem::_SetMenu);
		/**
		 * @tiapi(method=True,name=UI.Tray.setHint,since=0.2) Sets a TrayItem's tooltip
		 * @tiarg(for=UI.Tray.setHint,name=hint,type=string,optional=True) tooltip value or null to unset
		 */
		this->SetMethod("setHint", &TrayItem::_SetHint);
		/**
		 * @tiapi(method=True,name=UI.Tray.remove,since=0.2) Removes a TrayItem
		 */
		this->SetMethod("remove", &TrayItem::_Remove);
	}

	TrayItem::~TrayItem()
	{
	}

	void TrayItem::_SetIcon(const ValueList& args, SharedValue result)
	{
		bool valid = args.Verify("s");
		if (!valid)
			return;

		const char *icon_url = args.at(0)->ToString();
		SharedString icon_path = UIModule::GetResourcePath(icon_url);
		if (!icon_path.isNull())
		{
			this->SetIcon(icon_path);
		}
	}

	void TrayItem::_SetMenu(const ValueList& args, SharedValue result)
	{
		SharedPtr<MenuItem> menu = NULL; // A NULL value is an unset
		if (args.size() > 0 && args.at(0)->IsList())
		{
			menu = args.at(0)->ToList().cast<MenuItem>();
		}
		this->SetMenu(menu);
	}

	void TrayItem::_SetHint(const ValueList& args, SharedValue result)
	{
		SharedString hint = NULL; // A NULL value is an unset
		if (args.size() > 0 && args.at(0)->IsString())
		{
			hint = new std::string(args.at(0)->ToString());
		}
		this->SetHint(hint);
	}

	void TrayItem::_Remove(const ValueList& args, SharedValue result)
	{
		this->Remove();
		UIModule::UnregisterTrayItem(this);
	}

}
