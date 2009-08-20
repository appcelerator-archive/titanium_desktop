/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef SCRIPT_EVALUATOR_H_
#define SCRIPT_EVALUATOR_H_
namespace ti
{
	class ScriptEvaluator : public IWebScriptEvaluator {
	public:
		ScriptEvaluator() : ref_count(1) {}
		virtual ~ScriptEvaluator();

		// IUnknown
		virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject);
		virtual ULONG STDMETHODCALLTYPE AddRef(void);
		virtual ULONG STDMETHODCALLTYPE Release(void);

		// IWebScriptEvaluator
		virtual HRESULT STDMETHODCALLTYPE matchesMimeType(BSTR mimeType, BOOL *result);
		virtual HRESULT STDMETHODCALLTYPE evaluate(BSTR mimeType, BSTR sourceCode, int* context);

	protected:
		int ref_count;

		std::string GetModuleName(std::string mimeType);
		SharedValue FindScriptModule(std::string type);
		std::string BSTRToString(BSTR bstr);
	};
}
#endif /* SCRIPT_EVALUATOR_H_ */
