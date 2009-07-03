/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#include "../ui_module.h"
namespace ti
{
	OSXTrayItem::OSXTrayItem(SharedString iconPath, SharedKMethod cb) :
		nativeMenu(0),
		menu(0),
		callback(cb),
		delegate([[OSXTrayDelegate alloc] initWithTray:this])
	{
		this->SetIcon(iconPath);
	}

	OSXTrayItem::~OSXTrayItem()
	{
		if (delegate)
		{
			[delegate release];
			delegate = nil;
		}

		if (!this->menu.isNull() && this->nativeMenu) {
			this->menu->DestroyNative(this->nativeMenu);
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

	void OSXTrayItem::SetMenu(SharedMenu menu)
	{
		if (menu.get() == this->menu.get()) {
			return;
		}

		SharedPtr<OSXMenu> newMenu = menu.cast<OSXMenu>();
		NSMenu* newNativeMenu = nil;
		if (!newMenu.isNull()) {
			newNativeMenu = newMenu->CreateNativeNow(true);
		}
		[delegate setMenu:newNativeMenu];

		if (!this->menu.isNull() && this->nativeMenu) {
			this->menu->DestroyNative(this->nativeMenu);
		}
		this->menu = newMenu;
		this->nativeMenu = nativeMenu;
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
			[delegate setHint:[NSString stringWithCString:label.c_str() encoding:NSUTF8StringEncoding]];
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
