/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#ifndef _MENU_H_
#define _MENU_H_
#include <kroll/kroll.h>
namespace ti
{
	class Menu : public KAccessorObject
	{
	public:
		// Platform-independent implementation
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

		void AppendItem(AutoMenuItem item);
		AutoMenuItem GetItemAt(int index);
		void InsertItemAt(AutoMenuItem item, size_t index);
		void RemoveItemAt(size_t index);
		bool ContainsItem(MenuItem* item);
		bool ContainsSubmenu(Menu* submenu);

		// Platform-specific implementation
		virtual void AppendItemImpl(AutoMenuItem item) = 0;
		virtual void InsertItemAtImpl(AutoMenuItem item, unsigned int index) = 0;
		virtual void RemoveItemAtImpl(unsigned int index) = 0;
		virtual void ClearImpl() = 0;

	protected:
		std::vector<AutoMenuItem> children;
	};
}
#endif
