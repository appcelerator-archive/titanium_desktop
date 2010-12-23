/*
 * Copyright (c) 2009-2010 Appcelerator, Inc. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "../NetworkStatus.h"

#include <SystemConfiguration/SCNetworkReachability.h>

namespace Titanium {

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

} // namespace Titanium
