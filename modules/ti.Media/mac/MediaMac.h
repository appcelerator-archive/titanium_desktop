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

#ifndef MediaMac_h
#define MediaMac_h

#include <kroll/kroll.h>

#include "../Media.h"

using namespace kroll;

namespace Titanium {

class MediaMac : public Media {
public:
	MediaMac(KObjectRef);
	~MediaMac();

protected:
	void Beep();
	KObjectRef CreateSound(std::string&);
};

} // namespace Titanium

#endif
