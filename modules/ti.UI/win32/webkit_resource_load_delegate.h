/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef TI_WIN32_WEBKIT_RESOURCE_LOAD_DELEGATE_H
#define TI_WIN32_WEBKIT_RESOURCE_LOAD_DELEGATE_H

#include <Poco/URI.h>

namespace ti
{
	class Win32UserWindow;
	class Win32WebKitResourceLoadDelegate : public IWebResourceLoadDelegate, public COMReferenceCounted
	{
	public:
		Win32WebKitResourceLoadDelegate(Win32UserWindow *window);
		
		// IUnknown
		virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject)
		{
			if (IsEqualGUID(riid, IID_IWebResourceLoadDelegate))
			{
				*ppvObject = static_cast<IWebResourceLoadDelegate*>(this);
				return S_OK;
			}
			return COMReferenceCounted::QueryInterface(riid, ppvObject);
		}
		
		virtual ULONG STDMETHODCALLTYPE AddRef(void)
		{
			return COMReferenceCounted::AddRef();
		}
		
		virtual ULONG STDMETHODCALLTYPE Release(void)
		{
			return COMReferenceCounted::Release();
		}
		
		// IWebResourceLoadDelegate
		virtual HRESULT STDMETHODCALLTYPE identifierForInitialRequest(
			/*[in]*/ IWebView* webView,
			/*[in]*/ IWebURLRequest* request,
			/*[in]*/ IWebDataSource* dataSource,
			/*[in]*/ unsigned long identifier)
		{
			return S_OK;
		}
		
		virtual HRESULT STDMETHODCALLTYPE willSendRequest(
			/*[in]*/ IWebView* webView,
			/*[in]*/ unsigned long identifier,
			/*[in]*/ IWebURLRequest* request,
			/*[in]*/ IWebURLResponse* redirectResponse,
			/*[in]*/ IWebDataSource* dataSource,
			/*[out, retval]*/ IWebURLRequest** newRequest);
		
		virtual HRESULT STDMETHODCALLTYPE didReceiveAuthenticationChallenge(
			/*[in]*/ IWebView* webView, 
			/*[in]*/ unsigned long identifier,
			/*[in]*/ IWebURLAuthenticationChallenge* challenge,
			/*[in]*/ IWebDataSource* dataSource)
		{
			return S_OK;
		}
		
		virtual HRESULT STDMETHODCALLTYPE didCancelAuthenticationChallenge(
			/*[in]*/ IWebView* webView,
			/*[in]*/ unsigned long identifier,
			/*[in]*/ IWebURLAuthenticationChallenge* challenge,
			/*[in]*/ IWebDataSource* dataSource)
		{
			return S_OK;
		}
		
		virtual HRESULT STDMETHODCALLTYPE didReceiveResponse(
			/*[in]*/ IWebView* webView,
			/*[in]*/ unsigned long identifier,
			/*[in]*/ IWebURLResponse* response,
			/*[in]*/ IWebDataSource* dataSource)
		{
			BSTR urlBStr;
			response->URL(&urlBStr);
			std::string url = _bstr_t(urlBStr);
			
			Logger::Get("Win32.WebKitResourceLoadDelegate")->Debug("didReceiveResponse: id: %d, resonse url: %s", identifier, url.c_str());
			return S_OK;
		}
		
		virtual HRESULT STDMETHODCALLTYPE didReceiveContentLength(
			/*[in]*/ IWebView* webView,
			/*[in]*/ unsigned long identifier,
			/*[in]*/ UINT length,
			/*[in]*/ IWebDataSource* dataSource)
		{
			return S_OK;
		}
		
		virtual HRESULT STDMETHODCALLTYPE didFinishLoadingFromDataSource(
			/*[in]*/ IWebView* webView,
			/*[in]*/ unsigned long identifier,
			/*[in]*/ IWebDataSource* dataSource)
		{
			return S_OK;
		}
		
		virtual HRESULT STDMETHODCALLTYPE didFailLoadingWithError(
			/*[in]*/ IWebView* webView,
			/*[in]*/ unsigned long identifier,
			/*[in]*/ IWebError* error,
			/*[in]*/ IWebDataSource* dataSource)
		{
			return S_OK;
		}
		
		virtual HRESULT STDMETHODCALLTYPE plugInFailedWithError(
			/*[in]*/ IWebView* webView,
			/*[in]*/ IWebError* error,
			/*[in]*/ IWebDataSource* dataSource)
		{
			return S_OK;
		}
		
	protected:
		Win32UserWindow *window;
		bool CanPreprocess(Poco::URI& uri);
		std::string Preprocess(Poco::URI& uri, SharedKObject headers, std::string& method);
	};
}

#endif