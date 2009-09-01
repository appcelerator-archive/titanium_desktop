/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _SCRIPT_EVALUATOR_H_
#define _SCRIPT_EVALUATOR_H_

#include <kroll/kroll.h>
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
#include <Poco/URI.h>

#define JSContextToKrollContext(context) \
	new KKJSObject(reinterpret_cast<JSContextRef>(context), JSContextGetGlobalObject(reinterpret_cast<JSContextRef>(context)))
/**
 * Script evaluators are responsible for matching and evaluating custom <script> types, and preprocessing URLs
 */
namespace ti
{
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
		static AutoPtr<ScriptEvaluator> instance;
		
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
}

#if defined(OS_OSX)
@interface OSXScriptEvaluator : NSObject
{
	ti::ScriptEvaluator* delegate;
}
-(OSXScriptEvaluator*) initWithEvaluator:(ti::ScriptEvaluator*)evaluator;
-(BOOL) matchesMimeType:(NSString*)mimeType;
-(void) evaluate:(NSString *)mimeType sourceCode:(NSString*)sourceCode context:(void *)context;
@end
#elif defined(OS_LINUX)
	extern void addScriptEvaluator(WebKitWebScriptEvaluator *evaluator);
#endif

#endif
