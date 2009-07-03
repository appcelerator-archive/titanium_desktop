/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */
@interface OSXTrayDelegate : NSObject
{
	ti::OSXTrayItem *delegate;
	NSStatusItem *item;
	NSMenu *menu;
}
-(id)initWithTray:(ti::OSXTrayItem*)item;
-(void)invoke:(id)sender;
-(void)setIcon:(NSImage*)path;
-(void)setHint:(NSString*)hint;
-(void)setMenu:(NSMenu*)newMenu;

@end
