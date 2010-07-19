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
	class HTTPCookie : public KAccessorObject
	{
	public:
		HTTPCookie();
		HTTPCookie(Poco::Net::HTTPCookie& cookie);
		void InitializeBinding();

		void GetName(const ValueList& args, KValueRef result);
		void SetName(const ValueList& args, KValueRef result);
		void GetValue(const ValueList& args, KValueRef result);
		void SetValue(const ValueList& args, KValueRef result);
		void GetVersion(const ValueList& args, KValueRef result);
		void SetVersion(const ValueList& args, KValueRef result);
		void GetDomain(const ValueList& args, KValueRef result);
		void SetDomain(const ValueList& args, KValueRef result);
		void GetPath(const ValueList& args, KValueRef result);
		void SetPath(const ValueList& args, KValueRef result);
		void GetMaxAge(const ValueList& args, KValueRef result);
		void SetMaxAge(const ValueList& args, KValueRef result);
		void GetComment(const ValueList& args, KValueRef result);
		void SetComment(const ValueList& args, KValueRef result);
		void IsHTTPOnly(const ValueList& args, KValueRef result);
		void SetHTTPOnly(const ValueList& args, KValueRef result);
		void IsSecure(const ValueList& args, KValueRef result);
		void SetSecure(const ValueList& args, KValueRef result);
		void ToString(const ValueList& args, KValueRef result);

		SharedString DisplayString(int levels);

	private:
		Poco::Net::HTTPCookie cookie;
	};
}

#endif
