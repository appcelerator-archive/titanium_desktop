/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _MONKEY_BINDING_H_
#define _MONKEY_BINDING_H_

#include <kroll/kroll.h>
#include <vector>
#include <Poco/RegularExpression.h>

namespace ti
{
	class PatternMatcher : public std::unary_function<char const *, bool> 
	{
	public:
		PatternMatcher(std::string& pattern) : pattern(pattern) {}

		bool operator()(std::string const &str) const 
		{
			return(match(pattern.c_str(), str.c_str()));
		}
	private:
		std::string pattern;
		bool match(char const *pat, char const *str) const 
		{
			// exact match returns immediately
			if (strcmp(pat,str)==0) return true;
			
			switch (*pat) 
			{
				case '\0': 
					return *str=='\0';
				case '*':
					return match(pat+1, str) || (*str && match(pat, str+1));
				default: 
					return *pat==*str && match(pat+1, str+1);
			}
		}
	};

	typedef std::vector<std::string> VectorOfStr;
	typedef std::vector<PatternMatcher> VectorOfPatterns;
	
	class MonkeyBinding : public kroll::StaticBoundObject
	{
	public:
		MonkeyBinding(Host*,SharedKObject);
	protected:
		virtual ~MonkeyBinding();
	private:
		SharedKObject global;
		Logger* logger;
		std::vector< std::pair< std::pair< VectorOfPatterns,VectorOfPatterns > ,std::string> > scripts;
		
		bool Matches(VectorOfPatterns, std::string &subject);
		void Callback(const ValueList &args, SharedValue result);
	};
}

#endif
