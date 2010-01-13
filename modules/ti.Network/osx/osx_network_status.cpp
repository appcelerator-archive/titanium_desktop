/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009, 2010 Appcelerator, Inc. All Rights Reserved.
 */

#include "../network_status.h"
#include <SystemConfiguration/SCNetworkReachability.h>

namespace ti
{

static SCNetworkReachabilityRef primaryTarget = 0;
static SCNetworkReachabilityRef secondaryTarget = 0;

void NetworkStatus::InitializeLoop()
{
	primaryTarget = SCNetworkReachabilityCreateWithName(0, "www.google.com");
	secondaryTarget = SCNetworkReachabilityCreateWithName(0, "www.yahoo.com");
}

void NetworkStatus::CleanupLoop()
{
	if (primaryTarget)
		CFRelease(primaryTarget);
	if (secondaryTarget)
		CFRelease(secondaryTarget);
}

bool NetworkStatus::GetStatus()
{
	if (!primaryTarget)
		return true;

	SCNetworkConnectionFlags flags;
	SCNetworkReachabilityGetFlags(primaryTarget, &flags);
	if ((flags & kSCNetworkFlagsReachable) && 
		!(flags & kSCNetworkFlagsConnectionRequired))
		return true;

	if (!secondaryTarget)
		return true;

	SCNetworkReachabilityGetFlags(secondaryTarget, &flags);
	if ((flags & kSCNetworkFlagsReachable) && 
		!(flags & kSCNetworkFlagsConnectionRequired))
		return true;

	return false;
}

}
