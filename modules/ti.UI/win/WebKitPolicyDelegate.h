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

#ifndef WebKitPolicyDelegate_h
#define WebKitPolicyDelegate_h

#include "UserWindowWin.h"

namespace Titanium {

class UserWindowWin;

class WebKitPolicyDelegate : public IWebPolicyDelegate {
public:
	WebKitPolicyDelegate(UserWindowWin *window_);

	// These function declarations copied from WebKit source (PolicyDelegate.cpp)

	// IUnknown
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject);
	virtual ULONG STDMETHODCALLTYPE AddRef(void);
	virtual ULONG STDMETHODCALLTYPE Release(void);

	// IWebPolicyDelegate
	virtual HRESULT STDMETHODCALLTYPE decidePolicyForNavigationAction(
		/* [in] */ IWebView *webView,
		/* [in] */ IPropertyBag *actionInformation,
		/* [in] */ IWebURLRequest *request,
		/* [in] */ IWebFrame *frame,
		/* [in] */ IWebPolicyDecisionListener *listener);

	virtual HRESULT STDMETHODCALLTYPE decidePolicyForNewWindowAction(
		/* [in] */ IWebView *webView,
		/* [in] */ IPropertyBag *actionInformation,
		/* [in] */ IWebURLRequest *request,
		/* [in] */ BSTR frameName,
		/* [in] */ IWebPolicyDecisionListener *listener);

	virtual HRESULT STDMETHODCALLTYPE decidePolicyForMIMEType(
		/* [in] */ IWebView *webView,
		/* [in] */ BSTR type,
		/* [in] */ IWebURLRequest *request,
		/* [in] */ IWebFrame *frame,
		/* [in] */ IWebPolicyDecisionListener *listener);

	virtual HRESULT STDMETHODCALLTYPE unableToImplementPolicyWithError(
		/* [in] */ IWebView *webView,
		/* [in] */ IWebError *error,
		/* [in] */ IWebFrame *frame);

	// PolicyDelegate
	void setPermissive(bool permissive) { m_permissiveDelegate = permissive; }

private:
	ULONG m_refCount;
	UserWindowWin *window;
	bool m_permissiveDelegate;
};

} // namespace Titanium

#endif
