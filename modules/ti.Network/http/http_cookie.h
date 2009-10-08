/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _HTTP_COOKIE_H_
#define _HTTP_COOKIE_H_

#include <kroll/kroll.h>
#include <Poco/Net/HTTPCookie.h>

namespace ti
{
	class HTTPCookie : public AccessorBoundObject
	{
	public:
		HTTPCookie();
		HTTPCookie(Poco::Net::HTTPCookie& cookie);
		void InitializeBinding();

		void GetName(const ValueList& args, SharedValue result);
		void SetName(const ValueList& args, SharedValue result);
		void GetValue(const ValueList& args, SharedValue result);
		void SetValue(const ValueList& args, SharedValue result);
		void GetVersion(const ValueList& args, SharedValue result);
		void SetVersion(const ValueList& args, SharedValue result);
		void GetDomain(const ValueList& args, SharedValue result);
		void SetDomain(const ValueList& args, SharedValue result);
		void GetPath(const ValueList& args, SharedValue result);
		void SetPath(const ValueList& args, SharedValue result);
		void GetMaxAge(const ValueList& args, SharedValue result);
		void SetMaxAge(const ValueList& args, SharedValue result);
		void GetComment(const ValueList& args, SharedValue result);
		void SetComment(const ValueList& args, SharedValue result);
		void IsHTTPOnly(const ValueList& args, SharedValue result);
		void SetHTTPOnly(const ValueList& args, SharedValue result);
		void IsSecure(const ValueList& args, SharedValue result);
		void SetSecure(const ValueList& args, SharedValue result);
		void ToString(const ValueList& args, SharedValue result);

		SharedString DisplayString(int levels);

	private:
		Poco::Net::HTTPCookie cookie;
	};
}

#endif
