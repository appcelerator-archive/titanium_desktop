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
		FilesystemBinding(Host*, SharedKObject);
		virtual ~FilesystemBinding();

		private:
		Host *host;
		SharedKObject global;
		std::vector<SharedKObject> asyncOperations;
		Poco::Timer *timer;

		void CreateTempFile(const ValueList& args, SharedValue result);
		void CreateTempDirectory(const ValueList& args, SharedValue result);
		void GetFile(const ValueList& args, SharedValue result);
		void GetFileStream(const ValueList& args, SharedValue result);
		void GetApplicationDirectory(const ValueList& args, SharedValue result);
		void GetApplicationDataDirectory(const ValueList& args, SharedValue result);
		void GetRuntimeHomeDirectory(const ValueList& args, SharedValue result);
		void GetResourcesDirectory(const ValueList& args, SharedValue result);
		void GetProgramsDirectory(const ValueList &args, SharedValue result);
		void GetDesktopDirectory(const ValueList& args, SharedValue result);
		void GetDocumentsDirectory(const ValueList& args, SharedValue result);
		void GetUserDirectory(const ValueList& args, SharedValue result);
		void GetLineEnding(const ValueList& args, SharedValue result);
		void GetSeparator(const ValueList& args, SharedValue result);
		void GetRootDirectories(const ValueList& args, SharedValue result);
		void ExecuteAsyncCopy(const ValueList& args, SharedValue result);

		//INTERNAL ONLY
		void OnAsyncOperationTimer(Poco::Timer &timer);
		void DeletePendingOperations(const ValueList& args, SharedValue result);
		void ResolveFileName(const ValueList& args, std::string& filename);
	};
}

#endif
