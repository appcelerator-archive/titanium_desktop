/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */
#import "preinclude.h"
#import <WebKit/WebKit.h>
#import <Cocoa/Cocoa.h>

@interface TiProtocol : NSURLProtocol {
}

+ (NSString*) specialProtocolScheme;
+ (void) registerSpecialProtocol;

@end
