/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#import "menu_action.h"

@implementation MenuAction

- (id)initWithMethod:(SharedKMethod)m
{
	self = [super init];
	if (self!=nil)
	{
		method = new SharedKMethod(m);
	}
	return self;
}

- (void)dealloc
{
	delete method;
	[super dealloc];
}

- (void)fire
{
	ValueList args;
	(*method)->Call(args);
}

@end
