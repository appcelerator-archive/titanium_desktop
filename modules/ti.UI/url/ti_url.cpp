/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#include <kroll/base.h>
#include <Poco/StringTokenizer.h>
#include "ti_url.h"
#include <Poco/Environment.h>
#include <cstring>
#include <algorithm>
#include <cctype>
#include <kroll/kroll.h>

struct Curl_local_handler Titanium_ti_url_handler = {
	"ti",
	ti::TiURLGetAbsolutePath
};

namespace ti {

	std::string JoinTokenizer(Poco::StringTokenizer& tokenizer, const char *sep, int begin)
	{
		std::string joined = "";
		joined += sep;
		for (size_t i = begin; i < tokenizer.count(); i++) {
			joined += tokenizer[i];
			if (i < tokenizer.count() - 1) {
				joined += sep;
			}
		}
		return joined;
	}

	std::string TiURLGetRuntimePath(std::string path)
	{
		std::string runtime_path = Poco::Environment::get("KR_RUNTIME", "");
		return runtime_path + path;
	}
	
	std::string TiURLGetModulePath(std::string module, std::string path)
	{
		std::string module_path = kroll::Host::GetInstance()->GetModuleByName(module)->GetPath();
		return module_path + path;
	}

	/* TODO: Memory leak here */
	const char* TiURLGetAbsolutePath(const char *url)
	{
		std::string urlString = url;
		Poco::StringTokenizer tokenizer(urlString, "/", Poco::StringTokenizer::TOK_TRIM | Poco::StringTokenizer::TOK_IGNORE_EMPTY);
		std::string resource = tokenizer[0];

		if (resource == "runtime") {
			std::string absolute_path = TiURLGetRuntimePath(JoinTokenizer(tokenizer, "/", 1));
			std::cout << "  =>" << absolute_path << std::endl;
			return strdup(absolute_path.c_str());
		} else {
			// assume a module
			std::string absolute_path = TiURLGetModulePath(resource, JoinTokenizer(tokenizer, "/", 1));
			
			std::cout << "  =>" << absolute_path << std::endl;
			return strdup(absolute_path.c_str());
		}

		// TODO - add real logic to determine path for ti://appmodulename/path urls
		return "";
	}
}
