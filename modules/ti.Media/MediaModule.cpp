/*
 * Copyright (c) 2008-2010 Appcelerator, Inc. All Rights Reserved.
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

#include "MediaModule.h"

#include "Media.h"

#ifdef OS_LINUX
#include "gstreamer/MediaGStreamer.h"
#elif OS_WIN32
#include "win/MediaWin.h"
#elif OS_OSX
#include "mac/MediaMac.h"
#endif

namespace Titanium {

KROLL_MODULE(MediaModule, STRING(MODULE_NAME), STRING(MODULE_VERSION));

void MediaModule::Initialize()
{
#ifdef OS_LINUX
	this->binding = new MediaGStreamer(host->GetGlobalObject());
#elif OS_WIN32
	this->binding = new MediaWin(host->GetGlobalObject());
#elif OS_OSX
	this->binding = new MediaMac(host->GetGlobalObject());
#endif
	KValueRef value = Value::NewObject(this->binding);
	host->GetGlobalObject()->Set("Media", value);
}

void MediaModule::Stop()
{
}

} // namespace Titanium
