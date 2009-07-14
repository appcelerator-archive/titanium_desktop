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


namespace ti
{
	MonkeyBinding::MonkeyBinding(Host *host, SharedKObject global) :
		global(global),
		logger(Logger::Get("Monkey"))
	{
		std::string resourcesPath = host->GetApplication()->GetResourcesPath();
		std::string userscriptsPath = FileUtils::Join(
			resourcesPath.c_str(), "userscripts", NULL);

		if (FileUtils::IsDirectory(userscriptsPath))
		{
			logger->Debug("Found userscripts directory at: %s\n", userscriptsPath.c_str());
			std::vector<std::string> files;
			FileUtils::ListDir(userscriptsPath,files);
			if (files.size()>0)
			{
				std::vector<std::string>::iterator iter = files.begin();
				while(iter!=files.end())
				{
					std::string file = (*iter++);
					std::string fn = FileUtils::Join(userscriptsPath.c_str(),file.c_str(),NULL);
					Poco::Path path(fn);
					if (path.getExtension() == "js")
					{
						Poco::FileInputStream f(fn);
						std::string line;
						std::ostringstream str;
						bool found = false, start = false;
						VectorOfPatterns includes;
						VectorOfPatterns excludes;
						while (!f.eof())
						{
							std::getline(f, line);
							if (line.find("// ==UserScript==") == 0)
							{
								found = true;
							}
							else if (found && !start)
							{
								std::string::size_type i = line.find("// @include");
								if (i == 0)
								{
									std::string url = FileUtils::Trim(line.substr(i+12).c_str());
									includes.push_back(PatternMatcher(url));
									continue;
								}
								i = line.find("// @exclude");
								if (i == 0)
								{
									std::string url = FileUtils::Trim(line.substr(i+12).c_str());
									excludes.push_back(PatternMatcher(url));
								}
								else if (line.find("// ==/UserScript==") == 0)
								{
									start = true;
									str << "(function(){\n";
								}
								//TODO: @require
							}
							else if (start)
							{
								str << line << "\n";
							}
						}
						if (found && start)
						{
							str << "\n})();";
							try
							{
								std::pair<VectorOfPatterns,VectorOfPatterns> r(includes,excludes);
								scripts.push_back(std::pair<std::pair< VectorOfPatterns,VectorOfPatterns >,std::string>(r,str.str()));
							}
							catch(Poco::RegularExpressionException &e)
							{
								std::cerr << "Exception loading user script: " << fn << " Exception: " << e.what() << std::endl;
							}
						}
					}
				}
				
				if (scripts.size()>0)
				{
					this->SetMethod("callback",&MonkeyBinding::Callback);
					KEventObject::root->AddEventListener(
						Event::PAGE_LOADED, this->GetMethod("callback"));
				}
			}
		}
	}
	MonkeyBinding::~MonkeyBinding()
	{
		KEventObject::root->RemoveEventListener(
			Event::PAGE_LOADED, this->GetMethod("callback"));
	}
	bool MonkeyBinding::Matches(VectorOfPatterns patterns, std::string &subject)
	{
		if (patterns.size()==0) return false;
		VectorOfPatterns::iterator iter = patterns.begin();
		while(iter!=patterns.end())
		{
			PatternMatcher m = (*iter++);
			if (m(subject.c_str()))
			{
				return true;
			}
		}
		return false;
	}
	void MonkeyBinding::Callback(const ValueList &args, SharedValue result)
	{
		SharedKObject event = args.at(0)->ToObject();
		std::string url_value = event->Get("url")->ToString();
		
		std::vector< std::pair< std::pair< VectorOfPatterns,VectorOfPatterns >,std::string> >::iterator iter = scripts.begin();
		while(iter!=scripts.end())
		{
			std::pair< std::pair< VectorOfPatterns,VectorOfPatterns >, std::string> e = (*iter++);
			VectorOfPatterns include = e.first.first;
			VectorOfPatterns exclude = e.first.second;

			if (Matches(exclude,url_value))
			{
				continue;
			}
			if (Matches(include,url_value))
			{
				// I got a castle in brooklyn, that's where i dwell 
				try
				{
					SharedKMethod eval = event->Get("scope")->ToObject()->Get("window")->ToObject()->Get("eval")->ToMethod();
#ifdef DEBUG
					std::cout << ">>> loading user script for " << url_value << std::endl;
#endif
					eval->Call(Value::NewString(e.second));
				}
				catch (ValueException &ex)
				{
					Logger* logger = Logger::Get("Monkey");
					SharedString ss = ex.DisplayString();
					int line = -1;

					if (ex.GetValue()->IsObject() &&
						ex.GetValue()->ToObject()->Get("line")->IsNumber())
					{
						line = ex.GetValue()->ToObject()->Get("line")->ToInt();
					}
					logger->Error(
						"Exception generated evaluating user script for %s "
						"(line %i): %s", url_value.c_str(), line, ss->c_str());
				}
				catch (std::exception &ex)
				{
					Logger* logger = Logger::Get("Monkey");
					logger->Error("Exception generated evaluating user script for %s, Exception: %s",url_value.c_str(),ex.what());
				}
			}
		}
	}
}
