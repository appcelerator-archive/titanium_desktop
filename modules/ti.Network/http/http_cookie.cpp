/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#include "http_cookie.h"

namespace ti
{
	HTTPCookie::HTTPCookie() : AccessorBoundObject("HTTPCookie")
	{
		this->InitializeBinding();
	}

	HTTPCookie::HTTPCookie(Poco::Net::HTTPCookie& cookie) : 
		AccessorBoundObject("HTTPCookie"),
		cookie(cookie)
	{
		this->InitializeBinding();
	}

	void HTTPCookie::InitializeBinding()
	{
		/**
		 * @tiapi(method=True,name=Network.HTTPCookie.getName,since=0.7)
		 * @tiapi Get the cookie name
		 * @tiresult[String] cookie name
		 */
		this->SetMethod("getName", &HTTPCookie::GetName);

		/**
		 * @tiapi(method=True,name=Network.HTTPCookie.setName,since=0.7)
		 * @tiapi Set the cookie name
		 * @tiarg[String,name] name of the cookie
		 */
		this->SetMethod("setName", &HTTPCookie::SetName);

		/**
		 * @tiapi(method=True,name=Network.HTTPCookie.getValue,since=0.7)
		 * @tiapi Get the cookie value
		 * @tiresult[String] cookie value
		 */
		this->SetMethod("getValue", &HTTPCookie::GetValue);

		/**
		 * @tiapi(method=True,name=Network.HTTPCookie.setValue,since=0.7)
		 * @tiapi Set the cookie value
		 * @tiarg[String,value] value to set cookie
		 */
		this->SetMethod("setValue", &HTTPCookie::SetValue);
	}

	void HTTPCookie::GetName(const ValueList& args, SharedValue result)
	{
		result->SetString(this->cookie.getName().c_str());
	}

	void HTTPCookie::SetName(const ValueList& args, SharedValue result)
	{
		args.VerifyException("setName", "s");
		this->cookie.setName(args.GetString(0));
	}

	void HTTPCookie::GetValue(const ValueList& args, SharedValue result)
	{
		result->SetString(this->cookie.getValue().c_str());
	}

	void HTTPCookie::SetValue(const ValueList& args, SharedValue result)
	{
		args.VerifyException("setValue", "s");
		this->cookie.setValue(args.GetString(0));
	}
}

