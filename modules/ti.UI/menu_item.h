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
	class MenuItem : public StaticBoundObject
	{
		public:
		enum MenuItemType
		{
			NORMAL,
			SEPARATOR,
			CHECK
		};

		MenuItem(MenuItemType type, std::string label,
			SharedKMethod eventListener, std::string iconURL);
		~MenuItem();

		void _IsSeparator(const ValueList& args, SharedValue result);
		void _IsCheck(const ValueList& args, SharedValue result);
		void _SetLabel(const ValueList& args, SharedValue result);
		void _GetLabel(const ValueList& args, SharedValue result);
		void _SetIcon(const ValueList& args, SharedValue result);
		void _GetIcon(const ValueList& args, SharedValue result);
		void _SetState(const ValueList& args, SharedValue result);
		void _GetState(const ValueList& args, SharedValue result);
		void _AddEventListener(const ValueList& args, SharedValue result);
		void _RemoveEventListener(const ValueList& args, SharedValue result);
		void _GetEventListeners(const ValueList& args, SharedValue result);
		void _SetSubmenu(const ValueList& args, SharedValue result);
		void _GetSubmenu(const ValueList& args, SharedValue result);
		void _Enable(const ValueList& args, SharedValue result);
		void _Disable(const ValueList& args, SharedValue result);
		void _IsEnabled(const ValueList& args, SharedValue result);

		void _AddSubmenu(const ValueList& args, SharedValue result);
		void _AddItem(const ValueList& args, SharedValue result);
		void _AddSeparatorItem(const ValueList& args, SharedValue result);
		void _AddCheckItem(const ValueList& args, SharedValue result);

		void AddEventListener(SharedKMethod eventListener);
		std::string& GetLabel();
		std::string& GetIconPath();
		bool GetState();
		void SetState(bool);
		bool IsSeparator();
		bool IsCheck();
		bool IsEnabled();
		virtual void HandleClickEvent(SharedKObject source);
		void EnsureHasSubmenu();

		// Platform-specific implementation
		virtual void SetLabelImpl(std::string newLabel) = 0;
		virtual void SetIconImpl(std::string newIconPath) = 0;
		virtual void SetStateImpl(bool newState) = 0;
		virtual void SetSubmenuImpl(SharedMenu newSubmenu) = 0;
		virtual void EnableImpl() = 0;
		virtual void DisableImpl() = 0;

	protected:
		MenuItemType type;
		bool enabled;
		std::string label;
		std::string iconPath;
		SharedKMethod callback;
		SharedMenu submenu;
		std::vector<SharedKMethod> eventListeners;
		bool state;
	};
}
#endif
