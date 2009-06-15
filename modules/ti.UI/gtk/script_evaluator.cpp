/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */

#include "../ui_module.h"

using namespace kroll;

ScriptEvaluator::~ScriptEvaluator() {
	// TODO Auto-generated destructor stub
}

std::string ScriptEvaluator::GetModuleName(std::string type)
{
	if (type.find("text/") == 0) {
		type = type.substr(5);
	}

	std::string moduleName = "";
	moduleName += (char)std::toupper(type.at(0));
	moduleName += type.substr(1);

	return moduleName;
}

bool ScriptEvaluator::matchesMimeType(const gchar *mime_type)
{
	std::string moduleName = GetModuleName(mime_type);
	SharedKObject global = kroll::Host::GetInstance()->GetGlobalObject();

	SharedValue moduleValue = global->Get(moduleName.c_str());
	if (!moduleValue->IsNull() && moduleValue->IsObject()) {
		if (!moduleValue->ToObject()->Get("evaluate")->IsNull()
				&& !moduleValue->ToObject()->Get("evaluate")->IsUndefined()
				&& moduleValue->ToObject()->Get("evaluate")->IsMethod()) {
			return true;
		}
	}

	return false;
}

void ScriptEvaluator::evaluate(const gchar *mime_type, const gchar *source_code, void *context)
{
	std::string moduleName = GetModuleName(mime_type);
	SharedKObject global = kroll::Host::GetInstance()->GetGlobalObject();
	JSContextRef contextRef = reinterpret_cast<JSContextRef>(context);

	SharedValue moduleValue = global->Get(moduleName.c_str());
	if (!moduleValue->IsNull()) {
		SharedValue evalValue = moduleValue->ToObject()->Get("evaluate");
		if (!evalValue->IsNull() && evalValue->IsMethod()) {
			ValueList args;
			SharedValue typeValue = Value::NewString(mime_type);
			SharedValue sourceCodeValue = Value::NewString(source_code);
			JSObjectRef globalObjectRef = JSContextGetGlobalObject(contextRef);
			SharedKObject contextObject = new KKJSObject(contextRef, globalObjectRef);
			SharedValue contextValue = Value::NewObject(contextObject);
			args.push_back(typeValue);
			args.push_back(sourceCodeValue);
			args.push_back(contextValue);

			evalValue->ToMethod()->Call(args);
		}
	}
}
