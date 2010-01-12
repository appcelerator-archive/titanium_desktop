/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */
#include "../ui_module.h"
#include <comutil.h>

using namespace ti;
using namespace kroll;

Win32WebKitFrameLoadDelegate::Win32WebKitFrameLoadDelegate(Win32UserWindow *window) :
	window(window),
	ref_count(1)
{
}

HRESULT STDMETHODCALLTYPE Win32WebKitFrameLoadDelegate::didFinishLoadForFrame(
	IWebView *webView, IWebFrame *frame)
{
	JSGlobalContextRef context = frame->globalContext();
	JSObjectRef global_object = JSContextGetGlobalObject(context);
	KObjectRef frame_global = new KKJSObject(context, global_object);

	IWebDataSource *webDataSource;
	frame->dataSource(&webDataSource);
	IWebMutableURLRequest *urlRequest;
	webDataSource->request(&urlRequest);

	BSTR u;
	urlRequest->URL(&u);
	std::wstring wideURL(u);
	std::string url(::WideToUTF8(wideURL));

	window->FrameLoaded();
	window->PageLoaded(frame_global, url, context);
	
	return S_OK;
}

HRESULT STDMETHODCALLTYPE Win32WebKitFrameLoadDelegate::didClearWindowObject(
	IWebView *webView, JSContextRef context, JSObjectRef windowScriptObject,
	IWebFrame *frame)
{
	this->window->RegisterJSContext((JSGlobalContextRef) context);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE Win32WebKitFrameLoadDelegate::QueryInterface(
	REFIID riid, void **ppvObject)
{
	*ppvObject = 0;
	if (IsEqualGUID(riid, IID_IUnknown))
		*ppvObject = static_cast<IWebFrameLoadDelegate*>(this);
	else if (IsEqualGUID(riid, IID_IWebFrameLoadDelegate))
		*ppvObject = static_cast<IWebFrameLoadDelegate*>(this);
	else
		return E_NOINTERFACE;

	AddRef();
	return S_OK;
}

ULONG STDMETHODCALLTYPE Win32WebKitFrameLoadDelegate::AddRef()
{
	return ++ref_count;
}

ULONG STDMETHODCALLTYPE Win32WebKitFrameLoadDelegate::Release()
{
	ULONG new_count = --ref_count;
	if (!new_count) delete(this);

	return new_count;
}

HRESULT STDMETHODCALLTYPE Win32WebKitFrameLoadDelegate::didReceiveTitle(
	/* [in] */ IWebView *webView,
	/* [in] */ BSTR title,
	/* [in] */ IWebFrame *frame)
{
	Win32UserWindow* userWindow = this->window;

	if (title)
	{
		std::string newTitle;
		newTitle.append(bstr_t(title));
		userWindow->SetTitle(newTitle);
	}
	return S_OK;
}
