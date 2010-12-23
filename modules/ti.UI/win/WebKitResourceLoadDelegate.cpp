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

#include "WebKitResourceLoadDelegate.h"

namespace Titanium {

WebKitResourceLoadDelegate::WebKitResourceLoadDelegate(UserWindowWin* userWindow)
	: userWindow(userWindow)
	, refCount(1)
{

}

ULONG STDMETHODCALLTYPE WebKitResourceLoadDelegate::AddRef()
{
	return ++refCount;
}

ULONG STDMETHODCALLTYPE WebKitResourceLoadDelegate::Release()
{
	ULONG newCount = --refCount;
	if (!newCount)
		delete(this);
	return newCount;
}

HRESULT STDMETHODCALLTYPE WebKitResourceLoadDelegate::QueryInterface(
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

HRESULT STDMETHODCALLTYPE WebKitResourceLoadDelegate::identifierForInitialRequest(
	/* [in] */ IWebView *webView,
	/* [in] */ IWebURLRequest *request,
	/* [in] */ IWebDataSource *dataSource,
	/* [in] */ unsigned long identifier)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebKitResourceLoadDelegate::willSendRequest(
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

HRESULT STDMETHODCALLTYPE WebKitResourceLoadDelegate::didReceiveAuthenticationChallenge(
	/* [in] */ IWebView *webView,
	/* [in] */ unsigned long identifier,
	/* [in] */ IWebURLAuthenticationChallenge *challenge,
	/* [in] */ IWebDataSource *dataSource)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebKitResourceLoadDelegate::didCancelAuthenticationChallenge(
	/* [in] */ IWebView *webView,
	/* [in] */ unsigned long identifier,
	/* [in] */ IWebURLAuthenticationChallenge *challenge,
	/* [in] */ IWebDataSource *dataSource)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebKitResourceLoadDelegate::didReceiveResponse(
	/* [in] */ IWebView *webView,
	/* [in] */ unsigned long identifier,
	/* [in] */ IWebURLResponse *response,
	/* [in] */ IWebDataSource *dataSource)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebKitResourceLoadDelegate::didReceiveContentLength(
	/* [in] */ IWebView *webView,
	/* [in] */ unsigned long identifier,
	/* [in] */ UINT length,
	/* [in] */ IWebDataSource *dataSource)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebKitResourceLoadDelegate::didFinishLoadingFromDataSource(
	/* [in] */ IWebView *webView,
	/* [in] */ unsigned long identifier,
	/* [in] */ IWebDataSource *dataSource)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebKitResourceLoadDelegate::didFailLoadingWithError(
	/* [in] */ IWebView *webView,
	/* [in] */ unsigned long identifier,
	/* [in] */ IWebError *error,
	/* [in] */ IWebDataSource *dataSource)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebKitResourceLoadDelegate::plugInFailedWithError(
	/* [in] */ IWebView *webView,
	/* [in] */ IWebError *error,
	/* [in] */ IWebDataSource *dataSource)
{
	return E_NOTIMPL;
}

} // namespace Titanium
