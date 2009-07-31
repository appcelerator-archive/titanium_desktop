/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _MONKEY_BINDING_H_
#define _MONKEY_BINDING_H_
#include <kroll/kroll.h>
#include <vector>

namespace ti
{
	struct Script
	{
		public:
		std::vector<std::string> includes;
		std::vector<std::string> excludes;
		std::string source;
		bool Matches(std::string& url);
		static bool Matches(std::vector<std::string>&, std::string& url);
		static bool Matches(const char* pattern, const char* target);
	};

	class MonkeyBinding : public kroll::StaticBoundObject
	{
		public:
		MonkeyBinding(Host*, SharedKObject);

		protected:
		virtual ~MonkeyBinding();
		void ParseFile(string filePath);
		void Callback(const ValueList &args, SharedValue result);
		void EvaluateUserScript(
			SharedKObject, std::string&,SharedKObject, std::string&);

		SharedKObject global;
		Logger* logger;
		SharedKMethod callback;
		std::vector<Script*> scripts;
	};
}

#endif
