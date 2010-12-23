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

#ifndef ScriptEvaluator_h
#define ScriptEvaluator_h

#include <kroll/kroll.h>
#include <Poco/URI.h>

#if defined(OS_OSX)
#	import <WebKit/WebKit.h>
#	import <WebKit/WebViewPrivate.h>
#	import <WebKit/WebInspector.h>
#	import <WebKit/WebScriptDebugDelegate.h>
#	import <WebKit/WebScriptObject.h>
#	import <WebKit/WebPreferencesPrivate.h>
#	import <WebKit/WebScriptElement.h>
#elif defined(OS_LINUX)
#	include <webkit/webkit.h>
#	include <webkit/webkittitanium.h>
#else
#	include <WebKit/WebKit.h>
#	include <WebKit/WebKitCOMAPI.h>
#	include <WebKit/WebKitTitanium.h>
#	include <comutil.h>
#endif

#include <kroll/javascript/javascript_module.h>

#define JSContextToKrollContext(context) \
	new KKJSObject(reinterpret_cast<JSContextRef>(context), JSContextGetGlobalObject(reinterpret_cast<JSContextRef>(context)))

namespace Titanium {

/*
 * Script evaluators are responsible for matching and
 * evaluating custom <script> types, and preprocessing URLs.
 */
class ScriptEvaluator
#if defined(OS_WIN32)
	: public IWebScriptEvaluator, public COMReferenceCounted
#elif defined(OS_LINUX)
	: public WebKitWebScriptEvaluator, public ReferenceCounted
#else
	: public ReferenceCounted
#endif
{
	public:
	static void Initialize();

	protected:
	ScriptEvaluator() {}

	public:
#if defined(OS_WIN32)
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject)
	{
		 return COMReferenceCounted::QueryInterface(riid,ppvObject);
	}
	virtual ULONG STDMETHODCALLTYPE AddRef(void)
	{
		return COMReferenceCounted::AddRef();
	}
	virtual ULONG STDMETHODCALLTYPE Release(void)
	{
		return COMReferenceCounted::Release();
	}
	virtual HRESULT STDMETHODCALLTYPE matchesMimeType(BSTR mimeType, BOOL *result)
	{
		*result = kroll::Script::GetInstance()->CanEvaluate((const char *)_bstr_t(mimeType));
		return S_OK;
	}
	virtual HRESULT STDMETHODCALLTYPE evaluate(BSTR mimeType, BSTR sourceCode, int* context)
	{
		try
		{
			kroll::Script::GetInstance()->Evaluate(
				(const char *)_bstr_t(mimeType), "<script>",
				(const char *)_bstr_t(sourceCode),
				JSContextToKrollContext(context));
		}
		catch (ValueException& exception)
		{
			Logger::Get("UI.ScriptEvaluator")->Error("Script evaluation failed: %s",
				exception.ToString().c_str());

		}
		return S_OK;
	}
#elif defined(OS_LINUX)
	virtual bool matchesMimeType(const gchar *mimeType)
	{
		return kroll::Script::GetInstance()->CanEvaluate(mimeType);
	}
	virtual void evaluate(const gchar *mimeType, const gchar *sourceCode, void* context)
	{
		try
		{
			kroll::Script::GetInstance()->Evaluate(mimeType, "<script>",
				sourceCode, JSContextToKrollContext(context));
		}
		catch (ValueException& exception)
		{
			Logger::Get("UI.ScriptEvaluator")->Error("Script evaluation failed: %s",
				exception.ToString().c_str());

		}
	}
#endif
};

} // namespace Titanium

#if defined(OS_OSX)
@interface OSXScriptEvaluator : NSObject
{
	Titanium::ScriptEvaluator* delegate;
}
-(OSXScriptEvaluator*) initWithEvaluator:(Titanium::ScriptEvaluator*)evaluator;
-(BOOL) matchesMimeType:(NSString*)mimeType;
-(void) evaluate:(NSString *)mimeType sourceCode:(NSString*)sourceCode context:(void *)context;
@end
#elif defined(OS_LINUX)
	extern void addScriptEvaluator(WebKitWebScriptEvaluator *evaluator);
#endif

#endif
