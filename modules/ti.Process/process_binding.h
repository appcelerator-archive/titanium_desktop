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
	class ProcessBinding : public KAccessorObject
	{
	public:
		ProcessBinding();
		virtual ~ProcessBinding();
		static std::map<std::string,int> signals;

	private:
		void CreateProcess(const ValueList& args, KValueRef result);
		void CreatePipe(const ValueList& args, KValueRef result);
		void GetCurrentProcess(const ValueList& args, KValueRef result);
		void ExtendArgs(KListRef dest, KListRef args);
	};
}

#endif
