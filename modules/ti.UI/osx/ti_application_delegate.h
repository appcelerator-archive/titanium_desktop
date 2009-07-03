/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */
#ifndef TI_APP_H
#define TI_APP_H
#import "../ui_module.h"
@interface TiApplicationDelegate : NSObject
{
	ti::OSXUIBinding *binding;
}
+(TiApplicationDelegate*)instance;
+(NSString*)appID;
-(id)initWithBinding:(ti::OSXUIBinding*)binding;
@end

#endif
