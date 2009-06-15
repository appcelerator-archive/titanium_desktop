/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#include "osx_tray_item.h"
#include "osx_tray_delegate.h"
#include "osx_ui_binding.h"

namespace ti
{
	OSXTrayItem::OSXTrayItem(SharedString iconPath, SharedKMethod cb)
	{
		this->callback = cb;
		this->delegate = [[OSXTrayDelegate alloc] initWithTray:this];
		this->SetIcon(iconPath);
	}

	OSXTrayItem::~OSXTrayItem()
	{
		if (delegate)
		{
			[delegate release];
			delegate = nil;
		}
	}

	void OSXTrayItem::SetIcon(SharedString iconPath)
	{
		std::string path = *iconPath;
		if (path.empty())
		{
			[delegate setIcon:nil];
		}
		else
		{
			NSImage* image = ti::OSXUIBinding::MakeImage(path);
			[delegate setIcon:image];
		}
	}
	void OSXTrayItem::SetMenu(SharedPtr<MenuItem> menu)
	{
		[delegate addMenu:menu];
	}
	void OSXTrayItem::SetHint(SharedString hint)
	{
		std::string label = *hint;
		if (label.empty())
		{
			[delegate setHint:@""];
		}
		else
		{
			[delegate setHint:[NSString stringWithCString:label.c_str()]];
		}
	}
	void OSXTrayItem::Remove()
	{
		[delegate release];
		delegate = nil;
	}
	void OSXTrayItem::Invoke()
	{
		//invoke callback
		try
		{
			ValueList args;
			callback->Call(args);
		}
		catch(...)
		{
			std::cerr << "Menu callback failed" << std::endl;
		}
	}
}
