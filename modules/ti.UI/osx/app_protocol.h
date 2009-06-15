/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */
#import "preinclude.h"
#import <WebKit/WebKit.h>
#import <Cocoa/Cocoa.h>
#import <Foundation/Foundation.h>
#import <Foundation/NSURLRequest.h>
#import <Foundation/NSURLProtocol.h>


@interface AppProtocol : NSURLProtocol {

}

+ (NSString *)mimeTypeFromExtension:(NSString *)ext;
+ (NSString*)specialProtocolScheme;
+ (void) registerSpecialProtocol;
+ (NSString*) getPath:(NSURL*)url;
@end
