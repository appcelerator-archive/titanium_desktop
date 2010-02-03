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

		AutoMenu CreateMenu();
		AutoMenuItem CreateMenuItem();
		AutoMenuItem CreateSeparatorMenuItem();
		AutoMenuItem CreateCheckMenuItem();
		void SetMenu(AutoMenu newMenu);
		void SetContextMenu(AutoMenu);
		void SetIcon(std::string&);
		AutoTrayItem AddTray(std::string&, KMethodRef cb);
		long GetIdleTime();
		AutoMenu GetMenu();
		AutoMenu GetContextMenu();
		std::string& GetIcon();

		static void ErrorDialog(std::string);

		protected:
		SharedPtr<ScriptEvaluator> evaluator;
		AutoPtr<GtkMenu> menu;
		AutoPtr<GtkMenu> contextMenu;
		std::string iconPath;
	};
}

#endif
