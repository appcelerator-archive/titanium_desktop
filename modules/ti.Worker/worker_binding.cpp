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
	WorkerBinding::WorkerBinding(Host *host, KObjectRef global) :
		StaticBoundObject("Worker"),
		host(host),
		global(global)
	{
		/**
		 * @tiapi(method=True,name=Worker.createWorker,since=0.6) 
		 * @tiapi Create a worker thread instance
		 * @tiarg[String|Function, source] Either a Javascript function (does not 
		 * @tiarg support closures), the URL of a JavaScript file, or
		 * @tiarg a string containing JavaScript source.
		 * @tiresult[Worker.Worker] The newly-created worker instance
		 */
		this->SetMethod("createWorker",&WorkerBinding::CreateWorker);
	}
	WorkerBinding::~WorkerBinding()
	{
	}
	void WorkerBinding::CreateWorker(const ValueList& args, KValueRef result)
	{
		if (args.size()!=2)
		{
			throw ValueException::FromString("invalid argument specified");
		}
		
		bool is_function = args.at(1)->ToBool();
		
		std::string code;
		Logger *logger = Logger::Get("Worker");
		
		if (is_function)
		{
			// convert the function to a string block 
			code =  "(";
			code += args.at(0)->ToString();
			code += ")()";
		}
		else 
		{
			// this is a path -- probably should verify that this is relative and not an absolute URL to remote
			KMethodRef appURLToPath = global->GetNS("App.appURLToPath")->ToMethod();
			ValueList a;
			a.push_back(args.at(0));
			KValueRef result = appURLToPath->Call(a);
			const char *path = result->ToString();
			
			logger->Debug("worker file path = %s",path);
			
			std::ios::openmode flags = std::ios::in;
			Poco::FileInputStream *fis = new Poco::FileInputStream(path,flags);
			std::stringstream ostr;
			char buf[8096];
			int count = 0;
			while(!fis->eof())
			{
				fis->read((char*)&buf,8095);
				std::streamsize len = fis->gcount();
				if (len>0)
				{
					buf[len]='\0';
					count+=len;
					ostr << buf;
				}
				else break;
			}
			fis->close();
			code = std::string(ostr.str());
		}
		
		logger->Debug("Worker script code = %s", code.c_str());
		
		KObjectRef worker = new Worker(host,global,code);
		result->SetObject(worker);
	}
}
