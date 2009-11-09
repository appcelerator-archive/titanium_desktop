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
#include "growl_delegate.h"

namespace ti {
	class GrowlOSX : public GrowlBinding {
	protected:
		TiGrowlDelegate *delegate;

	public:
		GrowlOSX(KObjectRef global);
		virtual ~GrowlOSX();

		void CopyToApp(kroll::Host *host, kroll::Module *module);
		virtual void ShowNotification(std::string& title,
			std::string& description, std::string& iconURL,
			int notification_delay, KMethodRef callback);
		virtual bool IsRunning();
	};
}

#endif /* GROWL_OSX_H_ */
