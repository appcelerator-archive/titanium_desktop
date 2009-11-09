/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _DESKTOP_BINDING_H_
#define _DESKTOP_BINDING_H_

#include <kroll/kroll.h>
#include <map>
#include <vector>
#include <string>

using namespace kroll;

namespace ti
{
	class DesktopBinding : public StaticBoundObject
	{
	public:
		DesktopBinding(KObjectRef);
	protected:
		virtual ~DesktopBinding();
	private:
		KObjectRef global;
		void CreateShortcut(const ValueList& args, KValueRef result);
		void OpenApplication(const ValueList& args, KValueRef result);
		void OpenURL(const ValueList& args, KValueRef result);
		void TakeScreenshot(const ValueList& args, KValueRef result);
	};
}

#endif
