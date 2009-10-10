/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#include "../ui_module.h"
#include <shellapi.h>
#include <string>

namespace ti {
	Win32WebKitPolicyDelegate::Win32WebKitPolicyDelegate(Win32UserWindow *window_)
		: window(window_), m_refCount(1), m_permissiveDelegate(false)
	{
	}

	// IUnknown
	HRESULT STDMETHODCALLTYPE Win32WebKitPolicyDelegate::QueryInterface(REFIID riid, void** ppvObject)
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

	ULONG STDMETHODCALLTYPE Win32WebKitPolicyDelegate::AddRef(void)
	{
		return ++m_refCount;
	}

	ULONG STDMETHODCALLTYPE Win32WebKitPolicyDelegate::Release(void)
	{
		ULONG newRef = --m_refCount;
		if (!newRef)
			delete this;

		return newRef;
	}

	HRESULT STDMETHODCALLTYPE Win32WebKitPolicyDelegate::decidePolicyForNavigationAction(
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

	HRESULT STDMETHODCALLTYPE Win32WebKitPolicyDelegate::decidePolicyForNewWindowAction(
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

	HRESULT STDMETHODCALLTYPE Win32WebKitPolicyDelegate::decidePolicyForMIMEType(
		/* [in] */ IWebView *webView,
		/* [in] */ BSTR type,
		/* [in] */ IWebURLRequest *request,
		/* [in] */ IWebFrame *frame,
		/* [in] */ IWebPolicyDecisionListener *listener)
	{
		Logger::Get("UI.Win32WebKitPolicyDelegate")->Debug("decidePolicyForMIMEType() not implemented");
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE Win32WebKitPolicyDelegate::unableToImplementPolicyWithError(
		/* [in] */ IWebView *webView,
		/* [in] */ IWebError *error,
		/* [in] */ IWebFrame *frame)
	{
		Logger::Get("UI.Win32WebKitPolicyDelegate")->Debug("unableToImplementPolicyWithError() not implemented");
		return E_NOTIMPL;
	}

}
