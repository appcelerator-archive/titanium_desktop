/*
 * Copyright (c) 2008-2010 Appcelerator, Inc. All Rights Reserved.
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

#include <kroll/kroll.h>
#include <kroll/javascript/javascript_module.h>
#include <JavaScriptCore/JSContextRef.h>
#include "WebKitFrameLoadDelegate.h"

#include <comutil.h>

#include "UserWindowWin.h"

namespace Titanium {

WebKitFrameLoadDelegate::WebKitFrameLoadDelegate(UserWindowWin *window)
	: window(window)
	, ref_count(1)
{
}

HRESULT STDMETHODCALLTYPE WebKitFrameLoadDelegate::didFinishLoadForFrame(
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

HRESULT STDMETHODCALLTYPE WebKitFrameLoadDelegate::didClearWindowObject(
	IWebView *webView, JSContextRef context, JSObjectRef windowScriptObject,
	IWebFrame *frame)
{
	this->window->RegisterJSContext((JSGlobalContextRef) context);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE WebKitFrameLoadDelegate::QueryInterface(
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

ULONG STDMETHODCALLTYPE WebKitFrameLoadDelegate::AddRef()
{
	return ++ref_count;
}

ULONG STDMETHODCALLTYPE WebKitFrameLoadDelegate::Release()
{
	ULONG new_count = --ref_count;
	if (!new_count) delete(this);

	return new_count;
}

HRESULT STDMETHODCALLTYPE WebKitFrameLoadDelegate::didReceiveTitle(
	/* [in] */ IWebView* webView,
	/* [in] */ BSTR title,
	/* [in] */ IWebFrame* frame)
{
	// Only change the title if the new title was received for the main frame.
	IWebFrame* mainFrame;
	HRESULT hr = webView->mainFrame(&mainFrame);
	if (FAILED(hr))
	{
		Logger::Get("FrameLoadDelegate")->Error("Could not fetch main "
			"frame in didReceiveTitle delegate method");
		return S_OK;
	}
	if (frame != mainFrame)
		return S_OK;

	if (title)
	{
		std::string newTitle;
		newTitle.append(bstr_t(title));
		this->window->SetTitle(newTitle);
	}
	return S_OK;
}

} // namespace Titanium
