/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#include "../ui_module.h"

namespace ti
{
Win32WebKitResourceLoadDelegate::Win32WebKitResourceLoadDelegate(Win32UserWindow* userWindow) :
	userWindow(userWindow),
	refCount(1)
{

}

ULONG STDMETHODCALLTYPE Win32WebKitResourceLoadDelegate::AddRef()
{
	return ++refCount;
}

ULONG STDMETHODCALLTYPE Win32WebKitResourceLoadDelegate::Release()
{
	ULONG newCount = --refCount;
	if (!newCount)
		delete(this);
	return newCount;
}

HRESULT STDMETHODCALLTYPE Win32WebKitResourceLoadDelegate::QueryInterface(
	REFIID riid, void **ppvObject)
{
	*ppvObject = 0;
	if (IsEqualGUID(riid, IID_IUnknown))
		*ppvObject = static_cast<IWebResourceLoadDelegate*>(this);
	else if (IsEqualGUID(riid, IID_IWebResourceLoadDelegate))
		*ppvObject = static_cast<IWebResourceLoadDelegate*>(this);
	else
		return E_NOINTERFACE;

	AddRef();
	return S_OK;
}

HRESULT STDMETHODCALLTYPE Win32WebKitResourceLoadDelegate::identifierForInitialRequest(
	/* [in] */ IWebView *webView,
	/* [in] */ IWebURLRequest *request,
	/* [in] */ IWebDataSource *dataSource,
	/* [in] */ unsigned long identifier)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE Win32WebKitResourceLoadDelegate::willSendRequest(
	/* [in] */ IWebView *webView,
	/* [in] */ unsigned long identifier,
	/* [in] */ IWebURLRequest *request,
	/* [in] */ IWebURLResponse *redirectResponse,
	/* [in] */ IWebDataSource *dataSource,
	/* [retval][out] */ IWebURLRequest **newRequest)
{
	BSTR u;
	request->URL(&u);
	std::wstring u2(u);
	std::string url(::WideToUTF8(u2));
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE Win32WebKitResourceLoadDelegate::didReceiveAuthenticationChallenge(
	/* [in] */ IWebView *webView,
	/* [in] */ unsigned long identifier,
	/* [in] */ IWebURLAuthenticationChallenge *challenge,
	/* [in] */ IWebDataSource *dataSource)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE Win32WebKitResourceLoadDelegate::didCancelAuthenticationChallenge(
	/* [in] */ IWebView *webView,
	/* [in] */ unsigned long identifier,
	/* [in] */ IWebURLAuthenticationChallenge *challenge,
	/* [in] */ IWebDataSource *dataSource)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE Win32WebKitResourceLoadDelegate::didReceiveResponse(
	/* [in] */ IWebView *webView,
	/* [in] */ unsigned long identifier,
	/* [in] */ IWebURLResponse *response,
	/* [in] */ IWebDataSource *dataSource)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE Win32WebKitResourceLoadDelegate::didReceiveContentLength(
	/* [in] */ IWebView *webView,
	/* [in] */ unsigned long identifier,
	/* [in] */ UINT length,
	/* [in] */ IWebDataSource *dataSource)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE Win32WebKitResourceLoadDelegate::didFinishLoadingFromDataSource(
	/* [in] */ IWebView *webView,
	/* [in] */ unsigned long identifier,
	/* [in] */ IWebDataSource *dataSource)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE Win32WebKitResourceLoadDelegate::didFailLoadingWithError(
	/* [in] */ IWebView *webView,
	/* [in] */ unsigned long identifier,
	/* [in] */ IWebError *error,
	/* [in] */ IWebDataSource *dataSource)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE Win32WebKitResourceLoadDelegate::plugInFailedWithError(
	/* [in] */ IWebView *webView,
	/* [in] */ IWebError *error,
	/* [in] */ IWebDataSource *dataSource)
{
	return E_NOTIMPL;
}
}
