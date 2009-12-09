/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license. 
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#include <kroll/kroll.h>
#include "monkey_binding.h"
#include <sstream>
#include <functional>
#include <Poco/Path.h>
#include <Poco/FileStream.h>
using std::vector;
using std::string;

namespace ti
{
	MonkeyBinding::MonkeyBinding(Host *host, KObjectRef global) :
		StaticBoundObject("Monkey"),
		global(global),
		logger(Logger::Get("Monkey"))
	{
		this->callback = StaticBoundMethod::FromMethod(this, &MonkeyBinding::Callback);

		std::string resourcesPath = host->GetApplication()->GetResourcesPath();
		std::string userscriptsPath = FileUtils::Join(
			resourcesPath.c_str(), "userscripts", NULL);
		if (FileUtils::IsDirectory(userscriptsPath))
		{
			logger->Debug("Found userscripts directory at: %s\n", userscriptsPath.c_str());
			vector<string> files;
			FileUtils::ListDir(userscriptsPath, files);

			std::vector<std::string>::iterator iter = files.begin();
			while(iter != files.end())
			{
				std::string filename = (*iter++);
				Poco::Path filePath(FileUtils::Join(
					userscriptsPath.c_str(), filename.c_str(), NULL));
				if (filePath.getExtension() == "js")
				{
					this->ParseFile(filePath.toString());
				}
			}

			if (!scripts.empty())
			{
				GlobalObject::GetInstance()->AddEventListener(Event::PAGE_LOADED, callback);
			}
			
		}
	}

	MonkeyBinding::~MonkeyBinding()
	{
		GlobalObject::GetInstance()->RemoveEventListener(Event::PAGE_LOADED, this->callback);

		vector<Script*>::iterator i = scripts.begin();
		while (i != scripts.end())
		{
			Script* s = *i;
			i = scripts.erase(i);
			delete s;
		}
	}

	void MonkeyBinding::ParseFile(string filePath)
	{
		Poco::FileInputStream f(filePath);
		std::ostringstream scriptSource;
		bool inScript = false;
		Script* script = 0;

		std::string line;
		while (!f.eof())
		{
			std::getline(f, line);
			if (line.find("// ==UserScript==") == 0)
			{
				script = new Script;
			}
			else if (script && !inScript)
			{
				string::size_type i;
				if ((i = line.find("// @include")) == 0)
				{
					std::string pattern = FileUtils::Trim(line.substr(i+12).c_str());
					script->includes.push_back(pattern);
					continue;
				}
				else if ((i = line.find("// @exclude")) == 0)
				{
					std::string pattern = FileUtils::Trim(line.substr(i+12).c_str());
					script->excludes.push_back(pattern);
				}
				else if (line.find("// ==/UserScript==") == 0)
				{
					inScript = true;
					scriptSource << "(function(){\n";
				}
				//TODO: @require
			}
			else if (inScript)
			{
				scriptSource << line << "\n";
			}
		}

		if (script && inScript)
		{
			scriptSource << "\n})();";
			script->source = scriptSource.str();
			scripts.push_back(script);
		}
	}

	void MonkeyBinding::Callback(const ValueList &args, KValueRef result)
	{
		KObjectRef event = args.at(0)->ToObject();
		if (!event->Get("url")->IsString() ||
			!event->Get("scope")->IsObject() ||
			!event->GetObject("scope")->Get("window"))
		{
			throw ValueException::FromString(
				"ti.Monkey could not find required components of PAGE_LOADED events");
		}

		std::string url = event->GetString("url");
		KObjectRef windowObject = event->GetObject("scope")->GetObject("window");
		vector<Script*>::iterator iter = scripts.begin();
		while (iter != scripts.end())
		{
			Script* script = (*iter++);
			if (script->Matches(url))
			{
				EvaluateUserScript(event, url, windowObject, script->source);
			}
		}
	}

	void MonkeyBinding::EvaluateUserScript(
		KObjectRef event, std::string& url,
		KObjectRef windowObject, std::string& scriptSource)
	{
		static Logger *logger = Logger::Get("Monkey");
		// I got a castle in brooklyn, that's where i dwell
		// Word, brother.

		if (!windowObject->Get("eval")->IsMethod())
		{
			logger->Error("Found a window object without an "
				"eval function (%s) -- aborting", url.c_str());
			return;
		}

		KObjectRef target = event->GetObject("target");
		if (!windowObject->Get(GLOBAL_NS_VARNAME)->IsObject() &&
			!target.isNull() && target->Get("insertAPI")->IsMethod())
		{
			logger->Info("Forcing Titanium API into: %s\n", url.c_str());
			target->CallNS("insertAPI", Value::NewObject(windowObject));
		}

		KMethodRef evalFunction = windowObject->GetMethod("eval");
		logger->Info("Loading userscript for %s\n", url.c_str());
		try
		{
			evalFunction->Call(Value::NewString(scriptSource));
		}
		catch (ValueException &ex)
		{
			SharedString ss = ex.DisplayString();
			int line = -1;
			if (ex.GetValue()->IsObject() &&
				ex.GetValue()->ToObject()->Get("line")->IsNumber())
			{
				line = ex.GetValue()->ToObject()->Get("line")->ToInt();
			}
			logger->Error(
				"Exception generated evaluating user script for %s "
				"(line %i): %s", url.c_str(), line, ss->c_str());
		}
	}

	bool Script::Matches(const char* pattern, const char* target)
	{
		// exact match returns immediately
		if (!strcmp(pattern, target))
			return true;

		switch (*pattern)
		{
			case '\0':
				return *target == '\0';
			case '*':
				return Matches(pattern+1, target) 
					|| (*target && Matches(pattern, target+1));
			default:
				return *pattern == *target &&
					Matches(pattern+1, target+1);
		}
	}

	bool Script::Matches(vector<string>& patterns, string& url)
	{
		vector<string>::iterator iter = patterns.begin();
		while (iter != patterns.end())
		{
			string& pattern = (*iter++);
			if (Matches(pattern.c_str(), url.c_str()))
			{
				return true;
			}
		}
		return false;
	}

	bool Script::Matches(string& url)
	{
		return !Matches(this->excludes, url) && Matches(this->includes, url);
	}
}
