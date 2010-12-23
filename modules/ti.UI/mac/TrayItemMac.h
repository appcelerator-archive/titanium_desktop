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

#ifndef TrayItemMac_h
#define TrayItemMac_h

#import <AppKit/NSMenu.h>
#import <AppKit/NSStatusItem.h>

#include "../TrayItem.h"

namespace Titanium {

class MenuMac;

class TrayItemMac: public TrayItem {
public:
	TrayItemMac(std::string& iconURL, KMethodRef cb);
	virtual ~TrayItemMac();

	void SetIcon(std::string& iconPath);
	void SetMenu(AutoPtr<Menu> menu);
	void SetHint(std::string& hint);
	void Remove();
	void InvokeCallback();

private:
	NSMenu* nativeMenu;
	AutoPtr<MenuMac> menu;
	KMethodRef callback;
	NSStatusItem* nativeItem;
};

} // namespace Titanium

#endif
