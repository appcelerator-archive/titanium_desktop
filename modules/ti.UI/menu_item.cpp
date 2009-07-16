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
			this->SetMethod("addSubmenu", &MenuItem::_AddSubmenu);
			this->SetMethod("addItem", &MenuItem::_AddItem);
			this->SetMethod("addSeparatorItem", &MenuItem::_AddSeparatorItem);
			this->SetMethod("addCheckItem", &MenuItem::_AddCheckItem);
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

	/**
	* @tiapi(method=True,name=UI.MenuItem.isSeparator,version=1.0) Is this a seperator MenuItem?
	* @tiresult[Boolean] returns true if the menu item is a seperator, false otherwise.
	*/
	void MenuItem::_IsSeparator(const ValueList& args, SharedValue result)
	{
		result->SetBool(this->type == SEPARATOR);
	}

	/**
	* @tiapi(method=True,name=UI.MenuItem.isCheck,version=1.0) check to see if the MenuItem has the Check style enabled.
	* @tiresult[Boolean] returns true if the menu item is a checked MenuItem type, false otherwise.
	*/
	void MenuItem::_IsCheck(const ValueList& args, SharedValue result)
	{
		result->SetBool(this->type == CHECK);
	}

	/**
	* @tiapi(method=True,name=UI.MenuItem.setLabel,version=1.0) 
	* @tiarg[String, label] The label for this MenuItem
	*/
	void MenuItem::_SetLabel(const ValueList& args, SharedValue result)
	{
		args.VerifyException("setLabel", "s|0");
		string newLabel = args.GetString(0, "");
		this->SetLabel(newLabel);
	}

	/**
	* @tiapi(method=True,name=UI.MenuItem.getLabel,version=1.0) returns the string label for this MenuItem 
	* @tiresult[String] the MenuItem label as a string.
	*/
	void MenuItem::_GetLabel(const ValueList& args, SharedValue result)
	{
		result->SetString(this->label);
	}

	/**
	* @tiapi(method=True,name=UI.MenuItem.setIcon,version=1.0) 
	* @tiarg[String, iconURL] A URL to an icon to use for this menu item
	*/
	void MenuItem::_SetIcon(const ValueList& args, SharedValue result)
	{
		args.VerifyException("setIcon", "s|0");
		std::string newIcon = "";
		if (args.size() > 0) {
			newIcon = args.GetString(0);
		}
		this->SetIcon(newIcon);
	}

	/**
	* @tiapi(method=True,name=UI.MenuItem.getIcon,version=1.0) retrieve the icon URL path for this MenuItem
	* @tiresult[String] The URL path to the MenuItem icon.
	*/
	void MenuItem::_GetIcon(const ValueList& args, SharedValue result)
	{
		result->SetString(this->iconURL);
	}

	/**
	* @tiapi(method=True,name=UI.MenuItem.setState,version=1.0) Sets the checked state of the MenuItem
	* @tiarg[Boolean, state] the new checked state for the MenuItem.
	*/
	void MenuItem::_SetState(const ValueList& args, SharedValue result)
	{
		args.VerifyException("setState", "b");
		this->SetState(args.GetBool(0));
	}

	/**
	* @tiapi(method=True,name=UI.MenuItem.getState,version=1.0) Gets the checked state of the MenuItem
	*/
	void MenuItem::_GetState(const ValueList& args, SharedValue result)
	{
		result->SetBool(this->state);
	}

	/**
	* @tiapi(method=True,name=UI.MenuItem.setSubMenu,version=1.0) Add a subMenu to this MenuItem.
	* @tiarg[UI.Menu, menu] the new subMenu to add.  An exception will be thrown if you attempt to add a recursive menu.
	*/
	void MenuItem::_SetSubmenu(const ValueList& args, SharedValue result)
	{
		args.VerifyException("setCallback", "o|0");
		AutoMenu newSubmenu = NULL;

		if (args.at(0)->IsObject())
		{
			SharedKObject o = args.at(0)->ToObject();
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

	/**
	* @tiapi(method=True,name=UI.MenuItem.getSubMenu,version=1.0) Get the subMenu for this MenuItem.
	* @tiresult[UI.Menu|null] The subMenu as a Menu object
	*/
	void MenuItem::_GetSubmenu(const ValueList& args, SharedValue result)
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

	/**
	* @tiapi(method=True,name=UI.MenuItem.enable,version=1.0) Enable this MenuItem
	*/
	void MenuItem::_Enable(const ValueList& args, SharedValue result)
	{
		this->enabled = true;
		this->SetEnabledImpl(true);
	}

	/**
	* @tiapi(method=True,name=UI.MenuItem.disable,version=1.0) Disable this MenuItem.
	*/
	void MenuItem::_Disable(const ValueList& args, SharedValue result)
	{
		this->enabled = false;
		this->SetEnabledImpl(true);
	}

	/**
	* @tiapi(method=True,name=UI.MenuItem.setAutoCheck,version=1.0) 
	* @tiarg[Boolean, autocheck] The new auto check state
	*/
	void MenuItem::_SetAutoCheck(const ValueList& args, SharedValue result)
	{
		args.VerifyException("setAutoCheck", "b");
		this->autoCheck = args.GetBool(0);
	}

	/**
	* @tiapi(method=True,name=UI.MenuItem.isAutoCheck,version=1.0) 
	* @tiresult[Boolean] returns true if the MenuItem has the AutoCheck style enabled.
	*/
	void MenuItem::_IsAutoCheck(const ValueList& args, SharedValue result)
	{
		result->SetBool(this->autoCheck);
	}

	/**
	* @tiapi(method=True,name=UI.MenuItem.isEnabled,version=1.0) 
	* @tiresult[Boolean] returns true if the MenuItem is enabled, false otherwise.
	*/
	void MenuItem::_IsEnabled(const ValueList& args, SharedValue result)
	{
		result->SetBool(this->enabled);
	}

	/**
	* @tiapi(method=True,name=UI.MenuItem.click,version=1.0) Handle a MenuItem click event
	*/
	void MenuItem::_Click(const ValueList& args, SharedValue result)
	{
		this->HandleClickEvent(0);
	}

	/**
	* @tiapi(method=True,name=UI.MenuItem.addSubMenu,version=1.0) Add a new subMenu to the MenuItem
	* @tiarg[String, label] The label for this subMenu
	* @tiarg[Function, eventListener] An event listener for this menu item
	* @tiarg[String, iconURL] A URL to an icon to use for this menu item
	* @tiresult[UI.MenuItem|null] The new subMenu as a MenuItem object
	*/
	void MenuItem::_AddSubmenu(const ValueList& args, SharedValue result)
	{
		UIBinding* binding = UIBinding::GetInstance();

		AutoMenuItem newItem = binding->__CreateMenuItem(args);
		newItem->EnsureHasSubmenu();
		this->EnsureHasSubmenu();
		this->submenu->AppendItem(newItem);

		result->SetObject(newItem);
	}

	/**
	* @tiapi(method=True,name=UI.MenuItem.addItem,version=1.0) add a new MenuItem to the MenuItem's subMenu.	
	* @tiarg[UI.MenuItem, item] The new MenuItem to add.
	* @tiresult[UI.MenuItem|null] The new MenuItem object
	*/
	void MenuItem::_AddItem(const ValueList& args, SharedValue result)
	{
		args.VerifyException("addItem", "?s m|0 s|0");
		UIBinding* binding = UIBinding::GetInstance();

		// Create a menu item object and add it to this item's submenu
		AutoMenuItem newItem = binding->__CreateMenuItem(args);
		this->EnsureHasSubmenu();
		this->submenu->AppendItem(newItem);

		result->SetObject(newItem);
	}

	/**
	* @tiapi(method=True,name=UI.MenuItem.addSeparatorItem,version=1.0) Add a new seperator to the subMenu for the MenuItem.
	* @tiarg[UI.MenuItem, item] The new separator MenuItem to add.
	* @tiresult[UI.MenuItem|null] The new MenuItem object
	*/
	void MenuItem::_AddSeparatorItem(const ValueList& args, SharedValue result)
	{
		UIBinding* binding = UIBinding::GetInstance();
		AutoMenuItem newItem = binding->__CreateSeparatorMenuItem(args);
		this->EnsureHasSubmenu();
		this->submenu->AppendItem(newItem);

		result->SetObject(newItem);
	}

	/**
	* @tiapi(method=True,name=UI.MenuItem.addCheckItem,version=1.0) Add a check MenuItem to the menuItems submenu.
	* @tiarg[UI.MenuItem, item] The new checked MenuItem to add.
	* @tiresult[UI.MenuItem|null] The new subMenu as a MenuItem object
	*/
	void MenuItem::_AddCheckItem(const ValueList& args, SharedValue result)
	{
		UIBinding* binding = UIBinding::GetInstance();

		// Create a menu item object
		AutoMenuItem newItem = binding->__CreateCheckMenuItem(args);
		this->EnsureHasSubmenu();
		this->submenu->AppendItem(newItem);

		result->SetObject(newItem);
	}

	void MenuItem::HandleClickEvent(SharedKObject source)
	{
		if (this->IsCheck() && this->autoCheck)
		{
			// Execute this later on the main thread
			Host* host = Host::GetInstance();
			host->InvokeMethodOnMainThread(
				this->Get("setState")->ToMethod(),
				ValueList(Value::NewBool(!this->GetState())),
				false);
		}

		this->FireEvent(Event::CLICKED);
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
			this->iconPath = URLToPathOrURL(this->iconURL);
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
