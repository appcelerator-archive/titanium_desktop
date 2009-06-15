/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#import "osx_menu_delegate.h"
#import "osx_menu_item.h"
#import "osx_ui_binding.h"
#import "../ui_module.h"

@implementation OSXMenuDelegate
-(id)initWithMenu:(ti::OSXMenuItem*)item menu:(NSMenu*)submenu
{
	const char *label = item->GetLabel();
	NSString *title = label!=NULL ? [NSString stringWithCString:label] : @"";
	self = [super initWithTitle:title action:@selector(invoke:) keyEquivalent:@""];
	if (self!=nil)
	{
		delegate = item;
		[self setTarget:self];
		const char *icon_url = item->GetIconURL();
		if (icon_url)
		{
			if (ti::UIModule::IsResourceLocalFile(icon_url))
			{
				SharedString file = ti::UIModule::GetResourcePath(icon_url);
				NSString *path = [NSString stringWithCString:((*file).c_str())];
				NSImage *image = [[NSImage alloc] initWithContentsOfFile:path];
				[self setImage:image];
				[image release];
			}
			else
			{
				NSURL *url = [NSURL URLWithString:[NSString stringWithCString:icon_url]];
				NSImage *image = [[NSImage alloc] initWithContentsOfURL:url];
				[self setImage:image];
				[image release];
			}
		}
		int count = item->GetChildCount();
		if (count > 0)
		{
			if (submenu == nil)
			{
				NSMenu *submenu = ti::OSXUIBinding::MakeMenu(item);
				if ([submenu numberOfItems] == 0)
				{
					// this happens if they're all disabled
					[submenu release];
				}
				else
				{
					[self setSubmenu:submenu];
					// don't release this or items 
				}
			}
			else
			{
				for (int c=0;c<count;c++)
				{
					OSXMenuItem *child = item->GetChild(c);
					NSMenuItem *i = child->CreateNative();
					[submenu addItem:i];
					[i release];
				}
			}
		}
	}
	return self;
}
-(void)dealloc
{
	delegate = nil;
	[super dealloc];
}
-(void)invoke:(id)sender
{
	delegate->Invoke();
}
@end


