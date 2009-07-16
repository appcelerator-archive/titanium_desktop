/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#include <kroll/kroll.h>
#include "ui_module.h"
namespace ti
{
	using std::vector;
	using std::string;

	Menu::Menu() : StaticBoundObject("UI.Menu")
	{

		/**
		 * @tiapi(method=True,name=UI.Menu.appendItem,version=1.0) Append a MenuItem object to a menu.
		 * @tiarg[UI.MenuItem, menuItem] The menuitem to append to the menu
		 */
		this->SetMethod("appendItem", &Menu::_AppendItem);

		/**
		* @tiapi(method=True,name=UI.Menu.getItemAt,version=1.0) retrieve a MenuItem from the specified index.  If the index is out of range a value exception is thrown.
		* @tiarg[Integer, index] The index for this menu item
		* @tiresult[UI.MenuItem|null] The MenuItem object at the specified index.  
		*/
		this->SetMethod("getItemAt", &Menu::_GetItemAt);

		/**
		* @tiapi(method=True,name=UI.Menu.insertItemAt,version=1.0) retrieve a MenuItem from the specified index.  If the index is out of range a value exception is thrown.
		* @tiarg[Integer, index] The index for this menu item
		* @tiresult[UI.MenuItem|null] The MenuItem object at the specified index.  
		*/
		this->SetMethod("insertItemAt", &Menu::_InsertItemAt);

		/**
		* @tiapi(method=True,name=UI.Menu.removeItemAt,version=1.0) remove a MenuItem at the specified index.  If the index is out of range a value exception is thrown.
		* @tiarg[Integer, index] The index for the menu item to remove
		*/
		this->SetMethod("removeItemAt", &Menu::_RemoveItemAt);

		/**
		* @tiapi(method=True,name=UI.Menu.getLength,version=1.0) returns the number of MenuItems in the menu.
		* @tiresult[Integer] the number of MenuItem elements in the Menu.  
		*/
		this->SetMethod("getLength", &Menu::_GetLength);

		/**
		* @tiapi(method=True,name=UI.Menu.clear,version=1.0) Clear all MenuItems from the menu
		*/
		this->SetMethod("clear", &Menu::_Clear);

		/**
		* @tiapi(method=True,name=UI.Menu.addSubmenu,version=1.0) Add a subMenu to the current menu
		* @tiarg[String, label] The label for this menu item
		* @tiarg[Function, eventListener, optional=True] An event listener for this menu item
		* @tiarg[String, iconURL, optional=True] A URL to an icon to use for this menu item
		* @tiresult[UI.MenuItem|null] The new subMenu as a MenuItem object
		*/
		this->SetMethod("addSubmenu", &Menu::_AddSubmenu);

		/**
		* @tiapi(method=True,name=UI.Menu.addItem,version=1.0) retrieve a MenuItem from the specified index.  If the index is out of range a value exception is thrown.
		* @tiarg[UI.MenuItem, item] The new MenuItem to add.
		* @tiresult[UI.MenuItem|null] The new MenuItem object
		*/
		this->SetMethod("addItem", &Menu::_AddItem);

		/**
		* @tiapi(method=True,name=UI.Menu.addSeparatorItem,version=1.0) Add a menu separator to the menu
		* @tiarg[UI.MenuItem, item] The new separator MenuItem to add.
		* @tiresult[UI.MenuItem|null] The separator as a MenuItem object
		*/
		this->SetMethod("addSeparatorItem", &Menu::_AddSeparatorItem);

		/**
		* @tiapi(method=True,name=UI.addCheckItem,version=1.0) Create a new MenuItem object
		* @tiarg[UI.MenuItem, item] The new check MenuItem to add.
		* @tiresult[UI.MenuItem] The new MenuItem object
		*/
		this->SetMethod("addCheckItem", &Menu::_AddCheckItem);
	}

	Menu::~Menu()
	{
		this->children.clear();
	}

	void Menu::_AddSubmenu(const ValueList& args, SharedValue result)
	{
		UIBinding* binding = UIBinding::GetInstance();
		AutoMenuItem newItem = binding->__CreateMenuItem(args);
		newItem->EnsureHasSubmenu();
		this->AppendItem(newItem);
		result->SetObject(newItem);
	}

	void Menu::_AddItem(const ValueList& args, SharedValue result)
	{
		args.VerifyException("addItem", "?s m|0 s|0");
		UIBinding* binding = UIBinding::GetInstance();

		// Create a menu item object and add it to this item's submenu
		AutoMenuItem newItem = binding->__CreateMenuItem(args);
		this->AppendItem(newItem);
		result->SetObject(newItem);
	}

	void Menu::_AddSeparatorItem(const ValueList& args, SharedValue result)
	{
		UIBinding* binding = UIBinding::GetInstance();
		AutoMenuItem newItem = binding->__CreateSeparatorMenuItem(args);
		this->AppendItem(newItem);
		result->SetObject(newItem);
	}

	void Menu::_AddCheckItem(const ValueList& args, SharedValue result)
	{
		UIBinding* binding = UIBinding::GetInstance();
		AutoMenuItem newItem = binding->__CreateCheckMenuItem(args);
		this->AppendItem(newItem);
		result->SetObject(newItem);
	}

	void Menu::_AppendItem(const ValueList& args, SharedValue result)
	{
		args.VerifyException("appendItem", "o");
		SharedKObject o = args.at(0)->ToObject();

		AutoMenuItem item = o.cast<MenuItem>();
		if (!item.isNull())
		{
			if (item->ContainsSubmenu(this))
				throw ValueException::FromString("Tried to construct a recursive menu");
			this->AppendItem(item);
		}
	}

	void Menu::_GetItemAt(const ValueList& args, SharedValue result)
	{
		args.VerifyException("getItemAt", "i");
		AutoMenuItem item = this->GetItemAt(args.GetInt(0));
		result->SetObject(item);
	}

	void Menu::_InsertItemAt(const ValueList& args, SharedValue result)
	{
		args.VerifyException("insertItemAt", "o,i");
		SharedKObject o = args.at(0)->ToObject();
		AutoMenuItem item = o.cast<MenuItem>();

		if (!item.isNull())
		{
			if (item->ContainsSubmenu(this))
				throw ValueException::FromString("Tried to construct a recursive menu");
			size_t index = static_cast<size_t>(args.GetInt(1));
			this->InsertItemAt(item, index);
		}
	}

	void Menu::_RemoveItemAt(const ValueList& args, SharedValue result)
	{
		args.VerifyException("removeItemAt", "i");
		size_t index = static_cast<size_t>(args.GetInt(0));

		this->RemoveItemAt(index);
	}

	void Menu::_GetLength(const ValueList& args, SharedValue result)
	{
		result->SetInt(this->children.size());
	}

	void Menu::_Clear(const ValueList& args, SharedValue result)
	{
		this->children.clear();
		this->ClearImpl();
	}

	void Menu::AppendItem(AutoMenuItem item)
	{
		if (!item.isNull())
		{
			this->children.push_back(item);
			this->AppendItemImpl(item);
		}
	}

	AutoMenuItem Menu::GetItemAt(int index)
	{
		if (index >= 0 && (size_t) index < this->children.size()) {
			return this->children[index];
		} else {
			throw ValueException::FromFormat("Index %i is out of range", index);
		}
	}

	void Menu::InsertItemAt(AutoMenuItem item, size_t index)
	{
		if (item.isNull())
		{
			throw ValueException::FromString("Tried to insert an object that was not a MenuItem");
		}

		if (index >= 0 && index <= this->children.size())
		{
			vector<AutoMenuItem>::iterator i = this->children.begin() + index;
			this->children.insert(i, item);
			this->InsertItemAtImpl(item, index);
		}
		else
		{
			throw ValueException::FromString("Tried to insert a MenuItem at an inavlid index");
		}
	}

	void Menu::RemoveItemAt(size_t index)
	{
		if (index >= 0 && index < this->children.size())
		{
			vector<AutoMenuItem>::iterator i = this->children.begin() + index;
			this->children.erase(i);
			this->RemoveItemAtImpl(index);
		}
		else
		{
			throw ValueException::FromString("Tried to remove a MenuItem at an invalid index");
		}
	}

	bool Menu::ContainsItem(MenuItem* item)
	{
		for (size_t i = 0; i < this->children.size(); i++)
		{
			if (this->children.at(i).get() == item ||
				this->children.at(i)->ContainsItem(item))
				return true;
		}
		return false;
	}

	bool Menu::ContainsSubmenu(Menu* submenu)
	{
		for (size_t i = 0; i < this->children.size(); i++)
		{
			if (this->children.at(i)->ContainsSubmenu(submenu))
				return true;
		}
		return false;
	}
}
