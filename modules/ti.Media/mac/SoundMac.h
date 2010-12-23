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

#ifndef SoundMac_h
#define SoundMac_h

#include <AppKit/NSSound.h>

#include <kroll/kroll.h>

#include "../Sound.h"

namespace Titanium {

class SoundMac : public Sound {
public:
	SoundMac(std::string& url);
	virtual ~SoundMac();

	virtual void LoadImpl();
	virtual void UnloadImpl();
	virtual void PlayImpl();
	virtual void PauseImpl();
	virtual void StopImpl();
	virtual void SetVolumeImpl(double volume);
	virtual double GetVolumeImpl();

private:
	NSSound* sound;
	id delegate;
	NSURL* fileURL;
};

} // namespace Titanium

#endif
