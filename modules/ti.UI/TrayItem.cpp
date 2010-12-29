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

#include "TrayItem.h"

#include "Menu.h"
#include "UI.h"

namespace Titanium {

TrayItem::TrayItem(std::string& iconURL)
    : KEventObject("UI.TrayItem")
    , iconURL(iconURL)
    , iconPath(URLUtils::URLToPath(iconURL))
    , removed(false)
{
    this->SetMethod("setIcon", &TrayItem::_SetIcon);
    this->SetMethod("getIcon", &TrayItem::_GetIcon);
    this->SetMethod("setMenu", &TrayItem::_SetMenu);
    this->SetMethod("getMenu", &TrayItem::_GetMenu);
    this->SetMethod("setHint", &TrayItem::_SetHint);
    this->SetMethod("getHint", &TrayItem::_GetHint);
    this->SetMethod("remove", &TrayItem::_Remove);
}

TrayItem::~TrayItem()
{
}

void TrayItem::_SetIcon(const ValueList& args, KValueRef result)
{
    args.VerifyException("setIcon", "s");

    this->iconPath = this->iconURL = "";
    if (args.size() > 0) {
        this->iconURL = args.GetString(0);
        this->iconPath = URLUtils::URLToPath(iconURL);
    }

    if (!removed)
        this->SetIcon(this->iconPath);
}

void TrayItem::_GetIcon(const ValueList& args, KValueRef result)
{
    result->SetString(this->iconURL);
}

void TrayItem::_SetMenu(const ValueList& args, KValueRef result)
{
    args.VerifyException("setMenu", "o|0");
    AutoPtr<Menu> menu(0); // A NULL value is an unset
    if (args.size() > 0 && args.at(0)->IsObject())
    {
        menu = args.at(0)->ToObject().cast<Menu>();
    }

    if (!removed)
        this->SetMenu(menu);

    this->menu = menu;
}

void TrayItem::_GetMenu(const ValueList& args, KValueRef result)
{
    result->SetObject(this->menu);
}

void TrayItem::_SetHint(const ValueList& args, KValueRef result)
{
    args.VerifyException("setHint", "s|0");
    this->hint = "";
    if (args.size() > 0) {
        hint = args.GetString(0);
    }

    if (!removed)
        this->SetHint(hint);
}

void TrayItem::_GetHint(const ValueList& args, KValueRef result)
{
    result->SetString(this->hint);
}

void TrayItem::_Remove(const ValueList& args, KValueRef result)
{
    if (removed)
        return;

    this->Remove();
    UI::GetInstance()->UnregisterTrayItem(this);
    removed = true;
}

} // namespace Titanium
