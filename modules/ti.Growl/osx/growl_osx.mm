/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */

#include "growl_osx.h"
#include <Foundation/Foundation.h>
#include <Cocoa/Cocoa.h>
#include <Growl/GrowlApplicationBridge.h>
#include <Growl/GrowlDefines.h>
#include <Growl/GrowlDefinesInternal.h>
#include <Growl/GrowlPathway.h>
#include <Growl/CFGrowlAdditions.h>
#include <sys/types.h>
#include <sys/stat.h>

using namespace kroll;
using namespace ti;

namespace ti
{

namespace
{
static std::map<std::string, KMethodRef> callbackMap;
static Poco::Mutex callbackMapMutex;

static id<GrowlNotificationProtocol> GetGrowlProxy(NSConnection* connection)
{
	NSDistantObject* theProxy = [connection rootProxy];
	[theProxy setProtocolForProxy:@protocol(GrowlNotificationProtocol)];
	id<GrowlNotificationProtocol> growlProxy = (id)theProxy;
	return growlProxy;
}

static CFDataRef GetAppIcon(SharedApplication app)
{
	return (CFDataRef) [[[NSApplication sharedApplication]
		applicationIconImage] TIFFRepresentation];
}

static void IsValueClickContext(const void* key,
	const void* value, void* context)
{
	CFStringRef stringKey = static_cast<CFStringRef>(key);
	if (CFStringCompare(stringKey, (CFStringRef) GROWL_NOTIFICATION_CLICK_CONTEXT, 0 ==
		kCFCompareEqualTo))
	{
		CFStringRef* result = static_cast<CFStringRef*>(const_cast<void*>(context));
		CFStringRef stringValue = static_cast<CFStringRef>(const_cast<void*>(value));
		*result = stringValue;
	}
}

std::string GetClickContext(CFDictionaryRef dictionary)
{
	CFStringRef cfcontext = 0;
	CFDictionaryApplyFunction(dictionary, IsValueClickContext, &cfcontext);

	if (cfcontext)
		return CFStringToUTF8(cfcontext);
	else
		return "";
}

static void NotificationTimedOut(CFNotificationCenterRef center,
	void* observer, CFStringRef name, const void*, CFDictionaryRef userInfo)
{
	std::string context(GetClickContext(userInfo));
	if (context.empty() && callbackMap.find(context) == callbackMap.end())
		return;

	{
		Poco::Mutex::ScopedLock lock(callbackMapMutex);
		callbackMap.erase(context);
	}
}

static void NotificationClicked(CFNotificationCenterRef center,
	void* observer, CFStringRef name, const void*, CFDictionaryRef userInfo)
{
	std::string context(GetClickContext(userInfo));
	if (context.empty() && callbackMap.find(context) == callbackMap.end())
		return;

	KMethodRef method(callbackMap[context]);
	RunOnMainThread(method, ArgList(), false);

	// When a notification gets a click message it does not time out.
	// So call TimedOut here so that the callback will be removed from
	// the callbackMap.
	NotificationTimedOut(center, observer, name, NULL, userInfo);
}

}

GrowlOSX::GrowlOSX(KObjectRef global) :
	GrowlBinding(global),
	app(Host::GetInstance()->GetApplication()),
	appName(UTF8ToCFString(app->name)),
	appId(UTF8ToCFString(app->id)),
	notificationName(CFSTR("tiNotification")),
	distCenter(CFNotificationCenterGetDistributedCenter()),
	connection(0)
{
	this->connection = [NSConnection 
		connectionWithRegisteredName:@"GrowlApplicationBridgePathway"
		host:nil];

	CFDataRef icon = GetAppIcon(app);
	CFRef<CFArrayRef> defaultAndAllNotifications(CFArrayCreate(kCFAllocatorDefault,
		 (const void **)&notificationName, 1, &kCFTypeArrayCallBacks));
	CFTypeRef registerKeys[4] =
	{
		GROWL_APP_NAME,
		GROWL_NOTIFICATIONS_ALL,
		GROWL_NOTIFICATIONS_DEFAULT,
		GROWL_APP_ICON
	};
	CFTypeRef registerValues[4] = {
		appName.get(),
		defaultAndAllNotifications.get(),
		defaultAndAllNotifications.get(),
		icon
	};
	CFRef<CFDictionaryRef> registerInfo(CFDictionaryCreate(kCFAllocatorDefault,
		registerKeys, registerValues, 4 , &kCFTypeDictionaryKeyCallBacks,
		&kCFTypeDictionaryValueCallBacks));

	if (this->connection)
	{
		@try
		{
			id<GrowlNotificationProtocol> proxy = GetGrowlProxy(connection);
			[proxy registerApplicationWithDictionary:
				(NSDictionary*) registerInfo.get()];
		}
		@catch(NSException *e)
		{
			Logger::Get("Notification.Notification")->Error(
				"Could not register application for Growl notifications");
		}
	}
	else
	{
		CFNotificationCenterPostNotificationWithOptions(distCenter,
			(CFStringRef)GROWL_APP_REGISTRATION, NULL, registerInfo.get(),
			kCFNotificationPostToAllSessions);
	}

	CFRef<CFMutableStringRef> notificationName(CFStringCreateMutable(kCFAllocatorDefault, 0));
	CFStringAppend(notificationName, appName.get());
	CFStringAppend(notificationName, (CFStringRef) GROWL_NOTIFICATION_CLICKED);
	CFNotificationCenterAddObserver(distCenter, NULL,
		NotificationClicked, notificationName.get(), NULL,
		CFNotificationSuspensionBehaviorCoalesce);

	CFStringReplaceAll(notificationName.get(), appName.get());
	CFStringAppend(notificationName, (CFStringRef)GROWL_NOTIFICATION_TIMED_OUT);
	CFNotificationCenterAddObserver(distCenter, NULL,
		NotificationTimedOut, notificationName.get(), NULL,
		CFNotificationSuspensionBehaviorCoalesce);
}

GrowlOSX::~GrowlOSX()
{
}

void GrowlOSX::ShowNotification(std::string& title, std::string& description,
	std::string& iconURL, int timeout, KMethodRef callback)
{
	int priority = 0;
	CFRef<CFUUIDRef> uuid(CFUUIDCreate(kCFAllocatorDefault));
	CFRef<CFStringRef> clickContext(CFUUIDCreateString(kCFAllocatorDefault, uuid));
	CFRef<CFNumberRef> priorityNumber(CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &priority));
	CFRef<CFMutableDictionaryRef> notificationInfo(CFDictionaryCreateMutable(kCFAllocatorDefault,
		9, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks));
	CFRef<CFStringRef> cftitle(UTF8ToCFString(title));
	CFRef<CFStringRef> cfdescription(UTF8ToCFString(description));

	CFDataRef icon = 0;
	if (!iconURL.empty())
	{
		// Convert all app:// and ti:// URLs to paths and then
		// back to file:// URLs. If a path is passed in, it
		// will not be modified by URLToPath and thus won't
		// need to be converted back to a URL.
		std::string iconPath(URLUtils::URLToPath(iconURL));
		std::string myIconURL(URLUtils::PathToFileURL(iconPath));

		NSURL* iconNSURL = [NSURL URLWithString:
			[NSString stringWithUTF8String:myIconURL.c_str()]];
		icon = (CFDataRef) [NSData dataWithContentsOfURL:iconNSURL];
	}

	if (!icon)
		icon = GetAppIcon(app);

	CFDictionarySetValue(notificationInfo.get(), GROWL_NOTIFICATION_NAME, notificationName);
	CFDictionarySetValue(notificationInfo.get(), GROWL_APP_NAME, appName.get());
	CFDictionarySetValue(notificationInfo.get(), GROWL_NOTIFICATION_TITLE, cftitle.get());
	CFDictionarySetValue(notificationInfo.get(), GROWL_NOTIFICATION_DESCRIPTION, cfdescription.get());
	CFDictionarySetValue(notificationInfo.get(), GROWL_NOTIFICATION_STICKY, kCFBooleanFalse);
	CFDictionarySetValue(notificationInfo.get(), GROWL_NOTIFICATION_ICON, icon);
	CFDictionarySetValue(notificationInfo.get(), GROWL_NOTIFICATION_PRIORITY, priorityNumber.get());
	CFDictionarySetValue(notificationInfo.get(), GROWL_NOTIFICATION_CLICK_CONTEXT, clickContext.get());

	if (!callback.isNull())
	{
		Poco::Mutex::ScopedLock lock(callbackMapMutex);
		std::string clickContextString(CFStringToUTF8(clickContext.get()));
		callbackMap[clickContextString] = callback;
	}

	this->connection = [NSConnection 
		connectionWithRegisteredName:@"GrowlApplicationBridgePathway" host:nil];
	if (this->connection)
	{
		id<GrowlNotificationProtocol> proxy = GetGrowlProxy(this->connection);
		[proxy postNotificationWithDictionary:(NSDictionary *) notificationInfo.get()];
	}
	else
	{
		CFNotificationCenterPostNotificationWithOptions(distCenter,
			(CFStringRef) GROWL_NOTIFICATION, NULL, notificationInfo,
			kCFNotificationPostToAllSessions);
	}
}

bool GrowlOSX::IsRunning()
{
	return [GrowlApplicationBridge isGrowlRunning];
}
}
