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
		DesktopBinding(SharedKObject);
	protected:
		virtual ~DesktopBinding();
	private:
		SharedKObject global;
		void CreateShortcut(const ValueList& args, SharedValue result);
		void OpenApplication(const ValueList& args, SharedValue result);
		void OpenURL(const ValueList& args, SharedValue result);
		void TakeScreenshot(const ValueList& args, SharedValue result);
	};
}

#endif
