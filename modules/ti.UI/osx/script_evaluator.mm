/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */
#include <kroll/kroll.h>
#include "../ui_module.h"
#include "osx_menu_item.h"
#include <WebKit/WebFramePrivate.h>
#include <WebKit/WebScriptElement.h>

@implementation ScriptEvaluator
	-(ScriptEvaluator*)initWithHost:(Host*)hostin
	{
		self = [self init];
		host = hostin;
		[WebScriptElement addScriptEvaluator:self];
		return self;
	}
	-(NSString*) getModuleName:(NSString*)typeStr
	{
		std::string type = [typeStr UTF8String];
		if (type.find("text/") == 0) {
			type = type.substr(5);
		}
		std::string moduleName = "";
		moduleName += (char)std::toupper(type.at(0));
		moduleName += type.substr(1);
		return [[NSString stringWithUTF8String:moduleName.c_str()] retain];
	}
	
	-(BOOL) matchesMimeType:(NSString*)mimeType
	{
		NSString* moduleName = [self getModuleName:mimeType];
		SharedKObject global = host->GetGlobalObject();
		SharedValue moduleValue = global->Get([moduleName UTF8String]);

		[moduleName release];
		return (moduleValue->IsObject() && moduleValue->ToObject()->Get("evaluate")->IsMethod());
	}
	
	-(void) evaluate:(NSString *)mimeType sourceCode:(NSString*)sourceCode context:(void *)context
	{
		std::string type = [mimeType UTF8String];
		NSString* moduleName = [self getModuleName:mimeType];
		SharedKObject global = host->GetGlobalObject();
		SharedValue moduleValue = global->Get([moduleName UTF8String]);
		[moduleName release];

		JSContextRef contextRef = reinterpret_cast<JSContextRef>(context);
		if (!moduleValue->IsNull()) 
		{
			SharedValue evalValue = moduleValue->ToObject()->Get("evaluate");
			if (evalValue->IsMethod()) 
			{
				ValueList args;
				SharedValue typeValue = Value::NewString(type);
				SharedValue sourceCodeValue = Value::NewString([sourceCode UTF8String]);
				JSObjectRef globalObjectRef = JSContextGetGlobalObject(contextRef);
				SharedKObject contextObject = new KKJSObject(contextRef, globalObjectRef);
				SharedValue contextValue = Value::NewObject(contextObject);
				args.push_back(typeValue);
				args.push_back(sourceCodeValue);
				args.push_back(contextValue);
				
				//TODO: on error, should we call window.onerror function if found?
				try
				{
					evalValue->ToMethod()->Call(args);
				}
				catch(ValueException &e)
				{
					SharedString s = e.GetValue()->DisplayString();
					Logger* logger = Logger::Get("UI.ScriptEvaluator");
					logger->Error("Exception evaluating %s. Error: %s", type.c_str(), (*s).c_str());
				}
				catch(std::exception &e)
				{
					Logger* logger = Logger::Get("UI.ScriptEvaluator");
					logger->Error("Exception evaluating %s. Error: %s", type.c_str(), e.what());
				}
				catch(...)
				{
					Logger* logger = Logger::Get("UI.ScriptEvaluator");
					logger->Error("Exception evaluating %s. Unknown Error.", type.c_str());
				}
			}
		}
		else
		{
			Logger* logger = Logger::Get("UI.ScriptEvaluator");
			logger->Error("Couldn't find script type bound object for %s", type.c_str());
		}
	}
@end
