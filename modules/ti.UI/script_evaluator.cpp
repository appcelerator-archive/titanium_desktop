/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#include "ui_module.h"
#include <algorithm>
#include <cstring>

namespace ti
{
	/*static*/
	AutoPtr<ScriptEvaluator> ScriptEvaluator::instance;

	/*static*/
	void ScriptEvaluator::Initialize()
	{
		instance = new ScriptEvaluator();
#if defined(OS_OSX)
		OSXScriptEvaluator *evaluator = [[OSXScriptEvaluator alloc] initWithEvaluator:instance.get()];
		[WebScriptElement addScriptEvaluator:evaluator];
#else
		addScriptEvaluator(instance.get());
#endif
	}
	
	SharedValue ScriptEvaluator::FindScriptModule(std::string type)
	{
		if (type.find("text/") == 0)
		{
			type = type.substr(5);
		}
		
		// Try uppercasing the first char
		std::string moduleName = "";
		moduleName += (char)std::toupper(type.at(0));
		moduleName += type.substr(1);
		
		SharedKObject global = Host::GetInstance()->GetGlobalObject();
		SharedValue moduleValue = global->Get(moduleName.c_str());
		if (!moduleValue->IsNull() && moduleValue->IsObject())
		{
			return moduleValue;
		}
		
		// Now try all uppercase (I'm looking at you PHP)
		std::transform(moduleName.begin(), moduleName.end(), moduleName.begin(), toupper);
		moduleValue = global->Get(moduleName.c_str());
		if (!moduleValue->IsNull() && moduleValue->IsObject())
		{
			return moduleValue;
		}
		
		// And lastly, we can support all lowercase too right?
		std::transform(moduleName.begin(), moduleName.end(), moduleName.begin(), tolower);
		moduleValue = global->Get(moduleName.c_str());
		if (!moduleValue->IsNull() && moduleValue->IsObject())
		{
			return moduleValue;
		}
		
		return Value::Undefined;
	}

	bool ScriptEvaluator::MatchesMimeType(std::string mimeType)
	{
		SharedValue moduleValue = FindScriptModule(mimeType);
		if (!moduleValue->IsUndefined())
		{
			if (!moduleValue->ToObject()->GetMethod("evaluate").isNull())
			{
				return true;
			}
		}
		return false;
	}
	
	SharedValue ScriptEvaluator::Evaluate(std::string mimeType,
		std::string sourceCode, JSContextRef context)
	{
		SharedValue moduleValue = FindScriptModule(mimeType);
		if (!moduleValue->IsNull())
		{
			SharedKMethod evalMethod = moduleValue->ToObject()->GetMethod("evaluate");
			if (!evalMethod.isNull())
			{
				ValueList args;
				SharedValue typeValue = Value::NewString(mimeType);
				SharedValue sourceCodeValue = Value::NewString(sourceCode);
				JSObjectRef globalObjectRef = JSContextGetGlobalObject(context);
				SharedKObject contextObject = new KKJSObject(context, globalObjectRef);
				SharedValue contextValue = Value::NewObject(contextObject);
				args.push_back(typeValue);
				args.push_back(sourceCodeValue);
				args.push_back(contextValue);

				return evalMethod->Call(args);
			}
		}
		
		return Value::Undefined;
	}
}

#if defined(OS_OSX)
@implementation OSXScriptEvaluator
-(OSXScriptEvaluator*) initWithEvaluator:(ti::ScriptEvaluator*)evaluator
{
	self = [self init];
	delegate = evaluator;
	return self;
}
-(BOOL) matchesMimeType:(NSString*)mimeType
{
	return delegate->MatchesMimeType([mimeType UTF8String]);
}
-(void) evaluate:(NSString *)mimeType sourceCode:(NSString*)sourceCode context:(void *)context
{
	delegate->Evaluate([mimeType UTF8String], [sourceCode UTF8String], reinterpret_cast<JSContextRef>(context));
}
@end
#endif
