/**
* Appcelerator Titanium - licensed under the Apache Public License 2
* see LICENSE in the root folder for details on the license.
* Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
*/
#ifndef _MENU_ITEM_H_
#define _MENU_ITEM_H_
#include <kroll/kroll.h>
namespace ti
{
	class MenuItem : public KEventObject
	{
	public:
		enum MenuItemType
		{
			NORMAL,
			SEPARATOR,
			CHECK
		};

		MenuItem(MenuItemType type);
		~MenuItem();

		void _IsSeparator(const ValueList& args, KValueRef result);
		void _IsCheck(const ValueList& args, KValueRef result);
		void _SetLabel(const ValueList& args, KValueRef result);
		void _GetLabel(const ValueList& args, KValueRef result);
		void _SetIcon(const ValueList& args, KValueRef result);
		void _GetIcon(const ValueList& args, KValueRef result);
		void _SetState(const ValueList& args, KValueRef result);
		void _GetState(const ValueList& args, KValueRef result);
		void _SetAutoCheck(const ValueList& args, KValueRef result);
		void _IsAutoCheck(const ValueList& args, KValueRef result);
		void _SetSubmenu(const ValueList& args, KValueRef result);
		void _GetSubmenu(const ValueList& args, KValueRef result);
		void _Enable(const ValueList& args, KValueRef result);
		void _Disable(const ValueList& args, KValueRef result);
		void _IsEnabled(const ValueList& args, KValueRef result);
		void _Click(const ValueList& args, KValueRef result);

		void _AddItem(const ValueList& args, KValueRef result);
		void _AddSeparatorItem(const ValueList& args, KValueRef result);
		void _AddCheckItem(const ValueList& args, KValueRef result);

		void SetLabel(string& label);
		std::string& GetLabel();
		void SetIcon(string& iconURL);
		bool GetState();
		void SetState(bool);
		bool IsSeparator();
		bool IsCheck();
		bool IsEnabled();
		virtual void HandleClickEvent(KObjectRef source);
		void EnsureHasSubmenu();
		bool ContainsItem(MenuItem* item);
		bool ContainsSubmenu(Menu* submenu);

		// Platform-specific implementation
		virtual void SetLabelImpl(std::string newLabel) = 0;
		virtual void SetIconImpl(std::string newIconPath) = 0;
		virtual void SetStateImpl(bool newState) = 0;
		virtual void SetSubmenuImpl(AutoMenu newSubmenu) = 0;
		virtual void SetEnabledImpl(bool enabled) = 0;

	protected:
		MenuItemType type;
		bool enabled;
		std::string label;
		std::string iconURL;
		std::string iconPath;
		KMethodRef callback;
		AutoMenu submenu;
		std::vector<KMethodRef> eventListeners;
		bool state;
		bool autoCheck;
	};
}
#endif