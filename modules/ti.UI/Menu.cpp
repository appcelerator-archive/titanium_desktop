/*
 * Copyright (c) 2009-2010 Appcelerator, Inc. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "Menu.h"

#include <string>
#include <vector>

#include <kroll/kroll.h>

#include "MenuItem.h"
#include "UI.h"

using namespace std;

namespace Titanium {

Menu::Menu()
    : KAccessorObject("UI.Menu")
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
    UI* binding = UI::GetInstance();

    // Create a menu item object and add it to this item's submenu
    AutoPtr<MenuItem> newItem = binding->__CreateMenuItem(args);
    this->AppendItem(newItem);
    result->SetObject(newItem);
}

void Menu::_AddSeparatorItem(const ValueList& args, KValueRef result)
{
    UI* binding = UI::GetInstance();
    AutoPtr<MenuItem> newItem = binding->__CreateSeparatorMenuItem(args);
    this->AppendItem(newItem);
    result->SetObject(newItem);
}

void Menu::_AddCheckItem(const ValueList& args, KValueRef result)
{
    UI* binding = UI::GetInstance();
    AutoPtr<MenuItem> newItem = binding->__CreateCheckMenuItem(args);
    this->AppendItem(newItem);
    result->SetObject(newItem);
}

void Menu::_AppendItem(const ValueList& args, KValueRef result)
{
    args.VerifyException("appendItem", "o");
    KObjectRef o = args.at(0)->ToObject();

    AutoPtr<MenuItem> item = o.cast<MenuItem>();
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
    AutoPtr<MenuItem> item = this->GetItemAt(args.GetInt(0));
    result->SetObject(item);
}

void Menu::_InsertItemAt(const ValueList& args, KValueRef result)
{
    args.VerifyException("insertItemAt", "o,i");
    KObjectRef o = args.at(0)->ToObject();
    AutoPtr<MenuItem> item = o.cast<MenuItem>();

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

void Menu::AppendItem(AutoPtr<MenuItem> item)
{
    if (!item.isNull())
    {
        this->children.push_back(item);
        this->AppendItemImpl(item);
    }
}

AutoPtr<MenuItem> Menu::GetItemAt(int index)
{
    if (index >= 0 && (size_t) index < this->children.size()) {
        return this->children[index];
    } else {
        throw ValueException::FromFormat("Index %i is out of range", index);
    }
}

void Menu::InsertItemAt(AutoPtr<MenuItem> item, size_t index)
{
    if (item.isNull())
    {
        throw ValueException::FromString("Tried to insert an object that was not a MenuItem");
    }

    if (index >= 0 && index <= this->children.size())
    {
        vector<AutoPtr<MenuItem> >::iterator i = this->children.begin() + index;
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
        vector<AutoPtr<MenuItem> >::iterator i = this->children.begin() + index;
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

} // namespace Titanium
