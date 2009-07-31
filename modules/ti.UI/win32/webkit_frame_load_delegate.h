/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef TI_WIN32_WEBKIT_FRAME_LOAD_DELEGATE_H_
#define TI_WIN32_WEBKIT_FRAME_LOAD_DELEGATE_H_
namespace ti {

class Win32UserWindow;

class Win32WebKitFrameLoadDelegate : public IWebFrameLoadDelegate {
protected:
	int ref_count;
	Win32UserWindow *window;

public:
	Win32WebKitFrameLoadDelegate(Win32UserWindow *window);

	// IUnknown
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject);
	virtual ULONG STDMETHODCALLTYPE AddRef(void);
	virtual ULONG STDMETHODCALLTYPE Release(void);

	// IWebFrameLoadDelegate
	virtual HRESULT STDMETHODCALLTYPE didStartProvisionalLoadForFrame(
		/* [in] */ IWebView* webView,
		/* [in] */ IWebFrame* /*frame*/) { return S_OK; }

	virtual HRESULT STDMETHODCALLTYPE didReceiveServerRedirectForProvisionalLoadForFrame(
		/* [in] */ IWebView *webView,
		/* [in] */ IWebFrame *frame) { return S_OK; }

	virtual HRESULT STDMETHODCALLTYPE didFailProvisionalLoadWithError(
		/* [in] */ IWebView *webView,
		/* [in] */ IWebError *error,
		/* [in] */ IWebFrame *frame) { return S_OK; }

	virtual HRESULT STDMETHODCALLTYPE didCommitLoadForFrame(
		/* [in] */ IWebView *webView,
		/* [in] */ IWebFrame *frame) { return S_OK; }

	virtual HRESULT STDMETHODCALLTYPE didReceiveTitle(
		/* [in] */ IWebView *webView,
		/* [in] */ BSTR title,
		/* [in] */ IWebFrame *frame);

	virtual HRESULT STDMETHODCALLTYPE didReceiveIcon(
		/* [in] */ IWebView *webView,
		/* [in] */ OLE_HANDLE hBitmap,
		/* [in] */ IWebFrame *frame) { return S_OK; }

	virtual HRESULT STDMETHODCALLTYPE didFinishLoadForFrame(
		/* [in] */ IWebView* webView,
		/* [in] */ IWebFrame* /*frame*/);

	virtual HRESULT STDMETHODCALLTYPE didFailLoadWithError(
		/* [in] */ IWebView *webView,
		/* [in] */ IWebError *error,
		/* [in] */ IWebFrame *forFrame) { return S_OK; }

	virtual HRESULT STDMETHODCALLTYPE didChangeLocationWithinPageForFrame(
		/* [in] */ IWebView *webView,
		/* [in] */ IWebFrame *frame) { return S_OK; }

	virtual HRESULT STDMETHODCALLTYPE willPerformClientRedirectToURL(
		/* [in] */ IWebView *webView,
		/* [in] */ BSTR url,
		/* [in] */ double delaySeconds,
		/* [in] */ DATE fireDate,
		/* [in] */ IWebFrame *frame) { return S_OK; }

	virtual HRESULT STDMETHODCALLTYPE didCancelClientRedirectForFrame(
		/* [in] */ IWebView *webView,
		/* [in] */ IWebFrame *frame) { return S_OK; }

	virtual HRESULT STDMETHODCALLTYPE willCloseFrame(
		/* [in] */ IWebView *webView,
		/* [in] */ IWebFrame *frame) { return S_OK; }

	virtual /* [local] */ HRESULT STDMETHODCALLTYPE windowScriptObjectAvailable(
		/* [in] */ IWebView *webView,
		/* [in] */ JSContextRef context,
		/* [in] */ JSObjectRef windowScriptObject) { return S_OK; }
		
	virtual HRESULT STDMETHODCALLTYPE didClearWindowObject(
		IWebView *webView,
		JSContextRef context,
		JSObjectRef windowScriptObject,
		IWebFrame *frame);
};

}

#endif /* TI_WIN32_WEBKIT_FRAME_LOAD_DELEGATE_H_ */
