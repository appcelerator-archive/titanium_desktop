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
	
	bool Win32WebKitResourceLoadDelegate::CanPreprocess(Poco::URI& uri)
	{
		std::vector<std::string> pathSegments;
		uri.getPathSegments(pathSegments);
		if (pathSegments.size() == 0) return false;
		
		if (uri.getScheme() == "app" || uri.getScheme() == "ti" || uri.getScheme() == "file")
		{
			std::string& resource = pathSegments.at(pathSegments.size()-1);
			if (resource.find(".php") == resource.size()-4)
			{
				return true;
			}
		}
		return false;
	}
	
	std::string Win32WebKitResourceLoadDelegate::Preprocess(Poco::URI& uri, SharedKObject headers, std::string& method)
	{
		std::string path = URLUtils::URLToPath(uri.toString());
		
		ValueList args;
		args.push_back(Value::NewString(uri.toString()));
		args.push_back(Value::NewObject(headers));
		args.push_back(Value::NewString(method));
		args.push_back(Value::NewString(path));
		
		SharedValue preprocessed = Host::GetInstance()->GetGlobalObject()->CallNS("PHP.preprocess", args);
		if (!preprocessed->IsNull() && !preprocessed->IsUndefined())
		{
			Poco::File tempFile(Poco::TemporaryFile::tempName()+".html");
			Poco::TemporaryFile::registerForDeletion(tempFile.path());
			tempFile.createFile();
			
			std::ofstream ostream(tempFile.path().c_str());
			ostream << preprocessed->ToString();
			ostream.close();
			
			return URLUtils::PathToFileURL(tempFile.path());
			//const char *tempURLStr = strdup(tempURL.c_str());
			/*return tempURL;
			
			_bstr_t url(tempURL.c_str());
			
			IWebMutableURLRequest *mutableRequest;
			if (SUCCEEDED(request->QueryInterface(IID_IWebMutableURLRequest, (void **)&mutableRequest)))
			{
				Logger::Get("UI.Win32WebKitResourceLoadDelegate")->Debug("preprocessed %s into => %s", uri.toString().c_str(), tempURL.c_str());
				mutableRequest->setURL(url.copy());
			}*/
		}
		return "";
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
		Poco::URI uri(urlStr);
		if (!this->CanPreprocess(uri))
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
		/*if (FAILED(request->HTTPBodyStream(&httpBodyStream))
		{
			return S_OK;
		}*/
		
		std::string httpMethod = _bstr_t(httpMethodBStr);
		SharedKObject headers = new StaticBoundObject();
			
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
		
		std::string newURL = this->Preprocess(uri, headers, httpMethod);
		*newRequest = createWebURLRequest();
		IWebMutableURLRequest *mutableRequest;
		if (SUCCEEDED((*newRequest)->QueryInterface(IID_IWebMutableURLRequest, (void **)&mutableRequest)))
		{
			Logger::Get("UI.Win32WebKitResourceLoadDelegate")->Debug("preprocessed %s into => %s", uri.toString().c_str(), newURL.c_str());
			_bstr_t newURLBStr(newURL.c_str());
			mutableRequest->setURL(newURLBStr.copy());
		}
		
		return S_OK;
	}
}