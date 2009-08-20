/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */
#include "../ui_module.h"

#include <algorithm>

namespace ti
{
	ScriptEvaluator::~ScriptEvaluator()
	{
		// TODO Auto-generated destructor stub
	}

	std::string ScriptEvaluator::BSTRToString(BSTR bstr)
	{
		std::string str = _bstr_t(bstr);
		return str;
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
		std::transform(moduleName.begin(), moduleName.end(), moduleName.begin(), std::toupper);
		moduleValue = global->Get(moduleName.c_str());
		
		if (!moduleValue->IsNull() && moduleValue->IsObject())
		{
			return moduleValue;
		}
		
		return Value::Undefined;
	}

	HRESULT STDMETHODCALLTYPE
	ScriptEvaluator::matchesMimeType(BSTR mimeType, BOOL *result)
	{
		SharedValue moduleValue = FindScriptModule(BSTRToString(mimeType));
		*result = FALSE;
		if (!moduleValue->IsUndefined())
		{
			if (!moduleValue->ToObject()->GetMethod("evaluate").isNull())
			{
				*result = TRUE;
			}
		}

		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE
	ScriptEvaluator::evaluate(BSTR mimeType, BSTR sourceCode, int *context)
	{
		std::string type = BSTRToString(mimeType);
		JSContextRef contextRef = reinterpret_cast<JSContextRef>(context);
		SharedValue moduleValue = FindScriptModule(type);
		
		if (!moduleValue->IsNull())
		{
			SharedKMethod evalMethod = moduleValue->ToObject()->GetMethod("evaluate");
			if (!evalMethod.isNull())
			{
				ValueList args;
				SharedValue typeValue = Value::NewString(type);
				SharedValue sourceCodeValue = Value::NewString(BSTRToString(sourceCode));
				JSObjectRef globalObjectRef = JSContextGetGlobalObject(contextRef);
				SharedKObject contextObject = new KKJSObject(contextRef, globalObjectRef);
				SharedValue contextValue = Value::NewObject(contextObject);
				args.push_back(typeValue);
				args.push_back(sourceCodeValue);
				args.push_back(contextValue);

				evalMethod->Call(args);
			}
		}

		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE
	ScriptEvaluator::QueryInterface(REFIID riid, void **ppvObject)
	{
		*ppvObject = 0;
		if (IsEqualGUID(riid, IID_IUnknown))
		{
			*ppvObject = static_cast<IUnknown*>(this);
		}
		else
		{
			return E_NOINTERFACE;
		}
		return S_OK;
	}

	ULONG STDMETHODCALLTYPE
	ScriptEvaluator::AddRef()
	{
		return ++ref_count;
	}

	ULONG STDMETHODCALLTYPE
	ScriptEvaluator::Release()
	{
		ULONG new_count = --ref_count;
		if (!new_count) delete(this);

		return new_count;
	}
}