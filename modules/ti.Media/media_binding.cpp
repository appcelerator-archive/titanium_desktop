/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */
#include "media_binding.h"
#include <kroll/kroll.h>
#include <cstring>
#include <Poco/URI.h>

namespace ti
{
	MediaBinding::MediaBinding(KObjectRef global) :
		StaticBoundObject("Media"),
		global(global)
	{
		/**
		 * @tiapi(method=True,name=Media.createSound,since=0.2) Creates a sound object
		 * @tiarg(for=Media.createSound,name=path,type=String) path or url to the sound file
		 * @tiresult(for=Media.createSound,type=Media.Sound) a Sound object
		 */
		this->SetMethod("createSound", &MediaBinding::_CreateSound);
		/**
		 * @tiapi(method=True,name=Media.beep,since=0.2) Causes the system to beep
		 */
		this->SetMethod("beep", &MediaBinding::_Beep);
	}

	MediaBinding::~MediaBinding()
	{
	}

	void MediaBinding::_CreateSound(const ValueList& args, KValueRef result)
	{
		if (args.size()!=1)
			throw ValueException::FromString("createSound takes 1 parameter");

		std::string path(args.at(0)->ToString());
		result->SetObject(this->CreateSound(path));
	}

	void MediaBinding::_Beep(const ValueList& args, KValueRef result)
	{
		this->Beep();
	}
}
