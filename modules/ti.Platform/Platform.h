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

#ifndef Platform_h
#define Platform_h

#include <kroll/kroll.h>

namespace Titanium {

class Platform : public KAccessorObject {
public:
	Platform();

private:
	virtual ~Platform();
	std::string GetVersionImpl();
	bool OpenApplicationImpl(const std::string& path);
	bool OpenURLImpl(const std::string& url);
	void TakeScreenshotImpl(const std::string& targetFile);

	void _GetType(const ValueList& args, KValueRef result);
	void _GetName(const ValueList& args, KValueRef result);
	void _GetProcessorCount(const ValueList& args, KValueRef result);
	void _GetVersion(const ValueList& args, KValueRef result);
	void _GetArchitecture(const ValueList& args, KValueRef result);
	void _GetMachineId(const ValueList& args, KValueRef result);
	void _GetUsername(const ValueList& args, KValueRef result);
	void _CreateUUID(const ValueList& args, KValueRef result);
	void _OpenURL(const ValueList& args, KValueRef result);
	void _OpenApplication(const ValueList& args, KValueRef result);
	void _TakeScreenshot(const ValueList& args, KValueRef result);
};

} // namespace Titanium

#endif
