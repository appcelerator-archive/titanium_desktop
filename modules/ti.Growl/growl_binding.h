/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _GROWL_BINDING_H_
#define _GROWL_BINDING_H_

#include <kroll/kroll.h>

using namespace kroll;

namespace ti
{
	class GrowlBinding : public StaticBoundObject
	{
	public:
		GrowlBinding(SharedKObject);

		void ShowNotification(const ValueList& args, SharedValue result);
		void IsRunning(const ValueList& args, SharedValue result);
	protected:
		kroll::SharedKObject global;

		virtual ~GrowlBinding();
		virtual void ShowNotification(
			std::string& title,
			std::string& description,
			std::string& iconURL,
			int notification_delay,
			SharedKMethod callback) = 0;
		virtual bool IsRunning() = 0;
	};
}

#endif
