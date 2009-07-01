/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#include "../ui_module.h"

@interface NSApplication (LegacyWarningSurpression)
- (id) dockTile;
@end

@interface DockTileStandin: NSObject {
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
		defaultMenu(nil),
		menu(NULL),
		nativeMenu(nil),
		contextMenu(NULL),
		dockMenu(NULL),
		nativeDockMenu(nil),
		activeWindow(NULL),
		scriptEvaluator(nil)
	{
		[TiProtocol registerSpecialProtocol];
		[AppProtocol registerSpecialProtocol];
		application = [[TiApplication alloc] initWithBinding:this host:host];

		NSApplication *nsapp = [NSApplication sharedApplication];
		[nsapp setDelegate:application];
		[NSBundle loadNibNamed:@"MainMenu" owner:nsapp];

		// Create a default menu -- so that keybindings and such work out of the box.
		this->defaultMenu = [NSApp mainMenu];
		this->ReplaceMainMenu();

		// Add the custom script evaluator which will dynamically
		// dispatch unknown script types to loaded Kroll modules.
		scriptEvaluator = [[ScriptEvaluator alloc] initWithHost:host];
	}

	OSXUIBinding::~OSXUIBinding()
	{
		[scriptEvaluator release];
		[application release];
		[savedDockView release];
	}

	SharedUserWindow OSXUIBinding::CreateWindow(
		WindowConfig* config,
		SharedUserWindow& parent)
	{
		UserWindow* w = new OSXUserWindow(config, parent);
		return w->GetSharedPtr();
	}

	SharedMenu OSXUIBinding::CreateMenu()
	{
		return new OSXMenu();
	}

	SharedMenuItem OSXUIBinding::CreateMenuItem(
		std::string label, SharedKMethod callback, std::string iconURL)
	{
		return new OSXMenuItem(MenuItem::NORMAL, label, callback, iconURL);
	}

	SharedMenuItem OSXUIBinding::CreateSeparatorMenuItem()
	{
		return new OSXMenuItem(MenuItem::SEPARATOR, std::string(), NULL, std::string());
	}

	SharedMenuItem OSXUIBinding::CreateCheckMenuItem(
		std::string label, SharedKMethod callback)
	{
		return new OSXMenuItem(MenuItem::NORMAL, label, callback, std::string());
	}

	void OSXUIBinding::ErrorDialog(std::string msg)
	{
		NSApplicationLoad();
		if (!msg.empty())
			 NSRunCriticalAlertPanel (@"Application Error", [NSString stringWithUTF8String:msg.c_str()],nil,nil,nil);
		UIBinding::ErrorDialog(msg);
	}

	void OSXUIBinding::SetMenu(SharedMenu menu)
	{
		if (this->menu.get() == menu.get())
		{
			return;
		}
		SharedPtr<OSXMenu> osxmenu = menu.cast<OSXMenu>();
		this->menu = osxmenu;
		SetupMainMenu();
	}

	void OSXUIBinding::SetContextMenu(SharedMenu menu)
	{
		this->contextMenu = menu.cast<OSXMenu>();
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
			NSString *path = [NSString stringWithCString:((*file).c_str()) encoding:NSUTF8StringEncoding];
			return [[NSImage alloc] initWithContentsOfFile:path];
		}
		else
		{
			NSURL *url = [NSURL URLWithString:[NSString stringWithCString:value.c_str() encoding:NSUTF8StringEncoding]];
			return [[NSImage alloc] initWithContentsOfURL:url];
		}
	}
	
	void OSXUIBinding::WindowFocused(SharedPtr<OSXUserWindow> window)
	{
		this->activeWindow = window;
		this->SetupMainMenu();
	}

	void OSXUIBinding::WindowUnfocused(SharedPtr<OSXUserWindow> window)
	{
		this->activeWindow = NULL;
		this->SetupMainMenu();
	}

	NSMenu* OSXUIBinding::GetDefaultMenu()
	{
		return this->defaultMenu;
	}

	void OSXUIBinding::SetupMainMenu()
	{
		SharedPtr<OSXMenu> newActiveMenu = NULL;
		SharedPtr<OSXMenu> oldMenu = this->activeMenu;
		NSMenu* oldNativeMenu = [NSApp mainMenu];

		// If there is an active window, search there first for the menu
		if (!this->activeWindow.isNull()) {
			newActiveMenu = this->activeWindow->GetMenu().cast<OSXMenu>();
		}

		// No active window or that window has no menu, try to use the app menu
		if (newActiveMenu.isNull()) {
			newActiveMenu = this->menu;
		}

		if (newActiveMenu.get() != this->activeMenu.get()) {
			this->activeMenu = newActiveMenu;
			this->ReplaceMainMenu();
		}

		if (!oldMenu.isNull() && oldNativeMenu
			&& oldNativeMenu != this->GetDefaultMenu()) {
			oldMenu->DestroyNative(oldNativeMenu);
		}
	}

	void OSXUIBinding::SetupAppMenuParts(NSMenu* nativeMainMenu)
	{
		//OSXMenu::FixWindowMenu(nativeMainMenu);
		[NSApp setWindowsMenu:OSXMenu::GetWindowMenu(nativeMainMenu)];
		[NSApp performSelector:@selector(setAppleMenu:)
			withObject:OSXMenu::GetAppleMenu(nativeMainMenu)];
		[NSApp setServicesMenu:OSXMenu::GetServicesMenu(nativeMainMenu)];
	}

	void OSXUIBinding::ReplaceMainMenu()
	{
		NSMenu* newNativeMenu = nil;

		if (this->activeMenu.isNull()) {
			newNativeMenu = this->GetDefaultMenu();
		} else {
			newNativeMenu = [[NSMenu alloc] init];
			this->activeMenu->FillNativeMainMenu(defaultMenu, newNativeMenu);
		}

		SetupAppMenuParts(newNativeMenu);
		[NSApp setMainMenu:newNativeMenu];
	}

	void OSXUIBinding::RefreshMainMenu()
	{
		NSMenu* mainMenu = [NSApp mainMenu];
		if (!this->activeMenu.isNull() && mainMenu != defaultMenu) {
			OSXMenu::ClearNativeMenu(mainMenu);
			this->activeMenu->FillNativeMainMenu(defaultMenu, mainMenu);
			SetupAppMenuParts(mainMenu);
		}
	}

	void OSXUIBinding::SetDockMenu(SharedMenu menu)
	{
		this->dockMenu = menu.cast<OSXMenu>();
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
			label = [NSString stringWithCString:value.c_str() encoding:NSUTF8StringEncoding];
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

	SharedMenu OSXUIBinding::GetDockMenu()
	{
		return this->dockMenu;
	}

	SharedMenu OSXUIBinding::GetMenu()
	{
		return this->menu;
	}

	SharedMenu OSXUIBinding::GetContextMenu()
	{
		return this->contextMenu;
	}

	SharedTrayItem OSXUIBinding::AddTray(
		SharedString iconPath,
		SharedKMethod eventListener)
	{
		return new OSXTrayItem(iconPath, eventListener);
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
