/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#ifndef _PROCESS_BINDING_H_
#define _PROCESS_BINDING_H_

#include <kroll/kroll.h>
#include <vector>
#include "process.h"
#include "pipe.h"

namespace ti
{
	class ProcessBinding : public StaticBoundObject
	{
	public:
		ProcessBinding();
		virtual ~ProcessBinding();
			
	private:
		std::map<std::string,int> signals;
		std::vector<SharedPtr<Process> > processes;
		std::vector<SharedPtr<Pipe> > pipes;
		
		void CreateProcess(const ValueList& args, SharedValue result);
		void CreateInputPipe(const ValueList& args, SharedValue result);
		void CreateOutputPipe(const ValueList& args, SharedValue result);
		void GetCurrentProcess(const ValueList& args, SharedValue result);
		
	};
}

#endif
