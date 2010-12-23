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

#include "ScriptEvaluator.h"

#include <algorithm>
#include <cstring>

#include <kroll/javascript/javascript_module.h>

namespace Titanium {

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

} // namespace Titanium

#if defined(OS_OSX)
@implementation OSXScriptEvaluator
-(OSXScriptEvaluator*) initWithEvaluator:(Titanium::ScriptEvaluator*)evaluator
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
		Script::GetInstance()->Evaluate([mimeType UTF8String], "<script>",
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
