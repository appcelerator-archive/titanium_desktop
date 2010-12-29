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

#ifndef Menu_h
#define Menu_h

#include <kroll/kroll.h>

namespace Titanium {

class MenuItem;

class Menu : public KAccessorObject {
public:
    Menu();
    ~Menu();

    void _AppendItem(const ValueList& args, KValueRef result);
    void _GetItemAt(const ValueList& args, KValueRef result);
    void _InsertItemAt(const ValueList& args, KValueRef result);
    void _RemoveItemAt(const ValueList& args, KValueRef result);
    void _GetLength(const ValueList& args, KValueRef result);
    void _Clear(const ValueList& args, KValueRef result);

    void _AddItem(const ValueList& args, KValueRef result);
    void _AddSeparatorItem(const ValueList& args, KValueRef result);
    void _AddCheckItem(const ValueList& args, KValueRef result);

    void AppendItem(AutoPtr<MenuItem> item);
    AutoPtr<MenuItem> GetItemAt(int index);
    void InsertItemAt(AutoPtr<MenuItem> item, size_t index);
    void RemoveItemAt(size_t index);
    bool ContainsItem(MenuItem* item);
    bool ContainsSubmenu(Menu* submenu);

    // Platform-specific implementation
    virtual void AppendItemImpl(AutoPtr<MenuItem> item) = 0;
    virtual void InsertItemAtImpl(AutoPtr<MenuItem> item, unsigned int index) = 0;
    virtual void RemoveItemAtImpl(unsigned int index) = 0;
    virtual void ClearImpl() = 0;

protected:
    std::vector<AutoPtr<MenuItem> > children;
};

} // namespace Titanium

#endif
