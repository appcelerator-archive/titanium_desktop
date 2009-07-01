/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#import "../ui_module.h"

@implementation OSXMenuDelegate

-(id)initWithMenu:(ti::OSXMenu*)inMenu
{
	if ([super init]) {
		dirty = YES;
		menu = inMenu;
	}
	return self;
}

- (void)menuNeedsUpdate:(NSMenu *)nativeMenu
{
	if (!dirty) {
		return;
	}

	dirty = NO;
	menu->AddChildrenToNativeMenu(nativeMenu, true);
}

- (void)markAsDirty;
{
	dirty = YES;
}
@end


