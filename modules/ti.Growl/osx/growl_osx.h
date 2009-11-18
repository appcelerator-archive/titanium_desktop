/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef GROWL_OSX_H_
#define GROWL_OSX_H_

#include <kroll/kroll.h>
#include <string>
#include "../growl_binding.h"

namespace ti
{
	class GrowlOSX : public GrowlBinding
	{
	public:
		GrowlOSX(KObjectRef global);
		virtual ~GrowlOSX();
		virtual void ShowNotification(std::string& title, std::string& description,
			 std::string& iconURL, int timeout, KMethodRef callback);
		virtual bool IsRunning();

	protected:
		SharedApplication app;
		CFRef<CFStringRef> appName;
		CFRef<CFStringRef> appId;
		CFStringRef notificationName; // This is a constant.
		CFNotificationCenterRef distCenter;
		NSConnection* connection;
	};
}

#endif /* GROWL_OSX_H_ */
