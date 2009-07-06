/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008-2009 Appcelerator, Inc. All Rights Reserved.
 */
@interface OSXTrayItemDelegate : NSObject
{
	ti::OSXTrayItem *trayItem;
}
-(id)initWithTray:(ti::OSXTrayItem*)item;
-(void)invoke:(id)sender;
@end
