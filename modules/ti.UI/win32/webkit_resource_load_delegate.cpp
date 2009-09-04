/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#include "../ui_module.h"

#include <Poco/TemporaryFile.h>
#include <atlconv.h>

namespace ti
{

	Win32WebKitResourceLoadDelegate::Win32WebKitResourceLoadDelegate (Win32UserWindow *window) :
		window(window)
	{
	
	}
	
	std::string PropertyBagGetStringProperty(IPropertyBag *bag, const wchar_t *property)
	{
		VARIANT value;
		::VariantInit(&value);
		std::string result;
		
		if (bag->Read(property, &value, NULL) == S_OK)
		{
			result = _bstr_t(value.bstrVal);
		}
		result = "";
		::VariantClear(&value);
		
		return result;
	}
	
	HRESULT Win32WebKitResourceLoadDelegate::willSendRequest(
			/*[in]*/ IWebView* webView,
			/*[in]*/ unsigned long identifier,
			/*[in]*/ IWebURLRequest* request,
			/*[in]*/ IWebURLResponse* redirectResponse,
			/*[in]*/ IWebDataSource* dataSource,
			/*[out, retval]*/ IWebURLRequest** newRequest)
	{
		BSTR urlBStr;
		if (FAILED(request->URL(&urlBStr)))
		{
			return S_OK;
		}
		
		std::string urlStr = _bstr_t(urlBStr);
		if (!Script::GetInstance()->CanPreprocess(urlStr.c_str()))
		{
			return S_OK;
		}
		
		IPropertyBag *httpHeaders;
		BSTR httpMethodBStr;
		IStream *httpBodyStream;
		if (FAILED(request->allHTTPHeaderFields(&httpHeaders)))
		{
			return S_OK;
		}
		if (FAILED(request->HTTPMethod(&httpMethodBStr)))
		{
			return S_OK;
		}
		/* TODO: implement HTTPBodyStream in win32
		if (FAILED(request->HTTPBodyStream(&httpBodyStream))
		{
			return S_OK;
		}*/
		
		std::string httpMethod = _bstr_t(httpMethodBStr);
		SharedKObject scope = new StaticBoundObject();
		SharedKObject headers = new StaticBoundObject();
		scope->Set("httpHeaders", Value::NewObject(headers));
		
		IPropertyBag2* httpHeaders2;
		if (SUCCEEDED(httpHeaders->QueryInterface(IID_IPropertyBag2, (void **)&httpHeaders2)))
		{
			ULONG propertyCount;
			if (SUCCEEDED(httpHeaders2->CountProperties(&propertyCount)))
			{
				for (ULONG i = 0; i < propertyCount; i++)
				{
					ULONG infoCount;
					PROPBAG2 propBag;
					if (SUCCEEDED(httpHeaders2->GetPropertyInfo(i, 1, &propBag, &infoCount)))
					{
						std::wstring nameWString(propBag.pstrName);
						std::string name(nameWString.begin(), nameWString.end());
						
						std::string propertyValue = PropertyBagGetStringProperty(httpHeaders, propBag.pstrName);
						headers->Set(name.c_str(), Value::NewString(propertyValue));
					}
				}
			}
		}
		
		AutoPtr<PreprocessData> data =
			Script::GetInstance()->Preprocess(urlStr.c_str(), scope);
		//TODO: implement new preprocess stuff for win32
		/*if (!newURL.isNull())
		{
			*newRequest = createWebURLRequest();
			IWebMutableURLRequest *mutableRequest;
			if (SUCCEEDED((*newRequest)->QueryInterface(IID_IWebMutableURLRequest, (void **)&mutableRequest)))
			{
				Logger::Get("UI.Win32WebKitResourceLoadDelegate")->Debug("preprocessed %s into => %s", urlStr.c_str(), newURL->c_str());
				_bstr_t newURLBStr(newURL->c_str());
				mutableRequest->setURL(newURLBStr.copy());
			}
		}*/
		
		return S_OK;
	}
}
