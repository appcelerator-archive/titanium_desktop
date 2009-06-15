/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#include "webkit_javascript_listener.h"

#include "win32_user_window.h"

namespace ti {

Win32WebKitJavascriptListener::Win32WebKitJavascriptListener(Win32UserWindow *window_) : window(window_), ref_count(1) {
	// TODO Auto-generated constructor stub

}

Win32WebKitJavascriptListener::~Win32WebKitJavascriptListener() {
	// TODO Auto-generated destructor stub
}

HRESULT STDMETHODCALLTYPE
Win32WebKitJavascriptListener::QueryInterface(REFIID riid, void **ppvObject)
{
	*ppvObject = 0;
	if (IsEqualGUID(riid, IID_IUnknown)) {
		*ppvObject = static_cast<IWebScriptDebugListener*>(this);
	}
	else if (IsEqualGUID(riid, IID_IWebScriptDebugListener)) {
		*ppvObject = static_cast<IWebScriptDebugListener*>(this);
	}
	else {
		return E_NOINTERFACE;
	}
	return S_OK;
}

ULONG STDMETHODCALLTYPE
Win32WebKitJavascriptListener::AddRef()
{
	return ++ref_count;
}

ULONG STDMETHODCALLTYPE
Win32WebKitJavascriptListener::Release()
{
	ULONG new_count = --ref_count;
	if (!new_count) delete(this);

	return new_count;
}

HRESULT STDMETHODCALLTYPE
Win32WebKitJavascriptListener::exceptionWasRaised(
    /* [in] */ IWebView *view,
    /* [in] */ IWebScriptCallFrame *frame,
    /* [in] */ int sourceID,
    /* [in] */ int lineNumber,
    /* [in] */ IWebFrame *forWebFrame)
{
	Logger::Get("UI.Win32WebKitJavascriptListener")->Debug("exceptionWasRaised()" );
	 return S_OK;
}

}
