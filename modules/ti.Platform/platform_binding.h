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
	class PlatformBinding : public StaticBoundObject
	{
	public:
		PlatformBinding(KObjectRef);
	protected:
		virtual ~PlatformBinding();
	private:
		KObjectRef global;
		
		void CreateUUID(const ValueList& args, KValueRef result);
	};
}

#endif
