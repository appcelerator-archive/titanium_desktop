/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#include <kroll/kroll.h>
#include "ui_module.h"

namespace ti
{

	TrayItem::TrayItem() : StaticBoundObject("TrayItem")
	{
		/**
		 * @tiapi(method=True,name=UI.Tray.setIcon,since=0.2) Sets a TrayItem's icon
		 * @tiarg(for=UI.Tray.setIcon,name=icon,type=string,optional=True) path to the icon or null to unset
		 */
		this->SetMethod("setIcon", &TrayItem::_SetIcon);

		/**
		 * @tiapi(method=True,name=UI.Tray.getIcon,since=1.0)
		 * @tiapi Get the icon URL for this TrayItem
		 * @tiresult[String] The icon URL in use for this TrayItem
		 */
		this->SetMethod("getIcon", &TrayItem::_GetIcon);

		/**
		 * @tiapi(method=True,name=UI.Tray.setMenu,since=1.0)
		 * @tiapi Set the menu for this TrayItem
		 * @tiarg[UI.Menu|null, menu] The Menu to use for this TrayItem or null to unset
		 */
		this->SetMethod("setMenu", &TrayItem::_SetMenu);

		/**
		 * @tiapi(method=True,name=UI.Tray.getMenu,since=1.0)
		 * @tiapi Get the menu for this TrayItem
		 * @tiresult[UI.Menu|null] Thh Menu in use for this TrayItem or null if unset
		 */
		this->SetMethod("getMenu", &TrayItem::_GetMenu);

		/**
		 * @tiapi(method=True,name=UI.Tray.setHint,since=0.2) Sets a TrayItem's tooltip
		 * @tiarg(for=UI.Tray.setHint,name=hint,type=string,optional=True) tooltip value or null to unset
		 */
		this->SetMethod("setHint", &TrayItem::_SetHint);

		/**
		 * @tiapi(method=True,name=UI.Tray.getHint,since=1.0)
		 * @tiapi Get the hint for this TrayItem
		 * @tiresult[String] The hint in use for this TrayItem or an empty string if unset
		 */
		this->SetMethod("getHint", &TrayItem::_GetHint);

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
		bool valid = args.Verify("s"); if (!valid)
			return;

		const char *iconURL = args.at(0)->ToString();
		SharedString icon_path = UIModule::GetResourcePath(iconURL);
		if (!icon_path.isNull())
		{
			this->SetIcon(icon_path);
		}
		this->iconURL = iconURL;
	}

	void TrayItem::_GetIcon(const ValueList& args, SharedValue result)
	{
		result->SetString(this->iconURL);
	}

	void TrayItem::_SetMenu(const ValueList& args, SharedValue result)
	{
		SharedMenu menu = NULL; // A NULL value is an unset
		if (args.size() > 0 && args.at(0)->IsList())
		{
			menu = args.at(0)->ToObject().cast<Menu>();
		}
		this->SetMenu(menu);
		this->menu = menu;
	}

	void TrayItem::_GetMenu(const ValueList& args, SharedValue result)
	{
		result->SetObject(this->menu);
	}

	void TrayItem::_SetHint(const ValueList& args, SharedValue result)
	{
		SharedString hint = NULL; // A NULL value is an unset
		if (args.size() > 0 && args.at(0)->IsString())
		{
			hint = new std::string(args.at(0)->ToString());
		}
		this->SetHint(hint);
		this->hint = hint->c_str();
	}

	void TrayItem::_GetHint(const ValueList& args, SharedValue result)
	{
		result->SetString(this->hint);
	}

	void TrayItem::_Remove(const ValueList& args, SharedValue result)
	{
		this->Remove();
		UIBinding::GetInstance()->UnregisterTrayItem(this);
	}
}
