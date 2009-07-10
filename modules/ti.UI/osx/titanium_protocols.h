/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008-2009 Appcelerator, Inc. All Rights Reserved.
 */
#import "../ui_module.h"
#import <Foundation/Foundation.h>
#import <Foundation/NSURLRequest.h>
#import <Foundation/NSURLProtocol.h>
@interface TitaniumProtocols : NSURLProtocol {
}
+(NSString *)mimeTypeFromExtension:(NSString *)ext;
@end
