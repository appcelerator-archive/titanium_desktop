/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#ifndef _PROCESS_BINDING_H_
#define _PROCESS_BINDING_H_

#include <kroll/kroll.h>
#include <vector>

namespace ti
{
#ifdef OS_WIN32
	class Win32Process;
#else
	class Process;
#endif

	class Process;
	class ProcessBinding : public StaticBoundObject
	{
	public:
		ProcessBinding(Host *, SharedKObject);
		virtual ~ProcessBinding();
	
#ifdef OS_WIN32
		void Terminated(Win32Process* p);
#else
		void Terminated(Process* p);
#endif
		Host* GetHost() { return host; }
		
	private:
		Host *host;
		SharedKObject global;
		std::vector<SharedKObject> processes;
		
		void Launch(const ValueList& args, SharedValue result);
		void GetEnv(const ValueList& args, SharedValue result);
		void HasEnv(const ValueList& args, SharedValue result);
		void SetEnv(const ValueList& args, SharedValue result);
		void Restart(const ValueList& args, SharedValue result);
		
	};
}

#endif
