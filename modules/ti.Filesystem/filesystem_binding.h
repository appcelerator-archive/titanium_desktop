/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _FILESYSTEM_BINDING_H_
#define _FILESYSTEM_BINDING_H_

#include <vector>
#include <kroll/kroll.h>
#include <Poco/Mutex.h>
#include <Poco/Timer.h>

namespace ti
{
	class FilesystemBinding : public StaticBoundObject
	{
		public:
		FilesystemBinding(Host*, KObjectRef);
		virtual ~FilesystemBinding();

		private:
		Host *host;
		KObjectRef global;
		std::vector<KObjectRef> asyncOperations;
		Poco::Timer *timer;

		void CreateTempFile(const ValueList& args, KValueRef result);
		void CreateTempDirectory(const ValueList& args, KValueRef result);
		void GetFile(const ValueList& args, KValueRef result);
		void GetFileStream(const ValueList& args, KValueRef result);
		void GetApplicationDirectory(const ValueList& args, KValueRef result);
		void GetApplicationDataDirectory(const ValueList& args, KValueRef result);
		void GetRuntimeHomeDirectory(const ValueList& args, KValueRef result);
		void GetResourcesDirectory(const ValueList& args, KValueRef result);
		void GetProgramsDirectory(const ValueList &args, KValueRef result);
		void GetDesktopDirectory(const ValueList& args, KValueRef result);
		void GetDocumentsDirectory(const ValueList& args, KValueRef result);
		void GetUserDirectory(const ValueList& args, KValueRef result);
		void GetLineEnding(const ValueList& args, KValueRef result);
		void GetSeparator(const ValueList& args, KValueRef result);
		void GetRootDirectories(const ValueList& args, KValueRef result);
		void ExecuteAsyncCopy(const ValueList& args, KValueRef result);

		//INTERNAL ONLY
		void OnAsyncOperationTimer(Poco::Timer &timer);
		void DeletePendingOperations(const ValueList& args, KValueRef result);
		void ResolveFileName(const ValueList& args, std::string& filename);
	};
}

#endif
