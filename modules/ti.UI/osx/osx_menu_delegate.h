/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */
@interface OSXMenuDelegate : NSObject
{
	ti::OSXMenu* menu;
	BOOL dirty;
}
- (id)initWithMenu:(ti::OSXMenu*)menu;
- (void)menuNeedsUpdate:(NSMenu *)menu;
- (void)markAsDirty;
@end
