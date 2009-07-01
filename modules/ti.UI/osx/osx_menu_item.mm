/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#include "../ui_module.h"
namespace ti
{
	OSXMenuItem::OSXMenuItem(
		MenuItemType type, std::string label,
		SharedKMethod callback, std::string iconURL) :
			MenuItem(type, label, callback, iconURL)
	{
	}

	OSXMenuItem::~OSXMenuItem()
	{
	}

	void OSXMenuItem::SetLabelImpl(std::string newLabel)
	{
		if (this->type == SEPARATOR)
			return;
		this->UpdateNativeMenuItems();
	}

	void OSXMenuItem::SetIconImpl(std::string newIconPath)
	{
		if (this->type == SEPARATOR || this->type == CHECK)
			return;
		this->UpdateNativeMenuItems();
	}

	void OSXMenuItem::SetSubmenuImpl(SharedMenu newSubmenu)
	{
		if (this->type == SEPARATOR)
			return;
		this->UpdateNativeMenuItems();
	}

	void OSXMenuItem::EnableImpl()
	{
		if (this->type == SEPARATOR)
			return;
		this->UpdateNativeMenuItems();
	}

	void OSXMenuItem::DisableImpl()
	{
		if (this->type == SEPARATOR)
			return;
		this->UpdateNativeMenuItems();
	}

	bool OSXMenuItem::HasSubmenu()
	{
		return !this->submenu.isNull();
	}

	void OSXMenuItem::SetNSMenuItemTitle(NSMenuItem* item, std::string& title)
	{
		NSString* nstitle = [NSString stringWithUTF8String:title.c_str()];
		[item setTitle:nstitle];

		NSMenu* submenu = [item submenu];
		if (submenu != nil)
		{
			// Need to set the new native menu's title as this item's label. Each
			// native menu will have to use the title of the item it is attached to.
			[submenu setTitle:nstitle];
		}
		if ([item menu] != nil)
		{
			[[item menu] sizeToFit];
		}
	}

	void OSXMenuItem::SetNSMenuItemIconPath(
		NSMenuItem* item,
		std::string& iconPath,
		NSImage* image)
	{
		bool needsRelease = false;

		// If we weren't passed an image, create one for this call. This allows callers to do
		// one image creation in cases where the same image is used over and over again --
		// see SetIconImpl(...)
		if (image == nil)
		{
			NSString *path = [NSString stringWithCString:(iconPath.c_str())];
			image = [[NSImage alloc] initWithContentsOfFile:path];
			needsRelease = true;
		}

		if (!iconPath.empty())
		{
			[item setImage:image];
		}
		else
		{
			[item setImage:nil];
		}
		if ([item menu] != nil)
		{
			[[item menu] sizeToFit];
		}

		if (needsRelease)
		{
			[image release];
		}
	}

	void OSXMenuItem::SetNSMenuItemSubmenu(
		NSMenuItem* item,
		SharedMenu submenu,
		bool registerNative)
	{
		if (!submenu.isNull())
		{
			SharedPtr<OSXMenu> osxSubmenu = submenu.cast<OSXMenu>();
			NSMenu* nativeMenu = osxSubmenu->CreateNative(registerNative);
			[nativeMenu setTitle:[item title]];
			[item setSubmenu:nativeMenu];
		}
		else
		{
			[item setSubmenu:nil];
		}
	}

	void OSXMenuItem::SetNSMenuItemEnabled(NSMenuItem* item, bool enabled)
	{
		[item setEnabled:(enabled ? YES : NO)];
	}

	NSMenuItem* OSXMenuItem::CreateNative(bool registerNative)
	{
		if (this->IsSeparator())
		{
			return [NSMenuItem separatorItem];
		}
		else
		{
			NSMenuItem* item = [[NSMenuItem alloc] 
				initWithTitle:@"Temp" action:@selector(invoke:) keyEquivalent:@""];

			OSXMenuItemDelegate* delegate = [[OSXMenuItemDelegate alloc] initWithMenuItem:this];
			[item setTarget:delegate];

			SetNSMenuItemTitle(item, this->label);
			SetNSMenuItemIconPath(item, this->iconPath);
			SetNSMenuItemEnabled(item, this->enabled);
			SetNSMenuItemSubmenu(item, this->submenu, registerNative);

			if (registerNative)
			{
				this->realizations.push_back(item);
			}

			return item;
		}
	}

	void OSXMenuItem::DestroyNative(NSMenuItem* realization)
	{
		std::vector<NSMenuItem*>::iterator i = this->realizations.begin();
		while (i != this->realizations.end())
		{
			NSMenuItem* item = *i;
			if (item == realization)
			{
				i = this->realizations.erase(i);
				if (!this->submenu.isNull() && [item submenu] != nil)
				{
					SharedPtr<OSXMenu> osxSubmenu = this->submenu.cast<OSXMenu>();
					osxSubmenu->DestroyNative([item submenu]);
				}
				[item release];
			}
			else
			{
				i++;
			}
		}
	}

	void OSXMenuItem::UpdateNativeMenuItems()
	{
		std::vector<NSMenuItem*>::iterator i = this->realizations.begin();
		while (i != this->realizations.end())
		{
			NSMenuItem* nativeItem = (*i++);
			if ([nativeItem menu]) {
				OSXMenu::UpdateNativeMenu([nativeItem menu]);
			}
		}
	}
}


