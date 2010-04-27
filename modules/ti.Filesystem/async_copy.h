/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#ifndef _TI_ASYNC_COPY_H
#define _TI_ASYNC_COPY_H

#include <kroll/kroll.h>

#ifdef OS_WIN32
#include <windows.h>
#elif OS_OSX
#import <Foundation/Foundation.h>
#endif

#include <string>
#include <vector>
#include <Poco/Thread.h>
#include <Poco/Exception.h>
#include <Poco/Path.h>
#include <Poco/File.h>
#include "filesystem_binding.h"


namespace ti
{
	class AsyncCopy : public StaticBoundObject
	{
	public:
		AsyncCopy(FilesystemBinding* parent,kroll::Host *host,std::vector<std::string> files, std::string destination, KMethodRef callback);
		virtual ~AsyncCopy();

	private:
		FilesystemBinding* parent;
		Host *host;
		std::vector<std::string> files;
		std::string destination;
		KMethodRef callback;
		Poco::Thread *thread;
		bool stopped;

		static void Run(void*);

		void ToString(const ValueList& args, KValueRef result);
		void Cancel(const ValueList& args, KValueRef result);
		void Copy(Poco::Path &src, Poco::Path &dest);
	};
}

#endif
