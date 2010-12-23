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

#include "Media.h"

#include <cstring>

#include <Poco/URI.h>

namespace Titanium {

Media::Media(KObjectRef global) :
	StaticBoundObject("Media"),
	global(global)
{
	/**
	 * @tiapi(method=True,name=Media.createSound,since=0.2) Creates a sound object
	 * @tiarg(for=Media.createSound,name=path,type=String) path or url to the sound file
	 * @tiresult(for=Media.createSound,type=Media.Sound) a Sound object
	 */
	this->SetMethod("createSound", &Media::_CreateSound);
	/**
	 * @tiapi(method=True,name=Media.beep,since=0.2) Causes the system to beep
	 */
	this->SetMethod("beep", &Media::_Beep);
}

Media::~Media()
{
}

void Media::_CreateSound(const ValueList& args, KValueRef result)
{
	if (args.size()!=1)
		throw ValueException::FromString("createSound takes 1 parameter");

	std::string path(args.at(0)->ToString());
	result->SetObject(this->CreateSound(path));
}

void Media::_Beep(const ValueList& args, KValueRef result)
{
	this->Beep();
}

} // namespace Titanium
