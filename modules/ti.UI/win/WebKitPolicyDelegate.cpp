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

#include "WebKitPolicyDelegate.h"

#include <string>

#include <shellapi.h>

namespace Titanium {

WebKitPolicyDelegate::WebKitPolicyDelegate(UserWindowWin *window_)
	: window(window_)
	, m_refCount(1)
	, m_permissiveDelegate(false)
{
}

// IUnknown
HRESULT STDMETHODCALLTYPE WebKitPolicyDelegate::QueryInterface(REFIID riid, void** ppvObject)
{
	*ppvObject = 0;
	if (IsEqualGUID(riid, IID_IUnknown))
		*ppvObject = static_cast<IWebPolicyDelegate*>(this);
	else if (IsEqualGUID(riid, IID_IWebPolicyDelegate))
		*ppvObject = static_cast<IWebPolicyDelegate*>(this);
	else
		return E_NOINTERFACE;

	AddRef();
	return S_OK;
}

ULONG STDMETHODCALLTYPE WebKitPolicyDelegate::AddRef(void)
{
	return ++m_refCount;
}

ULONG STDMETHODCALLTYPE WebKitPolicyDelegate::Release(void)
{
	ULONG newRef = --m_refCount;
	if (!newRef)
		delete this;

	return newRef;
}

HRESULT STDMETHODCALLTYPE WebKitPolicyDelegate::decidePolicyForNavigationAction(
	/*[in]*/ IWebView* /*webView*/,
	/*[in]*/ IPropertyBag* actionInformation,
	/*[in]*/ IWebURLRequest* request,
	/*[in]*/ IWebFrame* frame,
	/*[in]*/ IWebPolicyDecisionListener* listener)
{
	BSTR u;
	request->URL(&u);
	std::wstring u2(u);
	std::string url;
	url.assign(u2.begin(), u2.end());

	// if url matches a window config, then modify window as needed
	this->window->UpdateWindowForURL(url);

	SysFreeString(u);

	listener->use();

	return S_OK;
}

HRESULT STDMETHODCALLTYPE WebKitPolicyDelegate::decidePolicyForNewWindowAction(
	/* [in] */ IWebView *webView,
	/* [in] */ IPropertyBag *actionInformation,
	/* [in] */ IWebURLRequest *request,
	/* [in] */ BSTR frameName,
	/* [in] */ IWebPolicyDecisionListener *listener)
{
	std::wstring frame(frameName);
	transform(frame.begin(), frame.end(), frame.begin(), tolower);

	if (frame == L"ti:systembrowser" || frame == L"_blank")
	{
		BSTR u;
		request->URL(&u);
		std::wstring url(u);

		ShellExecuteW(NULL, L"open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
		listener->ignore();

		SysFreeString(u);
	}
	else
	{
		listener->use();
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE WebKitPolicyDelegate::decidePolicyForMIMEType(
	/* [in] */ IWebView *webView,
	/* [in] */ BSTR type,
	/* [in] */ IWebURLRequest *request,
	/* [in] */ IWebFrame *frame,
	/* [in] */ IWebPolicyDecisionListener *listener)
{
	Logger::Get("UI.WebKitPolicyDelegate")->Debug("decidePolicyForMIMEType() not implemented");
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebKitPolicyDelegate::unableToImplementPolicyWithError(
	/* [in] */ IWebView *webView,
	/* [in] */ IWebError *error,
	/* [in] */ IWebFrame *frame)
{
	Logger::Get("UI.WebKitPolicyDelegate")->Debug("unableToImplementPolicyWithError() not implemented");
	return E_NOTIMPL;
}

} // namespace Titanium
