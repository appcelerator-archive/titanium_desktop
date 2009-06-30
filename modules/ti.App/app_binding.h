/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _APP_BINDING_H_
#define _APP_BINDING_H_

#include <kroll/kroll.h>


namespace ti
{
	class AppBinding : public StaticBoundObject
	{
	public:
		AppBinding(Host *host,SharedKObject);
		virtual ~AppBinding();
	private:
		Host *host;
		SharedKObject global;
		
		void GetID(const ValueList& args, SharedValue result);
		void GetName(const ValueList& args, SharedValue result);
		void GetVersion(const ValueList& args, SharedValue result);
		void GetPublisher(const ValueList& args, SharedValue result);
		void GetCopyright(const ValueList& args, SharedValue result);
		void GetDescription(const ValueList& args, SharedValue result);
		void GetURL(const ValueList& args, SharedValue result);
		void GetGUID(const ValueList& args, SharedValue result);
		void GetIcon(const ValueList& args, SharedValue result);
		void GetStreamURL(const ValueList& args, SharedValue result);
		void AppURLToPath(const ValueList& args, SharedValue result);
		void SetMenu(const ValueList& args, SharedValue result);
		void Exit(const ValueList& args, SharedValue result);
		void StdOut(const ValueList& args, SharedValue result);
		void StdErr(const ValueList& args, SharedValue result);

		void CreateProperties(const ValueList& args, SharedValue result);
		void LoadProperties(const ValueList& args, SharedValue result);
		void GetSystemProperties(const ValueList& args, SharedValue result);
	};
}

#endif
