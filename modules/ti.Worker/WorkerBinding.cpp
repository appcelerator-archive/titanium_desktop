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

#include "WorkerBinding.h"

#include <sstream>

#include <Poco/FileStream.h>

#include "Worker.h"

namespace Titanium {

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

} // namespace Titanium
