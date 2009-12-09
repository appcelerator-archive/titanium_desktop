/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#ifndef _TI_WEBKIT_RESOURCE_LOAD_DELEGATE_H_
#define _TI_WEBKIT_RESOURCE_LOAD_DELEGATE_H_

namespace ti
{

class Win32WebKitResourceLoadDelegate : public IWebResourceLoadDelegate
{
public:
	Win32WebKitResourceLoadDelegate(Win32UserWindow* userWindow);

	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject);
	virtual ULONG STDMETHODCALLTYPE AddRef(void);
	virtual ULONG STDMETHODCALLTYPE Release(void);

	virtual HRESULT STDMETHODCALLTYPE identifierForInitialRequest(
		/* [in] */ IWebView *webView,
		/* [in] */ IWebURLRequest *request,
		/* [in] */ IWebDataSource *dataSource,
		/* [in] */ unsigned long identifier);

	virtual HRESULT STDMETHODCALLTYPE willSendRequest(
		/* [in] */ IWebView *webView,
		/* [in] */ unsigned long identifier,
		/* [in] */ IWebURLRequest *request,
		/* [in] */ IWebURLResponse *redirectResponse,
		/* [in] */ IWebDataSource *dataSource,
		/* [retval][out] */ IWebURLRequest **newRequest);

	virtual HRESULT STDMETHODCALLTYPE didReceiveAuthenticationChallenge(
		/* [in] */ IWebView *webView,
		/* [in] */ unsigned long identifier,
		/* [in] */ IWebURLAuthenticationChallenge *challenge,
		/* [in] */ IWebDataSource *dataSource);

	virtual HRESULT STDMETHODCALLTYPE didCancelAuthenticationChallenge(
		/* [in] */ IWebView *webView,
		/* [in] */ unsigned long identifier,
		/* [in] */ IWebURLAuthenticationChallenge *challenge,
		/* [in] */ IWebDataSource *dataSource);

	virtual HRESULT STDMETHODCALLTYPE didReceiveResponse(
		/* [in] */ IWebView *webView,
		/* [in] */ unsigned long identifier,
		/* [in] */ IWebURLResponse *response,
		/* [in] */ IWebDataSource *dataSource);

	virtual HRESULT STDMETHODCALLTYPE didReceiveContentLength(
		/* [in] */ IWebView *webView,
		/* [in] */ unsigned long identifier,
		/* [in] */ UINT length,
		/* [in] */ IWebDataSource *dataSource);

	virtual HRESULT STDMETHODCALLTYPE didFinishLoadingFromDataSource(
		/* [in] */ IWebView *webView,
		/* [in] */ unsigned long identifier,
		/* [in] */ IWebDataSource *dataSource);

	virtual HRESULT STDMETHODCALLTYPE didFailLoadingWithError(
		/* [in] */ IWebView *webView,
		/* [in] */ unsigned long identifier,
		/* [in] */ IWebError *error,
		/* [in] */ IWebDataSource *dataSource);

	virtual HRESULT STDMETHODCALLTYPE plugInFailedWithError(
		/* [in] */ IWebView *webView,
		/* [in] */ IWebError *error,
		/* [in] */ IWebDataSource *dataSource);

private:
	Win32UserWindow* userWindow;
	int refCount;
};

}

#endif
