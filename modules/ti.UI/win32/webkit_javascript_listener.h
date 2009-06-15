/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _TI_WEBKIT_JAVASCRIPT_LISTENER_H_
#define _TI_WEBKIT_JAVASCRIPT_LISTENER_H_

#include <kroll/kroll.h>
#include <windows.h>

#include "WebKit.h"

namespace ti {

class Win32UserWindow;

class Win32WebKitJavascriptListener : public IWebScriptDebugListener {
protected:
	int ref_count;
	Win32UserWindow *window;
public:
	Win32WebKitJavascriptListener(Win32UserWindow *window);
	virtual ~Win32WebKitJavascriptListener();

    // IUnknown
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject);
    virtual ULONG STDMETHODCALLTYPE AddRef(void);
    virtual ULONG STDMETHODCALLTYPE Release(void);

    virtual HRESULT STDMETHODCALLTYPE didLoadMainResourceForDataSource(
        /* [in] */ IWebView *view,
        /* [in] */ IWebDataSource *dataSource) { return E_NOTIMPL; }

    virtual HRESULT STDMETHODCALLTYPE didParseSource(
        /* [in] */ IWebView *view,
        /* [in] */ BSTR sourceCode,
        /* [in] */ UINT baseLineNumber,
        /* [in] */ BSTR url,
        /* [in] */ int sourceID,
        /* [in] */ IWebFrame *forWebFrame) { return E_NOTIMPL; }

    virtual HRESULT STDMETHODCALLTYPE failedToParseSource(
        /* [in] */ IWebView *view,
        /* [in] */ BSTR sourceCode,
        /* [in] */ UINT baseLineNumber,
        /* [in] */ BSTR url,
        /* [in] */ BSTR error,
        /* [in] */ IWebFrame *forWebFrame) { return E_NOTIMPL; }

    virtual HRESULT STDMETHODCALLTYPE didEnterCallFrame(
        /* [in] */ IWebView *view,
        /* [in] */ IWebScriptCallFrame *frame,
        /* [in] */ int sourceID,
        /* [in] */ int lineNumber,
        /* [in] */ IWebFrame *forWebFrame) { return E_NOTIMPL; }

    virtual HRESULT STDMETHODCALLTYPE willExecuteStatement(
        /* [in] */ IWebView *view,
        /* [in] */ IWebScriptCallFrame *frame,
        /* [in] */ int sourceID,
        /* [in] */ int lineNumber,
        /* [in] */ IWebFrame *forWebFrame) { return E_NOTIMPL; }

    virtual HRESULT STDMETHODCALLTYPE willLeaveCallFrame(
        /* [in] */ IWebView *view,
        /* [in] */ IWebScriptCallFrame *frame,
        /* [in] */ int sourceID,
        /* [in] */ int lineNumber,
        /* [in] */ IWebFrame *forWebFrame) { return E_NOTIMPL; }

    virtual HRESULT STDMETHODCALLTYPE exceptionWasRaised(
        /* [in] */ IWebView *view,
        /* [in] */ IWebScriptCallFrame *frame,
        /* [in] */ int sourceID,
        /* [in] */ int lineNumber,
        /* [in] */ IWebFrame *forWebFrame);

    virtual HRESULT STDMETHODCALLTYPE serverDidDie( void) { return E_NOTIMPL; }
};

}

#endif /* _TI_WEBKIT_JAVASCRIPT_LISTENER_H_ */
