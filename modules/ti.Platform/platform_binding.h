/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _PLATFORM_BINDING_H_
#define _PLATFORM_BINDING_H_
#include <kroll/kroll.h>

namespace ti
{
	class PlatformBinding : public KAccessorObject
	{
	public:
		PlatformBinding();

	private:
		virtual ~PlatformBinding();
		std::string GetVersionImpl();
		bool OpenApplicationImpl(const std::string& path);
		bool OpenURLImpl(const std::string& url);
		void TakeScreenshotImpl(const std::string& targetFile);

		void _GetType(const ValueList& args, KValueRef result);
		void _GetName(const ValueList& args, KValueRef result);
		void _GetProcessorCount(const ValueList& args, KValueRef result);
		void _GetVersion(const ValueList& args, KValueRef result);
		void _GetArchitecture(const ValueList& args, KValueRef result);
		void _GetMachineId(const ValueList& args, KValueRef result);
		void _GetUsername(const ValueList& args, KValueRef result);
		void _CreateUUID(const ValueList& args, KValueRef result);
		void _OpenURL(const ValueList& args, KValueRef result);
		void _OpenApplication(const ValueList& args, KValueRef result);
		void _TakeScreenshot(const ValueList& args, KValueRef result);
	};
}

#endif
