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

	Menu::Menu() : StaticBoundObject()
	{
		this->SetMethod("appendItem", &Menu::_AppendItem);
		this->SetMethod("getItemAt", &Menu::_GetItemAt);
		this->SetMethod("insertItemAt", &Menu::_InsertItemAt);
		this->SetMethod("removeItemAt", &Menu::_RemoveItemAt);
		this->SetMethod("getLength", &Menu::_GetLength); this->SetMethod("clear", &Menu::_Clear);
	}

	Menu::~Menu()
	{
		this->children.clear();
	}

	void Menu::_AppendItem(const ValueList& args, SharedValue result)
	{
		args.VerifyException("appendItem", "o");
		SharedKObject o = args.at(0)->ToObject();

		SharedMenuItem item = o.cast<MenuItem>();
		this->AppendItem(item);
	}

	void Menu::_GetItemAt(const ValueList& args, SharedValue result)
	{
		args.VerifyException("getItemAt", "i");
		size_t index = static_cast<size_t>(args.GetInt(0));
		SharedMenuItem item = this->GetItemAt(index);
		result->SetObject(item);
	}

	void Menu::_InsertItemAt(const ValueList& args, SharedValue result)
	{
		args.VerifyException("insertItemAt", "o,i");
		SharedKObject o = args.at(0)->ToObject();
		SharedMenuItem item = o.cast<MenuItem>();
		size_t index = static_cast<size_t>(args.GetInt(1));

		this->InsertItemAt(item, index);
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
		this->ClearImpl();
		this->children.clear();
	}

	void Menu::AppendItem(SharedMenuItem item)
	{
		if (!item.isNull())
		{
			this->AppendItemImpl(item);
			this->children.push_back(item);
		}
	}

	SharedMenuItem Menu::GetItemAt(size_t index)
	{
		if (index >= 0 && index < this->children.size())
		{
			return this->children[index];
		}
		else
		{
			return 0;
		}
	}

	void Menu::InsertItemAt(SharedMenuItem item, size_t index)
	{
		if (item.isNull())
		{
			throw ValueException::FromString("Tried to insert an object that was not a MenuItem");
		}

		if (index >= 0 && index <= this->children.size())
		{
			this->InsertItemAtImpl(item, index);

			vector<SharedMenuItem>::iterator i = this->children.begin() + index;
			this->children.insert(i, item);
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
			this->RemoveItemAtImpl(index);
			vector<SharedMenuItem>::iterator i = this->children.begin() + index;
			this->children.erase(i);
		}
		else
		{
			throw ValueException::FromString("Tried to remove a MenuItem at an invalid index");
		}
	}
}
