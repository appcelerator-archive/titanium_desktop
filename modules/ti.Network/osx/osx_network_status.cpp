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
	// The SCNetworkReachability object must be created each time,
	// otherwise it cannot properly transition from initially offline
	// to online.
	CFRef<SCNetworkReachabilityRef> primaryTarget(
		SCNetworkReachabilityCreateWithName(0, "www.google.com"));
	if (!primaryTarget.get())
		return true;

	SCNetworkConnectionFlags flags;
	SCNetworkReachabilityGetFlags(primaryTarget.get(), &flags);
	if ((flags & kSCNetworkFlagsReachable) && 
		!(flags & kSCNetworkFlagsConnectionRequired))
		return true;

	CFRef<SCNetworkReachabilityRef> secondaryTarget(
		SCNetworkReachabilityCreateWithName(0, "www.yahoo.com"));
	if (!secondaryTarget.get())
		return true;

	SCNetworkReachabilityGetFlags(secondaryTarget.get(), &flags);
	if ((flags & kSCNetworkFlagsReachable) && 
		!(flags & kSCNetworkFlagsConnectionRequired))
		return true;

	return false;
}

}
