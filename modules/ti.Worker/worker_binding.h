/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _WORKER_BINDING_H_
#define _WORKER_BINDING_H_

#include <kroll/kroll.h>

namespace ti
{
	class WorkerBinding : public StaticBoundObject
	{
	public:
		WorkerBinding(Host*, KObjectRef);
	protected:
		virtual ~WorkerBinding();
	private:
		Host *host;
		KObjectRef global;
		
		void CreateWorker(const ValueList& args, KValueRef result);
	};
}

#endif
