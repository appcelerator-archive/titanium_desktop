/*
 * Copyright (c) 2009-2010 Appcelerator, Inc. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef Filesystem_h
#define Filesystem_h

#include <vector>

#include <kroll/kroll.h>
#include <Poco/Mutex.h>
#include <Poco/Timer.h>

namespace Titanium {

class Filesystem : public StaticBoundObject {
public:
	Filesystem(Host*, KObjectRef);
	virtual ~Filesystem();

private:
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

	Host *host;
	KObjectRef global;
	std::vector<KObjectRef> asyncOperations;
	Poco::Timer *timer;
};

} // namespace Titanium

#endif
