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

	private:
		Poco::Net::HTTPCookie cookie;
	};
}

#endif
