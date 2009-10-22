/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */

#import "growl_delegate.h"
#include <kroll/kroll.h>

@implementation TiGrowlDelegate

-(id) init
{
	self = [super init];
	
	if (self != nil)
	{
		[GrowlApplicationBridge setGrowlDelegate:self];
	}
	
	return self;
}

-(BOOL) growlReady {
	return ready;
}

//// GrowlDelegate callbacks

-(void) growlIsReady
{
	ready = TRUE;
}

- (void) growlNotificationWasClicked:(id)clickContext
{
	NSMutableArray* array = (NSMutableArray*) clickContext;
	MethodWrapper* wrapper = (MethodWrapper*) [array objectAtIndex:0];
	KMethodRef* method = [wrapper method];
	if (!method->isNull())
	{
		ValueList args;
		(*method)->Call(args);
	}
	[array release]; // after callback release the reference
}

@end

@implementation MethodWrapper

-(id) initWithMethod:(KMethodRef*)m
{
	self = [super init];
	
	if (self != nil) {
		method = m;
	}
	
	return self;
}

- (KMethodRef *) method
{
	return method;
}

- (void) dealloc
{
	delete method;
	[super dealloc];
}

@end
