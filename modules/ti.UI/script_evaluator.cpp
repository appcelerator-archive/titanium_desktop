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
	void ScriptEvaluator::Initialize()
	{
		static AutoPtr<ScriptEvaluator> instance(new ScriptEvaluator());
#if defined(OS_OSX)
		[WebScriptElement addScriptEvaluator:[[OSXScriptEvaluator alloc]
			initWithEvaluator:instance.get()]];
#elif defined(OS_WIN32)
		addScriptEvaluator(instance.get());
#elif defined(OS_LINUX)
		webkit_titanium_add_script_evaluator(instance.get());
#endif
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
	return kroll::Script::GetInstance()->CanEvaluate([mimeType UTF8String]);
}

-(void) evaluate:(NSString *)mimeType sourceCode:(NSString*)sourceCode context:(void *)context
{
	// TODO get source name from webkit
	try
	{
		kroll::Script::GetInstance()->Evaluate([mimeType UTF8String], "<script>",
			[sourceCode UTF8String], JSContextToKrollContext(context));
	}
	catch (ValueException& exception)
	{
		Logger::Get("UI.ScriptEvaluator")->Error("Script evaluation failed: %s",
			exception.ToString().c_str());
	}
}
@end
#endif
