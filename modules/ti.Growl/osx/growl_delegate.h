/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#import "GrowlApplicationBridge.h"
#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>
#include <kroll/kroll.h>

@interface TiGrowlDelegate : NSObject <GrowlApplicationBridgeDelegate> {
	BOOL ready;
};

-(id)init;
-(BOOL) growlReady;
-(void) growlIsReady;
-(void) growlNotificationWasClicked:(id)clickContext;

@end

@interface MethodWrapper : NSObject {
 	KMethodRef *method;
};

- (id) initWithMethod:(KMethodRef*)m;
- (KMethodRef*) method;
- (void) dealloc;
@end
