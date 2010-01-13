/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009, 2010 Appcelerator, Inc. All Rights Reserved.
 */

#include "../network_status.h"
#include <SystemConfiguration/SCNetworkReachability.h>

namespace ti
{

void NetworkStatus::InitializeLoop()
{
}

void NetworkStatus::CleanupLoop()
{
}

bool NetworkStatus::GetStatus()
{
	static SCNetworkReachabilityRef primaryTarget = 
		SCNetworkReachabilityCreateWithName(0, "www.google.com");
	if (!primaryTarget)
		return true;

	SCNetworkConnectionFlags flags;
	SCNetworkReachabilityGetFlags(primaryTarget, &flags);
	if ((flags & kSCNetworkFlagsReachable) && 
		!(flags & kSCNetworkFlagsConnectionRequired))
		return true;

	static SCNetworkReachabilityRef secondaryTarget = 
		SCNetworkReachabilityCreateWithName(0, "www.yahoo.com");
	if (!secondaryTarget)
		return true;

	SCNetworkReachabilityGetFlags(secondaryTarget, &flags);
	if ((flags & kSCNetworkFlagsReachable) && 
		!(flags & kSCNetworkFlagsConnectionRequired))
		return true;

	return false;
}

}
