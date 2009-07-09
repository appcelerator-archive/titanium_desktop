/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */


#ifndef _GTK_UI_BINDING_H_
#define _GTK_UI_BINDING_H_

#include "../ui_module.h"

namespace ti
{
	class GtkUIBinding : public UIBinding
	{

		public:
		GtkUIBinding(Host* host);

		SharedUserWindow CreateWindow(WindowConfig*, SharedUserWindow& parent);
		void ErrorDialog(std::string);

		SharedMenu CreateMenu();
		SharedMenuItem CreateMenuItem();
		SharedMenuItem CreateSeparatorMenuItem();
		SharedMenuItem CreateCheckMenuItem();
		void SetMenu(SharedMenu newMenu);
		void SetContextMenu(SharedMenu);
		void SetIcon(std::string&);
		SharedTrayItem AddTray(std::string&, SharedKMethod cb);
		long GetIdleTime();

		SharedMenu GetMenu();
		SharedMenu GetContextMenu();
		std::string& GetIcon();

		protected:
		SharedPtr<ScriptEvaluator> evaluator;
		SharedPtr<GtkMenu> menu;
		SharedPtr<GtkMenu> contextMenu;
		std::string iconPath;
	};
}

#endif
