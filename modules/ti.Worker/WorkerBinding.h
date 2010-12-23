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

#ifndef WorkerBinding_h
#define WorkerBinding_h

#include <kroll/kroll.h>

namespace Titanium {

class WorkerBinding : public StaticBoundObject {
public:
	WorkerBinding();
	~WorkerBinding();

private:
	void _CreateWorker(const ValueList& args, KValueRef result);
};

} // namespace Titanium

#endif
