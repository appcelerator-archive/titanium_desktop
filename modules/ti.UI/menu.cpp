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

	Menu::Menu() :
		KAccessorObject("UI.Menu")
	{
		this->SetMethod("appendItem", &Menu::_AppendItem);
		this->SetMethod("getItemAt", &Menu::_GetItemAt);
		this->SetMethod("insertItemAt", &Menu::_InsertItemAt);
		this->SetMethod("removeItemAt", &Menu::_RemoveItemAt);
		this->SetMethod("getLength", &Menu::_GetLength);
		this->SetMethod("clear", &Menu::_Clear);
		this->SetMethod("addItem", &Menu::_AddItem);
		this->SetMethod("addSeparatorItem", &Menu::_AddSeparatorItem);
		this->SetMethod("addCheckItem", &Menu::_AddCheckItem);
	}

	Menu::~Menu()
	{
		this->children.clear();
	}

	void Menu::_AddItem(const ValueList& args, KValueRef result)
	{
		args.VerifyException("addItem", "?s m|0 s|0");
		UIBinding* binding = UIBinding::GetInstance();

		// Create a menu item object and add it to this item's submenu
		AutoMenuItem newItem = binding->__CreateMenuItem(args);
		this->AppendItem(newItem);
		result->SetObject(newItem);
	}

	void Menu::_AddSeparatorItem(const ValueList& args, KValueRef result)
	{
		UIBinding* binding = UIBinding::GetInstance();
		AutoMenuItem newItem = binding->__CreateSeparatorMenuItem(args);
		this->AppendItem(newItem);
		result->SetObject(newItem);
	}

	void Menu::_AddCheckItem(const ValueList& args, KValueRef result)
	{
		UIBinding* binding = UIBinding::GetInstance();
		AutoMenuItem newItem = binding->__CreateCheckMenuItem(args);
		this->AppendItem(newItem);
		result->SetObject(newItem);
	}

	void Menu::_AppendItem(const ValueList& args, KValueRef result)
	{
		args.VerifyException("appendItem", "o");
		KObjectRef o = args.at(0)->ToObject();

		AutoMenuItem item = o.cast<MenuItem>();
		if (!item.isNull())
		{
			if (item->ContainsSubmenu(this))
				throw ValueException::FromString("Tried to construct a recursive menu");
			this->AppendItem(item);
		}
	}

	void Menu::_GetItemAt(const ValueList& args, KValueRef result)
	{
		args.VerifyException("getItemAt", "i");
		AutoMenuItem item = this->GetItemAt(args.GetInt(0));
		result->SetObject(item);
	}

	void Menu::_InsertItemAt(const ValueList& args, KValueRef result)
	{
		args.VerifyException("insertItemAt", "o,i");
		KObjectRef o = args.at(0)->ToObject();
		AutoMenuItem item = o.cast<MenuItem>();

		if (!item.isNull())
		{
			if (item->ContainsSubmenu(this))
				throw ValueException::FromString("Tried to construct a recursive menu");
			size_t index = static_cast<size_t>(args.GetInt(1));
			this->InsertItemAt(item, index);
		}
	}

	void Menu::_RemoveItemAt(const ValueList& args, KValueRef result)
	{
		args.VerifyException("removeItemAt", "i");
		size_t index = static_cast<size_t>(args.GetInt(0));

		this->RemoveItemAt(index);
	}

	void Menu::_GetLength(const ValueList& args, KValueRef result)
	{
		result->SetInt(this->children.size());
	}

	void Menu::_Clear(const ValueList& args, KValueRef result)
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
