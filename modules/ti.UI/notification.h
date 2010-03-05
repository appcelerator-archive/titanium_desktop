/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2010 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _NOTIFICATION_H_
#define _NOTIFICATION_H_

#include <kroll/kroll.h>

#ifdef OS_LINUX
typedef struct _NotifyNotification NotifyNotification;
typedef NotifyNotification* PlatformNotification;
#elif defined(OS_WIN32)
typedef long PlatformNotification;
#elif defined(OS_OSX)
// Unused.
typedef char PlatformNotification;
#endif

namespace ti
{

class Notification : public KAccessorObject
{
public:
	Notification();
	~Notification();
	void Configure(KObjectRef properties);

	static bool InitializeImpl();
	static void ShutdownImpl();
	void CreateImpl();
	void DestroyImpl();
	bool ShowImpl();
	bool HideImpl();

private:
	std::string title;
	std::string message;
	std::string iconURL;
	int timeout;
	KMethodRef clickedCallback;
	PlatformNotification notification;

	void _SetTitle(const ValueList& args, KValueRef result);
	void _SetMessage(const ValueList& args, KValueRef result);
	void _SetIcon(const ValueList& args, KValueRef result);
	void _SetTimeout(const ValueList& args, KValueRef result);
	void _SetCallback(const ValueList& args, KValueRef result);
	void _Show(const ValueList& args, KValueRef result);
	void _Hide(const ValueList& args, KValueRef result);
};

typedef AutoPtr<Notification> AutoNotification;

}

#endif
