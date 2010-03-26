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

		/**
		* @tiapi(method=True,name=UI.MenuItem.isSeparator,since=0.6)
		* @tiresult[Boolean] True if this item is a separator and false otherwise
		*/
		this->SetMethod("isSeparator", &MenuItem::_IsSeparator);

		/**
		* @tiapi(method=True,name=UI.MenuItem.isCheck,since=0.6)
		* @tiresult[Boolean] True if this item is a check item and false otherwise
		*/
		this->SetMethod("isCheck", &MenuItem::_IsCheck);

		if (this->type == NORMAL || this->type == CHECK)
		{

			/**
			* @tiapi(method=True,name=UI.MenuItem.setLabel,since=0.6)
			* @tiapi Set the label for this item.
			* @tiapi This method is not available for separator items.
			* @tiarg[String, label] The new label for this item
			*/
			this->SetMethod("setLabel", &MenuItem::_SetLabel);

			/**
			* @tiapi(method=True,name=UI.MenuItem.getLabel,since=0.6)
			* @tiapi Get this item's label.
			* @tiapi This method is not available for separator items.
			* @tiresult[String] The label for this item
			*/
			this->SetMethod("getLabel", &MenuItem::_GetLabel);

			/**
			* @tiapi(method=True,name=UI.MenuItem.setSubmenu,since=0.6)
			* @tiapi Set this item's submenu.
			* @tiapi This method is not available for separator items.
			* @tiarg[UI.Menu|null, menu] The submenu to use for this item or null to unset it
			*/
			this->SetMethod("setSubmenu", &MenuItem::_SetSubmenu);

			/**
			* @tiapi(method=True,name=UI.MenuItem.getSubmenu,since=0.6)
			* @tiapi Get this item's submenu.
			* @tiapi This method is not available for separator items.
			* @tiresult[UI.Menu|null] This item's submenu or null if it does not have one
			*/
			this->SetMethod("getSubmenu", &MenuItem::_GetSubmenu);

			/**
			* @tiapi(method=True,name=UI.MenuItem.enable,since=0.6)
			* @tiapi Enable this item i.e. make it clickable.
			* @tiapi This method is not available for separator items.
			*/
			this->SetMethod("enable", &MenuItem::_Enable);

			/**
			* @tiapi(method=True,name=UI.MenuItem.disable,since=0.6)
			* @tiapi Disable this item i.e. make it non-clickable.
			* @tiapi This method is not available for separator items.
			*/
			this->SetMethod("disable", &MenuItem::_Disable);

			/**
			* @tiapi(method=True,name=UI.MenuItem.isEnabled,since=0.6) 
			* @tiresult[Boolean] Whether or not this item is enabled
			* @tiapi This method is not available for separator items.
			*/
			this->SetMethod("isEnabled", &MenuItem::_IsEnabled);

			/**
			* @tiapi(method=True,name=UI.MenuItem.addItem,since=0.6)
			* @tiapi Add an item to this menu item's submenu with the given attributes.
			* @tiapi If this menu item does not have a submenu, it will be created.
			* @tiapi This method is not available for separator items.
			* @tiarg[String, label] The label for the new item
			* @tiarg[Function, listener, optional=True] An event listener callback for the item
			* @tiarg[String, iconURL] The URL for this item's icon
			* @tiresult[UI.MenuItem] The newly added item
			*/
			this->SetMethod("addItem", &MenuItem::_AddItem);

			/**
			* @tiapi(method=True,name=UI.MenuItem.addSeparatorItem,since=0.6)
			* @tiapi Add a separator item to this menu item's submenu.
			* @tiapi If this menu item does not have a submenu, it will be created.
			* @tiapi This method is not available for separator items.
			* @tiresult[UI.MenuItem] The newly added separator item
			*/
			this->SetMethod("addSeparatorItem", &MenuItem::_AddSeparatorItem);

			/**
			* @tiapi(method=True,name=UI.MenuItem.addCheckItem,since=0.6)
			* @tiapi Add a check item to this menu item's submenu with the given attributes.
			* @tiapi If this menu item does not have a submenu, it will be created.
			* @tiapi This method is not available for separator items.
			* @tiarg[String, label] The label for the new item
			* @tiarg[Function, listener, optional=True] An event listener callback for the item
			* @tiresult[UI.MenuItem] The newly added check item
			*/
			this->SetMethod("addCheckItem", &MenuItem::_AddCheckItem);

			// This is only for testing and should remain undocumented
			this->SetMethod("click", &MenuItem::_Click);
		}

		if (this->type == NORMAL)
		{
			/**
			* @tiapi(method=True,name=UI.MenuItem.setIcon,since=0.6)
			* @tiapi Set the icon URL for this item
			* @tiapi This method is not available for separator or check items.
			* @tiarg[String|null, iconURL] The new icon URL for this item or null to unset it
			*/
			this->SetMethod("setIcon", &MenuItem::_SetIcon);

			/**
			* @tiapi(method=True,name=UI.MenuItem.getIcon,since=0.6)
			* @tiapi Get this item's icon URL
			* @tiapi This method is not available for separator or check items.
			* @tiresult[String|null] The icon URL for this item or null if it does not have one
			*/
			this->SetMethod("getIcon", &MenuItem::_GetIcon);
		}

		if (this->type == CHECK)
		{
			/**
			* @tiapi(method=True,name=UI.MenuItem.setState,since=0.6)
			* @tiapi Sets the state of this check item
			* @tiapi This method is only available for check items.
			* @tiarg[Boolean, state] A True state is checked, while a False state in unchecked
			*/
			this->SetMethod("setState", &MenuItem::_SetState);

			/**
			* @tiapi(method=True,name=UI.MenuItem.getState,since=0.6)
			* @tiapi Gets the state of this check item
			* @tiapi This method is only available for check items.
			* @tiresult[Boolean] True if this item is checked, false otherwise
			*/
			this->SetMethod("getState", &MenuItem::_GetState);

			/**
			* @tiapi(method=True,name=UI.MenuItem.setAutoCheck,since=0.6) 
			* @tiapi Set whether or not this check item is an autocheck item. An autocheck
			* @tiapi item (the default) will automatically flip the state of the check on a
			* @tiapi a click event. Turning off this behavior makes the check item more useful
			* @tiapi as a radio button.
			* @tiapi This method is only available for check items.
			* @tiarg[Boolean, autocheck] Whether or not this item should be an autocheck
			*/
			this->SetMethod("setAutoCheck", &MenuItem::_SetAutoCheck);

			/**
			* @tiapi(method=True,name=UI.MenuItem.getAutoCheck,since=0.6) 
			* @tiapi This method is only available for check items.
			* @tiarg[Boolean, autocheck] Whether or not this item should is an autocheck
			*/
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
