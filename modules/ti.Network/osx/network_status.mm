/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#import "network_status.h"

// This routine is a System Configuration framework callback that
// indicates that the reachability of a given host has changed.
// It's call from the runloop.  target is the host whose reachability
// has changed, the flags indicate the new reachability status, and
// info is the context parameter that we passed in when we registered
// the callback.  In this case, info is a pointer to the host name.
//
// Our response to this notification is simply to print a line
// recording the transition.
static void TiReachabilityCallback(SCNetworkReachabilityRef target,
	SCNetworkConnectionFlags flags, void *info)
{
	BOOL online = (flags & kSCNetworkFlagsReachable) && !(flags & kSCNetworkFlagsTransientConnection) && !(flags & kSCNetworkFlagsConnectionRequired);
	NetworkReachability *network = (NetworkReachability*)info;
	[network triggerChange:online];
}

@implementation NetworkReachability

-(id)initWithDelegate:(SharedKMethod)m
{
	self = [super init];
	if (self!=nil)
	{
		delegate = new SharedKMethod(m);
		online = YES;
		[NSThread detachNewThreadSelector:@selector(start) toTarget:self withObject:nil];
	}
	return self;
}
- (void)dealloc
{
	delete delegate;
	[super dealloc];
}
- (void)start
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool init] alloc];
	SCNetworkReachabilityRef thisTarget;
	SCNetworkReachabilityContext thisContext;
	thisContext.version = 0;
	thisContext.info = (void *)self;
	thisContext.retain = NULL;
	thisContext.release = NULL;
	thisContext.copyDescription = NULL;

	// Create the target with the most reachable internet site in the world
	thisTarget = SCNetworkReachabilityCreateWithName(NULL, "www.google.com");

	// Set our callback and install on the runloop.
	if (thisTarget)
	{
		SCNetworkReachabilitySetCallback(thisTarget, TiReachabilityCallback, &thisContext);
		SCNetworkReachabilityScheduleWithRunLoop(thisTarget, CFRunLoopGetMain(), kCFRunLoopDefaultMode);
		SCNetworkConnectionFlags flags;
		if ( SCNetworkReachabilityGetFlags(thisTarget, &flags) )
		{
			if ((flags & kSCNetworkFlagsReachable) && !(flags & kSCNetworkFlagsConnectionRequired))
			{
				[self triggerChange:YES];
			}
			else
			{
				// this occurs on startup where the runloop hasn't scheduled
				// the check and the flags are 0 ... in this case we check
				// directly another site to make sure we're OK
				if (SCNetworkCheckReachabilityByName("www.yahoo.com", &flags) && (flags & kSCNetworkFlagsReachable))
				{
					[self triggerChange:YES];
				}
				else
				{
					[self triggerChange:NO];
				}
			}
		}
	}
	[pool release];
}
- (void)triggerChange:(BOOL)yn
{
	if (yn!=online)
	{
		online = yn;
		ValueList args;
		args.push_back(Value::NewBool(online));
		(*delegate)->Call(args);
	}
}
@end
