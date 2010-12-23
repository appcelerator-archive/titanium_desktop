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

#ifndef MenuWin_h
#define MenuWin_h

#include "../Menu.h"

#include <vector>

namespace Titanium {

class MenuItemWin;

class MenuWin : public Menu {
public:
	MenuWin();
	~MenuWin();

	void AppendItemImpl(AutoPtr<MenuItem> item);
	void InsertItemAtImpl(AutoPtr<MenuItem> item, unsigned int index);
	void RemoveItemAtImpl(unsigned int index);
	void ClearImpl();

	void ClearNativeMenu(HMENU nativeMenu);
	void DestroyNative(HMENU nativeMenu);
	HMENU CreateNative(bool registerNative);
	HMENU CreateNativeTopLevel(bool registerNative);
	void AddChildrenToNativeMenu(HMENU nativeMenu, bool registerNative);

	static void InsertItemIntoNativeMenu(
		MenuItemWin* menuItem, HMENU nativeMenu,
		bool registerNative, int position=-1);
	static void RemoveItemAtFromNativeMenu(
		MenuItemWin* item, HMENU nativeMenu, int position);
	static void ApplyNotifyByPositionStyleToNativeMenu(HMENU nativeMenu);

private:
	std::vector<AutoPtr<MenuItem> > oldChildren;
	std::vector<HMENU> nativeMenus;
};

} // namespace Titanium

#endif
