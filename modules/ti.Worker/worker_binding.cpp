/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license. 
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */	
#include <kroll/kroll.h>
#include "worker_binding.h"
#include "worker.h"
#include <Poco/FileStream.h>
#include <sstream>

namespace ti
{
	WorkerBinding::WorkerBinding() :
		StaticBoundObject("Worker")
	{
		/**
		 * @tiapi(method=True,name=Worker.createWorker,since=0.6) 
		 * @tiapi Create a worker thread instance
		 * @tiarg[String|Function, source] Either a JavaScript function (does not 
		 * @tiarg support closures), the URL of a JavaScript file, or
		 * @tiarg a string containing JavaScript source.
		 * @tiresult[Worker.Worker] The newly-created worker instance
		 */
		this->SetMethod("createWorker", &WorkerBinding::_CreateWorker);
	}

	WorkerBinding::~WorkerBinding()
	{
	}

	static std::string GetCodeFromMethod(KMethodRef method)
	{
		// Call the toString method on this JavaScript Function.
		if (!method->HasProperty("toString") || !method->Get("toString")->IsMethod())
			throw ValueException::FromString("Worker method must be a JavaScript method with a toString function.");

		KValueRef toStringResult(method->Get("toString")->ToMethod()->Call());
		if (!toStringResult->IsString())
			throw ValueException::FromString("Worker method toString did not return a string.");

		std::string result("(");
		result.append(toStringResult->ToString());
		result.append(")()");
		return result;
	}

	void WorkerBinding::_CreateWorker(const ValueList& args, KValueRef result)
	{
		static Logger* logger = Logger::Get("Worker");
		args.VerifyException("createWorker", "m|s");

		std::string code;
		if (args.at(0)->IsMethod())
		{
			code = GetCodeFromMethod(args.at(0)->ToMethod());
		}
		else 
		{
			// TODO: We assume this is a URL corresponding to a local path, we
			// should probably check that this isn't a remote URL.
			std::string path(URLUtils::URLToPath(args.GetString(0)));
			logger->Debug("Loading Worker from file at: '%s'", path.c_str());
			code = FileUtils::ReadFile(path);
		}

		printf("worker code: %s\n", code.c_str());
		result->SetObject(new Worker(code));
	}
}
