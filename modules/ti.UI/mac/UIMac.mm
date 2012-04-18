/*
 * Copyright (c) 2009-2010 Appcelerator, Inc. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "UIMac.h"

#import <AppKit/NSApplication.h>
#import <WebKit/WebKit.h>

#import "TitaniumApplicationDelegate.h"
#import "TitaniumProtocols.h"

#include "MenuItemMac.h"
#include "MenuMac.h"
#include "TrayItemMac.h"
#include "UserWindowMac.h"

#include <stdio.h>
#include <DiskArbitration/DiskArbitration.h>

#define DBG if(0)
void printPair(const void* key, const void* value, void* context)
{
	printf("{\n");
	CFShow(key);
	CFShow(value);
	printf("}\n");
}
inline bool TestBool(const void *v)
{
	return v && CFBooleanGetValue((CFBooleanRef)v);
}
inline bool TestStr(const void *v, CFStringRef s)
{
	return v && !CFStringCompare((CFStringRef)v, s, 0);
}

void report_disk(DADiskRef disk, void *context, const char *event, CFDictionaryRef desc = NULL)
{
	if (!desc)
		desc = DADiskCopyDescription(disk);
	if (desc) {
		DBG CFDictionaryApplyFunction(desc, printPair, context);
		DBG printf("disk %s %s\n", DADiskGetBSDName(disk), event);
		if (TestBool(CFDictionaryGetValue(desc, kDADiskDescriptionMediaLeafKey))) {
			DBG DBG printf("disk %s is leafy\n", DADiskGetBSDName(disk));
			if (TestBool(CFDictionaryGetValue(desc, kDADiskDescriptionMediaRemovableKey))) {
				DBG printf("disk %s is removable\n", DADiskGetBSDName(disk));
				if (TestBool(CFDictionaryGetValue(desc, kDADiskDescriptionMediaWritableKey))) {
					DBG printf("disk %s is writable\n", DADiskGetBSDName(disk));
					if (!TestBool(CFDictionaryGetValue(desc, kDADiskDescriptionVolumeNetworkKey))) {
						DBG printf("disk %s is not networked\n", DADiskGetBSDName(disk));
						if (TestStr(CFDictionaryGetValue(desc, kDADiskDescriptionMediaKindKey), CFSTR("IOMedia"))) {
							DBG printf("disk %s is not optical\n", DADiskGetBSDName(disk));
							DBG printf("disk %s %s\n", DADiskGetBSDName(disk), event);
							GlobalObject::GetInstance()->FireEvent(event);
						}
					}
				}
			}
		}
		CFRelease(desc);
	}
}

void hello_disk(DADiskRef disk, void *context)
{
	report_disk(disk, context, "volume.added");
}

void goodbye_disk(DADiskRef disk, void *context)
{
	report_disk(disk, context, "volume.removed");
}

void follow_disk(DADiskRef disk, CFArrayRef keys, void *context)
{
	CFDictionaryRef desc = DADiskCopyDescription(disk);
	if (desc && CFDictionaryGetValue(desc, kDADiskDescriptionVolumePathKey))
		report_disk(disk, context, "volume.added", desc);
}

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

namespace Titanium {

UIMac::UIMac()
    : defaultMenu(nil)
    , menu(0)
    , nativeMenu(nil)
    , contextMenu(0)
    , dockMenu(0)
    , nativeDockMenu(nil)
    , activeWindow(0)
{
    TitaniumApplicationDelegate* application = [[TitaniumApplicationDelegate alloc] initWithBinding:this];
    [application retain];

    NSApplication* nsapp = [NSApplication sharedApplication];
    [nsapp setDelegate:application];
    [NSBundle loadNibNamed:@"MainMenu" owner:nsapp];

    // Create a default menu -- so that keybindings and such work out of the box.
    this->defaultMenu = [NSApp mainMenu];

    NSString* appName = [NSString
        stringWithUTF8String:Host::GetInstance()->GetApplication()->name.c_str()];
    MenuMac::ReplaceAppNameStandinInMenu(this->defaultMenu, appName);
    MenuMac::SetupInspectorItem(this->defaultMenu);
    this->SetupMainMenu(true);

    // Register our custom URL handler
    [NSURLProtocol registerClass:[TitaniumProtocols class]];

    // SECURITY FLAG: this will allow apps to have the same security
    // as local files (like cross-domain XHR requests).  we should 
    // make sure this is part of the upcoming security work
    [WebView registerURLSchemeAsLocal:@"app"];
    [WebView registerURLSchemeAsLocal:@"ti"];

    session = DASessionCreate(kCFAllocatorDefault);

    DBG DARegisterDiskAppearedCallback(session, NULL, hello_disk, NULL);
    DARegisterDiskDisappearedCallback(session, NULL, goodbye_disk, NULL);
    DARegisterDiskDescriptionChangedCallback(session, NULL, kDADiskDescriptionWatchVolumePath, follow_disk, NULL);

    DASessionScheduleWithRunLoop(session,
        CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
}

UIMac::~UIMac()
{
    [application release];
    [savedDockView release];
    CFRelease(session);
}

AutoPtr<Menu> UIMac::CreateMenu()
{
    return new MenuMac();
}

AutoPtr<MenuItem> UIMac::CreateMenuItem()
{
    return new MenuItemMac(MenuItem::NORMAL);
}

AutoPtr<MenuItem> UIMac::CreateSeparatorMenuItem()
{
    return new MenuItemMac(MenuItem::SEPARATOR);
}

AutoPtr<MenuItem> UIMac::CreateCheckMenuItem()
{
    return new MenuItemMac(MenuItem::CHECK);
}

void UIMac::SetMenu(AutoPtr<Menu> menu)
{
    if (this->menu.get() == menu.get())
    {
        return;
    }
    AutoPtr<MenuMac> osxmenu = menu.cast<MenuMac>();
    this->menu = osxmenu;
    SetupMainMenu();
}

void UIMac::SetContextMenu(AutoPtr<Menu> menu)
{
    this->contextMenu = menu.cast<MenuMac>();
}

void UIMac::SetDockIcon(string& badgePath)
{
    //TODO: Put Dock Icon support back in for 10.4.
    if (![NSApp respondsToSelector:@selector(dockTile)]) {
        return;
    }

    DockTileStandin *dockTile = (DockTileStandin *)[NSApp dockTile];
    if (!badgePath.empty())
    {
        if (!savedDockView) // remember the old one
        {
            savedDockView = [dockTile contentView];
            [savedDockView retain];
        }

        // setup our image view for the dock tile
        NSRect frame = NSMakeRect(0, 0, dockTile.size.width, dockTile.size.height);
        NSImageView *dockImageView = [[NSImageView alloc] initWithFrame: frame];
        NSImage *image = MakeImage(badgePath);
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

NSImage* UIMac::MakeImage(string& iconURL)
{
    NSString* iconString = [NSString stringWithUTF8String:iconURL.c_str()];
    if (FileUtils::IsFile(iconURL)) {
        return [[NSImage alloc] initWithContentsOfFile:iconString];
    } else {
        return [[NSImage alloc] initWithContentsOfURL:[NSURL URLWithString: iconString]];
    }
}

void UIMac::WindowFocused(AutoPtr<UserWindowMac> window)
{
    this->activeWindow = window;
    this->SetupMainMenu();
}

void UIMac::WindowUnfocused(AutoPtr<UserWindowMac> window)
{
    this->activeWindow = NULL;
    this->SetupMainMenu();
}

NSMenu* UIMac::GetDefaultMenu()
{
    return this->defaultMenu;
}

AutoPtr<MenuMac> UIMac::GetActiveMenu()
{
    return this->activeMenu;
}

void UIMac::SetupMainMenu(bool force)
{
    AutoPtr<MenuMac> newActiveMenu = NULL;

    // If there is an active window, search there first for the menu
    if (!this->activeWindow.isNull()) {
        newActiveMenu = this->activeWindow->GetMenu().cast<MenuMac>();
    }

    // No active window or that window has no menu, try to use the app menu
    if (newActiveMenu.isNull()) {
        newActiveMenu = this->menu;
    }

    if (force || newActiveMenu.get() != this->activeMenu.get()) {
        AutoPtr<MenuMac> oldMenu = this->activeMenu; // Save a reference
        NSMenu* oldNativeMenu = [NSApp mainMenu];

        // Actually create and install the new menu
        NSMenu* newNativeMenu = nil;
        if (newActiveMenu.isNull()) {
            newNativeMenu = this->GetDefaultMenu();
        } else {
            newNativeMenu = [[NSMenu alloc] init];
            newActiveMenu->FillNativeMainMenu(defaultMenu, newNativeMenu);
        }
        SetupAppMenuParts(newNativeMenu);
        [NSApp setMainMenu:newNativeMenu];
        this->activeMenu = newActiveMenu;

        // Cleanup the old native menu
        if (!oldMenu.isNull() && oldNativeMenu
            && oldNativeMenu != this->GetDefaultMenu()) {
            oldMenu->DestroyNative(oldNativeMenu);
        }
    }
}

void UIMac::SetupAppMenuParts(NSMenu* nativeMainMenu)
{
    MenuMac::FixWindowMenu(nativeMainMenu);
    [NSApp setWindowsMenu:MenuMac::GetWindowMenu(nativeMainMenu)];
    [NSApp performSelector:@selector(setAppleMenu:)
        withObject:MenuMac::GetAppleMenu(nativeMainMenu)];
    [NSApp setServicesMenu:MenuMac::GetServicesMenu(nativeMainMenu)];
}

void UIMac::SetDockMenu(AutoPtr<Menu> menu)
{
    this->dockMenu = menu.cast<MenuMac>();
}

void UIMac::SetBadge(string& badgeLabel)
{
    //TODO: Put Dock Icon support back in for 10.4.
    if (![NSApp respondsToSelector:@selector(dockTile)]){
        return;
    }

    DockTileStandin *tile = (DockTileStandin *)[[NSApplication sharedApplication] dockTile];
    if (!badgeLabel.empty()) {
        [tile setBadgeLabel:[NSString stringWithUTF8String:badgeLabel.c_str()]];

    } else {
        [tile setBadgeLabel:@""];
    }
}

void UIMac::SetBadgeImage(string& badgePath)
{
    //TODO: need to support allowing custom badge images
}

void UIMac::SetIcon(string& iconPath)
{
    if (iconPath.empty())
    {
        [[NSApplication sharedApplication] setApplicationIconImage:nil];
    }
    else
    {
        NSImage *image = MakeImage(iconPath);
        [[NSApplication sharedApplication] setApplicationIconImage:image];
        [image release];
    }
}

AutoPtr<Menu> UIMac::GetDockMenu()
{
    return this->dockMenu;
}

AutoPtr<Menu> UIMac::GetMenu()
{
    return this->menu;
}

AutoPtr<Menu> UIMac::GetContextMenu()
{
    return this->contextMenu;
}

AutoPtr<TrayItem> UIMac::AddTray(string& iconPath, KMethodRef eventListener)
{
    return new TrayItemMac(iconPath, eventListener);
}

long UIMac::GetIdleTime()
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

/*static*/
void UIMac::ErrorDialog(string msg)
{
    NSApplicationLoad();
    NSRunCriticalAlertPanel(@"Application Error", 
        [NSString stringWithUTF8String:msg.c_str()], nil, nil, nil);
    UI::ErrorDialog(msg);
}

} // namespace Titanium
