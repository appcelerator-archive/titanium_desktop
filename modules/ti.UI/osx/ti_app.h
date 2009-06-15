/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */
#ifndef TI_APP_H
#define TI_APP_H

#import "preinclude.h"
#import <Cocoa/Cocoa.h>
#import "app_protocol.h"
#import "ti_protocol.h"
#import "../ui_module.h"

@interface TiApplication : NSObject
{
	ti::UIBinding *binding;
	kroll::Host *host;
}
+(TiApplication*)instance;
+(NSString*)appID;
-(id)initWithBinding:(ti::UIBinding*)binding host:(kroll::Host*)host;
-(kroll::Host*)host;
@end


#endif
