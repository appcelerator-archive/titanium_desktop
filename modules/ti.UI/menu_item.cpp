/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#include <kroll/kroll.h>
#include "ui_module.h"
namespace ti
{
	using std::string;
	using std::vector;

	MenuItem::MenuItem(MenuItemType type) :
		KEventObject("UI.MenuItem"),
		type(type),
		enabled(true),
		label(""),
		submenu(0),
		state(false),
		autoCheck(true)
	{
		this->SetMethod("isSeparator", &MenuItem::_IsSeparator);
		this->SetMethod("isCheck", &MenuItem::_IsCheck);

		if (this->type == NORMAL || this->type == CHECK)
		{
			this->SetMethod("setLabel", &MenuItem::_SetLabel);
			this->SetMethod("getLabel", &MenuItem::_GetLabel);
			this->SetMethod("setSubmenu", &MenuItem::_SetSubmenu);
			this->SetMethod("getSubmenu", &MenuItem::_GetSubmenu);
			this->SetMethod("enable", &MenuItem::_Enable);
			this->SetMethod("disable", &MenuItem::_Disable);
			this->SetMethod("isEnabled", &MenuItem::_IsEnabled);
			this->SetMethod("addItem", &MenuItem::_AddItem);
			this->SetMethod("addSeparatorItem", &MenuItem::_AddSeparatorItem);
			this->SetMethod("addCheckItem", &MenuItem::_AddCheckItem);

			// This is only for testing and should remain undocumented
			this->SetMethod("click", &MenuItem::_Click);
		}

		if (this->type == NORMAL)
		{
			this->SetMethod("setIcon", &MenuItem::_SetIcon);
			this->SetMethod("getIcon", &MenuItem::_GetIcon);
		}

		if (this->type == CHECK)
		{
			this->SetMethod("setState", &MenuItem::_SetState);
			this->SetMethod("getState", &MenuItem::_GetState);
			this->SetMethod("setAutoCheck", &MenuItem::_SetAutoCheck);
			this->SetMethod("isAutoCheck", &MenuItem::_IsAutoCheck);
		}
	}

	MenuItem::~MenuItem()
	{
	}

	void MenuItem::_IsSeparator(const ValueList& args, KValueRef result)
	{
		result->SetBool(this->type == SEPARATOR);
	}

	void MenuItem::_IsCheck(const ValueList& args, KValueRef result)
	{
		result->SetBool(this->type == CHECK);
	}

	void MenuItem::_SetLabel(const ValueList& args, KValueRef result)
	{
		args.VerifyException("setLabel", "s|0");
		string newLabel = args.GetString(0, "");
		this->SetLabel(newLabel);
	}

	void MenuItem::_GetLabel(const ValueList& args, KValueRef result)
	{
		result->SetString(this->label);
	}

	void MenuItem::_SetIcon(const ValueList& args, KValueRef result)
	{
		args.VerifyException("setIcon", "s|0");
		std::string newIcon = "";
		if (args.size() > 0) {
			newIcon = args.GetString(0);
		}
		this->SetIcon(newIcon);
	}

	void MenuItem::_GetIcon(const ValueList& args, KValueRef result)
	{
		result->SetString(this->iconURL);
	}

	void MenuItem::_SetState(const ValueList& args, KValueRef result)
	{
		args.VerifyException("setState", "b");
		this->SetState(args.GetBool(0));
	}

	void MenuItem::_GetState(const ValueList& args, KValueRef result)
	{
		result->SetBool(this->state);
	}

	void MenuItem::_SetSubmenu(const ValueList& args, KValueRef result)
	{
		args.VerifyException("setSubmenu", "o|0");
		AutoMenu newSubmenu = NULL;

		if (args.at(0)->IsObject())
		{
			KObjectRef o = args.at(0)->ToObject();
			o = KObject::Unwrap(o);
			newSubmenu = o.cast<Menu>();
		}

		if (!newSubmenu.isNull() && newSubmenu->ContainsItem(this))
		{
			throw ValueException::FromString("Tried to construct a recursive menu");
		}

		this->submenu = newSubmenu;
		this->SetSubmenuImpl(newSubmenu);
	}

	void MenuItem::_GetSubmenu(const ValueList& args, KValueRef result)
	{
		if (this->submenu.isNull())
		{
			result->SetNull();
		}
		else
		{
			result->SetObject(this->submenu);
		}
	}

	void MenuItem::_Enable(const ValueList& args, KValueRef result)
	{
		this->enabled = true;
		this->SetEnabledImpl(true);
	}

	void MenuItem::_Disable(const ValueList& args, KValueRef result)
	{
		this->enabled = false;
		this->SetEnabledImpl(false);
	}

	void MenuItem::_SetAutoCheck(const ValueList& args, KValueRef result)
	{
		args.VerifyException("setAutoCheck", "b");
		this->autoCheck = args.GetBool(0);
	}

	void MenuItem::_IsAutoCheck(const ValueList& args, KValueRef result)
	{
		result->SetBool(this->autoCheck);
	}

	void MenuItem::_IsEnabled(const ValueList& args, KValueRef result)
	{
		result->SetBool(this->enabled);
	}

	void MenuItem::_Click(const ValueList& args, KValueRef result)
	{
		this->HandleClickEvent(0);
	}

	void MenuItem::_AddItem(const ValueList& args, KValueRef result)
	{
		args.VerifyException("addItem", "?s m|0 s|0");
		UIBinding* binding = UIBinding::GetInstance();

		// Create a menu item object and add it to this item's submenu
		AutoMenuItem newItem = binding->__CreateMenuItem(args);
		this->EnsureHasSubmenu();
		this->submenu->AppendItem(newItem);

		result->SetObject(newItem);
	}

	void MenuItem::_AddSeparatorItem(const ValueList& args, KValueRef result)
	{
		UIBinding* binding = UIBinding::GetInstance();
		AutoMenuItem newItem = binding->__CreateSeparatorMenuItem(args);
		this->EnsureHasSubmenu();
		this->submenu->AppendItem(newItem);

		result->SetObject(newItem);
	}

	void MenuItem::_AddCheckItem(const ValueList& args, KValueRef result)
	{
		UIBinding* binding = UIBinding::GetInstance();

		// Create a menu item object
		AutoMenuItem newItem = binding->__CreateCheckMenuItem(args);
		this->EnsureHasSubmenu();
		this->submenu->AppendItem(newItem);

		result->SetObject(newItem);
	}

	void MenuItem::HandleClickEvent(KObjectRef source)
	{
		if (this->FireEvent(Event::CLICKED)
			&& this->IsCheck() && this->autoCheck)
		{
			// Execute this later on the main thread
			RunOnMainThread(this->Get("setState")->ToMethod(),
				ValueList(Value::NewBool(!this->GetState())), false);
		}
	}

	void MenuItem::SetState(bool newState)
	{
		this->state = newState;
		this->SetStateImpl(newState);
	}

	void MenuItem::SetLabel(string& newLabel)
	{
		this->label = newLabel;
		this->SetLabelImpl(newLabel);
	}

	void MenuItem::SetIcon(string& iconURL)
	{
		this->iconPath = this->iconURL = iconURL;
		if (!iconURL.empty()) {
			this->iconPath = URLUtils::URLToPath(this->iconURL);
		}
		this->SetIconImpl(this->iconPath); // platform-specific impl
	}

	bool MenuItem::GetState()
	{
		return this->state;
	}

	std::string& MenuItem::GetLabel()
	{
		return this->label;
	}

	bool MenuItem::IsSeparator()
	{
		return this->type == SEPARATOR;
	}

	bool MenuItem::IsCheck()
	{
		return this->type == CHECK;
	}

	bool MenuItem::IsEnabled()
	{
		return this->enabled;
	}

	void MenuItem::EnsureHasSubmenu()
	{
		if (this->submenu.isNull())
		{
			UIBinding* binding = UIBinding::GetInstance();
			AutoMenu newSubmenu = binding->CreateMenu();
			this->SetSubmenuImpl(newSubmenu);
			this->submenu = newSubmenu;
		}
	}

	bool MenuItem::ContainsItem(MenuItem* item)
	{
		return !this->submenu.isNull() &&
			this->submenu->ContainsItem(item);
	}

	bool MenuItem::ContainsSubmenu(Menu* submenu)
	{
		return !this->submenu.isNull() &&
			(this->submenu.get() == submenu ||
			this->submenu->ContainsSubmenu(submenu));
	}
}
