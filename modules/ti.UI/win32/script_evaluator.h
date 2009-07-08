/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef SCRIPT_EVALUATOR_H_
#define SCRIPT_EVALUATOR_H_
class ScriptEvaluator : public IWebScriptEvaluator {
public:
	ScriptEvaluator(kroll::Host *host) : host(host), ref_count(1) {}
	virtual ~ScriptEvaluator();

	// IUnknown
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject);
	virtual ULONG STDMETHODCALLTYPE AddRef(void);
	virtual ULONG STDMETHODCALLTYPE Release(void);

	// IWebScriptEvaluator
	virtual HRESULT STDMETHODCALLTYPE matchesMimeType(BSTR mimeType, BOOL *result);
	virtual HRESULT STDMETHODCALLTYPE evaluate(BSTR mimeType, BSTR sourceCode, int* context);

protected:
	kroll::Host *host;
	int ref_count;

	std::string GetModuleName(std::string mimeType);
	std::string BSTRToString(BSTR bstr);
};

#endif /* SCRIPT_EVALUATOR_H_ */
