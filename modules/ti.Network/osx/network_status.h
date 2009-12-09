/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#import <Carbon/Carbon.h>
#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>
#import <SystemConfiguration/SCNetworkReachability.h>
#import <kroll/kroll.h>

@interface NetworkReachability : NSObject
{
	KMethodRef *delegate;
	BOOL online;
}
- (id)initWithDelegate:(KMethodRef)delegate;
- (void)triggerChange:(BOOL)yn;
- (void)start;
@end

