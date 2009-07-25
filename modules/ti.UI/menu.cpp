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

	Menu::Menu() : AccessorBoundObject("UI.Menu")
	{

		/**
		 * @tiapi(method=True,name=UI.Menu.appendItem,version=1.0)
		 * @tiapi Append a MenuItem object to a menu.
		 * @tiarg[UI.MenuItem, item] Append an item to this menu
		 */
		this->SetMethod("appendItem", &Menu::_AppendItem);

		/**
		* @tiapi(method=True,name=UI.Menu.getItemAt,version=1.0)
		* @tiapi Get an item from this menu at the given index. This method 
		* @tiapi will throw an exception if the index is out of range.
		* @tiarg[Number, index] The index of the item to get
		* @tiresult[UI.MenuItem] the item at the given index
		*/
		this->SetMethod("getItemAt", &Menu::_GetItemAt);

		/**
		* @tiapi(method=True,name=UI.Menu.insertItemAt,version=1.0) 
		* @tiapi Insert a menu item before the given index. This method
		* @tiapi will throw an exception if the index of out of range.
		* @tiarg[Number, index] The index for this menu item
		*/
		this->SetMethod("insertItemAt", &Menu::_InsertItemAt);

		/**
		* @tiapi(method=True,name=UI.Menu.removeItemAt,version=1.0) 
		* @tiapi Remove the item in this menu at the given index. This method
		* @tiapi will throw an exception if the index is out of range.
		* @tiarg[Number, index] The index of the item to remove
		*/
		this->SetMethod("removeItemAt", &Menu::_RemoveItemAt);

		/**
		* @tiapi(method=True,name=UI.Menu.getLength,version=1.0) 
		* @tiapi Get the length of this menu.
		* @tiresult[Number] The number of items in this menu
		*/
		this->SetMethod("getLength", &Menu::_GetLength);

		/**
		* @tiapi(method=True,name=UI.Menu.clear,version=1.0)
		* @tiapi Remove all items from this menu.
		*/
		this->SetMethod("clear", &Menu::_Clear);

		/**
		* @tiapi(method=True,name=UI.Menu.addItem,version=1.0)
		* @tiapi Add an item to this menu with the given attributes.
		* @tiarg[String, label] The label for the new item
		* @tiarg[Function, listener, optional=True] An event listener callback for the item
		* @tiarg[String, iconURL] The URL for this item's icon
		* @tiresult[UI.MenuItem] The newly added item
		*/
		this->SetMethod("addItem", &Menu::_AddItem);

		/**
		* @tiapi(method=True,name=UI.Menu.addSeparatorItem,version=1.0)
		* @tiapi Add a separator item to this menu.
		* @tiresult[UI.MenuItem] The newly added separator item
		*/
		this->SetMethod("addSeparatorItem", &Menu::_AddSeparatorItem);

		/**
		* @tiapi(method=True,name=UI.Menu.addItem,version=1.0)
		* @tiapi Add a check item to this menu with the given attributes.
		* @tiarg[String, label] The label for the new item
		* @tiarg[Function, listener, optional=True] An event listener callback for the item
		* @tiresult[UI.MenuItem] The newly added check item
		*/
		this->SetMethod("addCheckItem", &Menu::_AddCheckItem);
	}

	Menu::~Menu()
	{
		this->children.clear();
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
