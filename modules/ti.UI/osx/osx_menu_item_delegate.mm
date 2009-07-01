/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#include "../ui_module.h"
@implementation OSXMenuItemDelegate
-(id)initWithMenuItem:(ti::OSXMenuItem*)inMenuItem
{
	if ([super init]) {
		menuItem = inMenuItem;
	}
	return self;
}

-(void)dealloc
{
	menuItem = nil;
	[super dealloc];
}

-(void)invoke:(id)sender
{
	menuItem->HandleClickEvent(NULL);
}

-(ti::OSXMenuItem*)getMenuItem
{
	return menuItem;
}
@end

