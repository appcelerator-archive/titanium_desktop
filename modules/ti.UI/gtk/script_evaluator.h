/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef SCRIPT_EVALUATOR_H_
#define SCRIPT_EVALUATOR_H_

class ScriptEvaluator : public WebKitWebScriptEvaluator {
public:
	ScriptEvaluator() {}
	virtual ~ScriptEvaluator();

	// WebKitWebScriptEvaluator
	virtual bool matchesMimeType(const gchar *mime_type);
	virtual void evaluate(const gchar *mime_type, const gchar *source_code, void*);

protected:
	std::string GetModuleName(std::string mimeType);
};

extern void addScriptEvaluator(WebKitWebScriptEvaluator *evaluator);

#endif /* SCRIPT_EVALUATOR_H_ */
