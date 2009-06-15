/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#include "../ui_module.h"
#include "osx_menu_item.h"
#include "osx_menu_delegate.h"

namespace ti {

	OSXMenuItem::OSXMenuItem()
		 : parent(NULL), enabled(true)
	{
	}
	OSXMenuItem::~OSXMenuItem()
	{
	}

	void OSXMenuItem::SetParent(OSXMenuItem* parent)
	{
		this->parent = parent;
	}

	OSXMenuItem* OSXMenuItem::GetParent()
	{
		return this->parent;
	}

	SharedValue OSXMenuItem::AddSeparator()
	{
		OSXMenuItem* item = new OSXMenuItem();
		item->MakeSeparator();
		return this->AppendItem(item);
	}

	SharedValue OSXMenuItem::AddItem(SharedValue label,
	                              SharedValue callback,
	                              SharedValue icon_url)
	{
		OSXMenuItem* item = new OSXMenuItem();
		item->MakeItem(label, callback, icon_url);
		return this->AppendItem(item);
	}

	SharedValue OSXMenuItem::AddSubMenu(SharedValue label,
	                                        SharedValue icon_url)
	{
		OSXMenuItem* item = new OSXMenuItem();
		item->MakeSubMenu(label, icon_url);
		return this->AppendItem(item);
	}

	SharedValue OSXMenuItem::AppendItem(OSXMenuItem* item)
	{
		item->SetParent(this);
		this->children.push_back(item);
		return MenuItem::AddToListModel(item);
	}
	
	int OSXMenuItem::GetChildCount()
	{
		return this->children.size();
	}
	
	OSXMenuItem* OSXMenuItem::GetChild(int c)
	{
		return this->children.at(c);
	}
	
	bool OSXMenuItem::IsEnabled()
	{
		return this->enabled;
	}
	

	/* Crazy mutations below */
	void OSXMenuItem::Enable()
	{
		this->enabled = true;
	}

	void OSXMenuItem::Disable()
	{
		this->enabled = false;
	}

	void OSXMenuItem::SetLabel(std::string label)
	{
	}

	void OSXMenuItem::SetIcon(std::string icon_url)
	{
	}
	
	NSMenuItem* OSXMenuItem::CreateNative()
	{
		if (this->IsSeparator())
		{
			return [NSMenuItem separatorItem];
		}
		return [[OSXMenuDelegate alloc] initWithMenu:this menu:nil]; 
	}	
	
	void OSXMenuItem::AttachMenu(NSMenu *menu)
	{
		if (this->IsSeparator())
		{
			[menu addItem:[NSMenuItem separatorItem]];
		}
		else
		{
			[[[OSXMenuDelegate alloc] initWithMenu:this menu:menu] autorelease]; 
		}
	}
		
	void OSXMenuItem::Invoke()
	{
		//invoke callback
		SharedValue callback_val = this->RawGet("callback");
		if (callback_val->IsMethod())
		{
			SharedKMethod method = callback_val->ToMethod();
			try
			{
				ValueList args;
				method->Call(args);
			}
			catch(...)
			{
				std::cerr << "Menu callback failed" << std::endl;
			}
		}
	}

}

