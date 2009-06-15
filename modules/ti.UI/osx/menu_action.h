/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#import <Cocoa/Cocoa.h>
#import <kroll/kroll.h>

@interface MenuAction : NSObject {
	SharedKMethod *method;
}
- (id)initWithMethod:(SharedKMethod)method;
- (void)fire;
@end
