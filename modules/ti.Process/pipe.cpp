/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#include "pipe.h"
#include <vector>
#include <cstring>

namespace ti
{
	Pipe::Pipe(const char *type) : StaticBoundObject(type), onClose(NULL)
	{
		sharedThis = this;
		
		//TODO doc me
		SetMethod("close", &Pipe::_Close);
		SetMethod("isClosed", &Pipe::_IsClosed);
	}
	
	void Pipe::Closed()
	{
		if (onClose != NULL && !onClose->isNull())
		{
			SharedKObject event = new StaticBoundObject();
			event->SetObject("pipe", sharedThis);
			ValueList args;
			args.push_back(Value::NewObject(event));
			
			Host::GetInstance()->InvokeMethodOnMainThread(*onClose, args, false);
		}
		
		//sharedThis = NULL;
	}
	
	void Pipe::_Close(const ValueList& args, SharedValue result)
	{
		this->Close();
	}
	
	void Pipe::_IsClosed(const ValueList& args, SharedValue result)
	{
		result->SetBool(this->IsClosed());
	}
	
	void Pipe::_SetOnClose(const ValueList& args, SharedValue result)
	{
		if (args.size() > 0 && args.at(0)->IsMethod())
		{
			this->onClose = new SharedKMethod(args.at(0)->ToMethod());
		}
	}
}
