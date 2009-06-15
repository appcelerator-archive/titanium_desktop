/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#include "../ui_module.h"
#include "osx_ui_binding.h"
#include "osx_menu_item.h"
#include "osx_tray_item.h"
#include "osx_user_window.h"

@interface NSApplication (LegacyWarningSurpression)
- (id) dockTile;
@end

@interface DockTileStandin: NSObject{
	
}
- (NSSize)size;
- (void)setContentView:(NSView *)view;
- (NSView *)contentView;
- (void)display;
- (void)setShowsApplicationBadge:(BOOL)flag;
- (BOOL)showsApplicationBadge;
- (void)setBadgeLabel:(NSString *)string;
- (NSString *)badgeLabel;
- (id)owner;
@end

namespace ti
{
	OSXUIBinding::OSXUIBinding(Host *host) :
		UIBinding(host),
		scriptEvaluator(nil)
	{
		[TiProtocol registerSpecialProtocol];
		[AppProtocol registerSpecialProtocol];
		application = [[TiApplication alloc] initWithBinding:this host:host];
		NSApplication *nsapp = [NSApplication sharedApplication];
		[nsapp setDelegate:application];
		[NSBundle loadNibNamed:@"MainMenu" owner:nsapp];
		InstallMenu(NULL); // force default app menu

		// Add the custom script evaluator which will dynamically
		// dispatch unknown script types to loaded Kroll modules.
		scriptEvaluator = [[ScriptEvaluator alloc] initWithHost:host];
	}

	OSXUIBinding::~OSXUIBinding()
	{
		[scriptEvaluator release];
		[application release];
		[appDockMenu release];
		[savedDockView release];
	}

	SharedUserWindow OSXUIBinding::CreateWindow(
		WindowConfig* config,
		SharedUserWindow& parent)
	{
		UserWindow* w = new OSXUserWindow(config, parent);
		return w->GetSharedPtr();
	}

	void OSXUIBinding::ErrorDialog(std::string msg)
	{
		NSApplicationLoad();
		if (!msg.empty())
			 NSRunCriticalAlertPanel (@"Application Error", [NSString stringWithUTF8String:msg.c_str()],nil,nil,nil);
		UIBinding::ErrorDialog(msg);
	}

	SharedPtr<MenuItem> OSXUIBinding::CreateMenu(bool trayMenu)
	{
		return new OSXMenuItem();
	}

	void OSXUIBinding::SetMenu(SharedPtr<MenuItem> menu)
	{
		if (!this->activeMenu)
		{
			SharedPtr<OSXMenuItem> osx_menu = menu.cast<OSXMenuItem>();
			InstallMenu(osx_menu.get());
		}
		
		if (menu.isNull() && (!this->activeMenu || this->activeMenu == this->menu.get()))
		{
			InstallMenu(NULL);
		}
		
		this->menu = menu;
		std::cout << "SetMenu => " << menu.get() << ", active=" << activeMenu << std::endl;
	}

	void OSXUIBinding::SetContextMenu(SharedPtr<MenuItem> menu)
	{
		this->contextMenu = menu;
	}

	void OSXUIBinding::SetDockIcon(SharedString badge_path)
	{
		//TODO: Put Dock Icon support back in for 10.4.
		if (![NSApp respondsToSelector:@selector(dockTile)]){
			return;
		}
		
		DockTileStandin *dockTile = (DockTileStandin *)[NSApp dockTile];
		std::string value = *badge_path;
		if (!value.empty())
		{
			// remember the old one
			if (!savedDockView)
			{
				savedDockView = [dockTile contentView];
				[savedDockView retain];
			}
		   	// setup our image view for the dock tile
		   	NSRect frame = NSMakeRect(0, 0, dockTile.size.width, dockTile.size.height);
		   	NSImageView *dockImageView = [[NSImageView alloc] initWithFrame: frame];

			NSImage *image = MakeImage(value);
		   	[dockImageView setImage:image];
			[image release];
			
		   	// by default, add it to the NSDockTile
		   	[dockTile setContentView: dockImageView];
		}
		else if (savedDockView)
		{
		   	[dockTile setContentView:savedDockView];
			[savedDockView release];
			savedDockView = nil;
		}
		else
		{
		   	[dockTile setContentView:nil];
		}
	   	[dockTile display];
	}
	
	NSImage* OSXUIBinding::MakeImage(std::string value)
	{
		if (ti::UIModule::IsResourceLocalFile(value) || FileUtils::IsFile(value))
		{
			SharedString file = ti::UIModule::GetResourcePath(value.c_str());
			NSString *path = [NSString stringWithCString:((*file).c_str())];
			return [[NSImage alloc] initWithContentsOfFile:path];
		}
		else
		{
			NSURL *url = [NSURL URLWithString:[NSString stringWithCString:value.c_str()]];
			return [[NSImage alloc] initWithContentsOfURL:url];
		}
	}
	
	void OSXUIBinding::WindowFocused(UserWindow *window, OSXMenuItem *menu)
	{
		if (menu == this->activeMenu)
		{
			// if we focused but our active menu is already showing
			// for this window, don't worry about it..
			return;
		}
		// if we're setting the active window's menu to null and we 
		// have a app menu, re-install the app menu, otherwise set 
		// the incoming menu
		if (menu==NULL && this->menu)
		{
			SharedPtr<OSXMenuItem> osx_item = this->menu.cast<OSXMenuItem>();
			InstallMenu(osx_item.get());
		}
		else
		{
			InstallMenu(menu);
		}
	}

	void OSXUIBinding::WindowUnfocused(UserWindow *window, OSXMenuItem *menu)
	{
		if (this->activeMenu!=this->menu)
		{
			InstallMenu(this->activeMenu);
		}
		else
		{
			SharedPtr<OSXMenuItem> osx_item = this->menu.cast<OSXMenuItem>();
			InstallMenu(osx_item.get());
		}
	}
	
	void OSXUIBinding::InstallMenu(OSXMenuItem *menu)
	{
		this->activeMenu = menu;
		
		NSMenu * mainMenu = [[NSMenu alloc] initWithTitle:@"MainMenu"];
		NSMenuItem * menuItem;
		NSMenu * submenu;

		// The titles of the menu items are for identification purposes only and shouldn't be localized.
		// The strings in the menu bar come from the submenu titles,
		// except for the application menu, whose title is ignored at runtime.
		menuItem = [mainMenu addItemWithTitle:@"Apple" action:NULL keyEquivalent:@""];
		submenu = [[NSMenu alloc] initWithTitle:@"Apple"];
		[NSApp performSelector:@selector(setAppleMenu:) withObject:submenu];
		[mainMenu setSubmenu:submenu forItem:menuItem];


		std::string appName = AppConfig::Instance()->GetAppName();
		NSString * applicationName = [NSString stringWithCString:appName.c_str()];
		NSMenu *aMenu = submenu;

		menuItem = [aMenu addItemWithTitle:[NSString stringWithFormat:@"%@ %@", NSLocalizedString(@"About", nil), applicationName]
									action:@selector(orderFrontStandardAboutPanel:)
							 keyEquivalent:@""];
		[menuItem setTarget:NSApp];

		// [aMenu addItem:[NSMenuItem separatorItem]];
		// 
		// menuItem = [aMenu addItemWithTitle:NSLocalizedString(@"Preferences...", nil)
		// 							action:NULL
		// 					 keyEquivalent:@","];
		// 
		[aMenu addItem:[NSMenuItem separatorItem]];

		menuItem = [aMenu addItemWithTitle:NSLocalizedString(@"Services", nil)
									action:NULL
							 keyEquivalent:@""];
		NSMenu * servicesMenu = [[NSMenu alloc] initWithTitle:@"Services"];
		[aMenu setSubmenu:servicesMenu forItem:menuItem];
		[NSApp setServicesMenu:servicesMenu];
		
		[aMenu addItem:[NSMenuItem separatorItem]];

		menuItem = [aMenu addItemWithTitle:[NSString stringWithFormat:@"%@ %@", NSLocalizedString(@"Hide", nil), applicationName]
									action:@selector(hide:)
							 keyEquivalent:@"h"];
		[menuItem setTarget:NSApp];
		
		menuItem = [aMenu addItemWithTitle:NSLocalizedString(@"Hide Others", nil)
									action:@selector(hideOtherApplications:)
							 keyEquivalent:@"h"];
		[menuItem setKeyEquivalentModifierMask:NSCommandKeyMask | NSAlternateKeyMask];
		[menuItem setTarget:NSApp];
		
		menuItem = [aMenu addItemWithTitle:NSLocalizedString(@"Show All", nil)
									action:@selector(unhideAllApplications:)
							 keyEquivalent:@""];
		[menuItem setTarget:NSApp];
		
		[aMenu addItem:[NSMenuItem separatorItem]];
		
		menuItem = [aMenu addItemWithTitle:[NSString stringWithFormat:@"%@ %@", NSLocalizedString(@"Quit", nil), applicationName]
									action:@selector(terminate:)
							 keyEquivalent:@"q"];
		[menuItem setTarget:NSApp];
		
		// edit
		menuItem = [mainMenu addItemWithTitle:@"Edit" action:NULL keyEquivalent:@""];
		submenu = [[NSMenu alloc] initWithTitle:NSLocalizedString(@"Edit", @"The Edit menu")];
		[mainMenu setSubmenu:submenu forItem:menuItem];
		menuItem = [submenu addItemWithTitle:NSLocalizedString(@"Undo", nil)
									action:@selector(undo:)
							 keyEquivalent:@"z"];
		
		menuItem = [submenu addItemWithTitle:NSLocalizedString(@"Redo", nil)
									action:@selector(redo:)
							 keyEquivalent:@"Z"];
		
		[submenu addItem:[NSMenuItem separatorItem]];
		
		menuItem = [submenu addItemWithTitle:NSLocalizedString(@"Cut", nil)
									action:@selector(cut:)
							 keyEquivalent:@"x"];
		
		menuItem = [submenu addItemWithTitle:NSLocalizedString(@"Copy", nil)
									action:@selector(copy:)
							 keyEquivalent:@"c"];
		
		menuItem = [submenu addItemWithTitle:NSLocalizedString(@"Paste", nil)
									action:@selector(paste:)
							 keyEquivalent:@"v"];
		
		menuItem = [submenu addItemWithTitle:NSLocalizedString(@"Delete", nil)
									action:@selector(delete:)
							 keyEquivalent:@""];

		menuItem = [submenu addItemWithTitle: NSLocalizedString(@"Select All", nil)
							  action: @selector(selectAll:)
							  keyEquivalent: @"a"];
		// window
		menuItem = [mainMenu addItemWithTitle:@"Window" action:NULL keyEquivalent:@""];
		submenu = [[NSMenu alloc] initWithTitle:NSLocalizedString(@"Window", @"The Window menu")];
		[mainMenu setSubmenu:submenu forItem:menuItem];
		menuItem = [submenu addItemWithTitle:NSLocalizedString(@"Minimize", nil)
									action:@selector(performMinimize:)
							 keyEquivalent:@"m"];
		
		menuItem = [submenu addItemWithTitle:NSLocalizedString(@"Zoom", nil)
									action:@selector(performZoom:)
							 keyEquivalent:@""];
		
		[submenu addItem:[NSMenuItem separatorItem]];
		
		menuItem = [submenu addItemWithTitle:NSLocalizedString(@"Bring All to Front", nil)
									action:@selector(arrangeInFront:)
							 keyEquivalent:@""];
		
		[NSApp setWindowsMenu:submenu];

		if (menu)
		{
			OSXUIBinding::AttachMainMenu(mainMenu,menu);
		}
		
		[NSApp setMainMenu:mainMenu];
		[mainMenu release];
	}

	void OSXUIBinding::AttachMainMenu (NSMenu *mainMenu, ti::OSXMenuItem *item)
	{
		int count = item->GetChildCount();
		for (int c=0;c<count;c++)
		{
			OSXMenuItem *i = item->GetChild(c);
			const char *label = i->GetLabel();
			NSString *title = label==NULL ? @"" : [NSString stringWithCString:label];
			NSMenuItem *menuItem = [mainMenu addItemWithTitle:title action:NULL keyEquivalent:@""];
			NSMenu *submenu = [[NSMenu alloc] initWithTitle:title];
			[mainMenu setSubmenu:submenu forItem:menuItem];

			NSMenu *submenu2 = [[NSMenu alloc] initWithTitle:title];
			i->AttachMenu(submenu2);
			[menuItem setSubmenu:submenu2];

			// [menuItem release];
			// [submenu release];
			// [mi release];
		}
	}
	NSMenu* OSXUIBinding::MakeMenu(ti::OSXMenuItem* item)
	{
		const char *label = item->GetLabel();
		NSString *title = label == NULL ? @"" : [NSString stringWithCString:label];
		NSMenu *menu = [[NSMenu alloc] initWithTitle:title];
		int count = item->GetChildCount();
		for (int c=0;c<count;c++)
		{
			OSXMenuItem *i = item->GetChild(c);
			NSMenuItem *mi = i->CreateNative();
			[mi setEnabled:i->IsEnabled()];
			[menu addItem:mi];
			[mi release];
		}
		return menu;
	}

	void OSXUIBinding::SetDockMenu(SharedPtr<MenuItem> menu)
	{
		this->dockMenu = menu;
	}

	void OSXUIBinding::SetBadge(SharedString badge_label)
	{
		//TODO: Put Dock Icon support back in for 10.4.
		if (![NSApp respondsToSelector:@selector(dockTile)]){
			return;
		}

		std::string value = *badge_label;
		NSString *label = @"";
		if (!value.empty())
		{
			label = [NSString stringWithCString:value.c_str()];
		}
		DockTileStandin *tile = (DockTileStandin *)[[NSApplication sharedApplication] dockTile];
		[tile setBadgeLabel:label];
	}
	
	void OSXUIBinding::SetBadgeImage(SharedString badge_path)
	{
		//TODO: need to support allowing custom badge images
	}

	void OSXUIBinding::SetIcon(SharedString path)
	{
		std::string icon_path = *path;
		if (icon_path.empty())
		{
			[[NSApplication sharedApplication] setApplicationIconImage:nil];
		}
		else
		{
			NSImage *image = MakeImage(icon_path);
			[[NSApplication sharedApplication] setApplicationIconImage:image];
			[image release];
		}
	}
	
	SharedPtr<MenuItem> OSXUIBinding::GetDockMenu()
	{
		return this->dockMenu;
	}
	
	SharedPtr<MenuItem> OSXUIBinding::GetMenu()
	{
		return this->menu;
	}

	SharedPtr<MenuItem> OSXUIBinding::GetContextMenu()
	{
		return this->contextMenu;
	}
	
	SharedPtr<TrayItem> OSXUIBinding::AddTray(
		SharedString icon_path,
		SharedKMethod cb)
	{
		return new OSXTrayItem(icon_path,cb);
	}

	long OSXUIBinding::GetIdleTime()
	{
		// some of the code for this was from:
		// http://ryanhomer.com/blog/2007/05/31/detecting-when-your-cocoa-application-is-idle/
		CFMutableDictionaryRef properties = 0;
		CFTypeRef obj;
		mach_port_t masterPort;
		io_iterator_t iter;
		io_registry_entry_t curObj;

		IOMasterPort(MACH_PORT_NULL, &masterPort);

		/* Get IOHIDSystem */
		IOServiceGetMatchingServices(masterPort, IOServiceMatching("IOHIDSystem"), &iter);
		if (iter == 0)
		{
			return -1;
		}
		else
		{
			curObj = IOIteratorNext(iter);
		}
		if (IORegistryEntryCreateCFProperties(curObj, &properties, kCFAllocatorDefault, 0) == KERN_SUCCESS && properties != NULL)
		{
			obj = CFDictionaryGetValue(properties, CFSTR("HIDIdleTime"));
			CFRetain(obj);
		}
		else
		{
			return -1;
		}

		uint64_t tHandle = 0;
		if (obj)
		{
			CFTypeID type = CFGetTypeID(obj);

			if (type == CFDataGetTypeID())
			{
				CFDataGetBytes((CFDataRef) obj, CFRangeMake(0, sizeof(tHandle)), (UInt8*) &tHandle);
			}
			else if (type == CFNumberGetTypeID())
			{
				CFNumberGetValue((CFNumberRef)obj, kCFNumberSInt64Type, &tHandle);
			}
			else
			{
				// error
				tHandle = 0;
			}

			CFRelease(obj);

			tHandle /= 1000000; // return as milliseconds
		}
		else
		{
			tHandle = -1;
		}

		CFRelease((CFTypeRef)properties);
		IOObjectRelease(curObj);
		IOObjectRelease(iter);
		return (long)tHandle;
	}

}
