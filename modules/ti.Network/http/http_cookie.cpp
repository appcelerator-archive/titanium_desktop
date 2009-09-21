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

		/**
		 * @tiapi(method=True,name=Network.HTTPCookie.getVersion,since=0.7)
		 * @tiapi Identifies to which version of the state management specification the cookie conforms. 0 = netscape 1 = RFC2109
		 * @tiresult[Integer] cookie version number (0 or 1)
		 */
		this->SetMethod("getVersion", &HTTPCookie::GetVersion);

		/**
		 * @tiapi(method=True,name=Network.HTTPCookie.setVersion,since=0.7)
		 * @tiapi Set the state management specifiction version the cookie conforms. (Default: 0)
		 * @tiarg[Integer,version] cookie version (0 or 1)
		 */
		this->SetMethod("setVersion", &HTTPCookie::SetVersion);

		/**
		 * @tiapi(method=True,name=Network.HTTPCookie.getDomain,since=0.7)
		 * @tiapi Get the domain for which the cookie is valid
		 * @tiresult[String] the domain
		 */
		this->SetMethod("getDomain", &HTTPCookie::GetDomain);

		/**
		 * @tiapi(method=True,name=Network.HTTPCookie.setDomain,since=0.7)
		 * @tiapi Set the domain for which the cookie is valid
		 * @tiarg[String,domain] the domain to set
		 */
		this->SetMethod("setDomain", &HTTPCookie::SetDomain);

		/**
		 * @tiapi(method=True,name=Network.HTTPCookie.getPath,since=0.7)
		 * @tiapi Get the subset of URLs to which this cookie applies
		 * @tiresult[String] the path
		 */
		this->SetMethod("getPath", &HTTPCookie::GetPath);

		/**
		 * @tiapi(method=True,name=Network.HTTPCookie.setPath,since=0.7)
		 * @tiapi Set the subset of URLs to which this cookie applies
		 * @tiarg[String,path] the path to set
 		 */
		this->SetMethod("setPath", &HTTPCookie::SetPath);

		/**
		 * @tiapi(method=True,name=Network.HTTPCookie.getMaxAge,since=0.7)
		 * @tiapi Get the lifetime of the cookie, in seconds.
		 * @tiresult[Integer] lifetime in seconds. 0 = discard, -1 = never expire
		 */
		this->SetMethod("getMaxAge", &HTTPCookie::GetMaxAge);

		/**
		 * @tiapi(method=True,name=Network.HTTPCookie.setMaxAge,since=0.7)
		 * @tiapi Set the lifetime of the cookie, in seconds.
		 * @tiarg[Integer,lifetime] the lifetime in seconds. 0 = discard, -1 = never expire
		 */
		this->SetMethod("setMaxAge", &HTTPCookie::SetMaxAge);

		/**
		 * @tiapi(method=True,name=Network.HTTPCookie.getComment,since=0.7)
		 * @tiapi Get the cookie comment text
		 * @tiresult[String] comment text
		 */
		this->SetMethod("getComment", &HTTPCookie::GetComment);

		/**
		 * @tiapi(method=True,name=Network.HTTPCookie.setComment,since=0.7)
		 * @tiapi Set the cookie comment text
		 * @tiarg[String,comment] text to set as comment
 		 */
		this->SetMethod("setComment", &HTTPCookie::SetComment);

		/**
		 * @tiapi(method=True,name=Network.HTTPCookie.isHTTPOnly,since=0.7)
		 * @tiapi Check if the http only flag is set on the cookie
		 * @tiresult[Boolean] return True if http only flag is set
 		 */
		this->SetMethod("isHTTPOnly", &HTTPCookie::IsHTTPOnly);

		/**
		 * @tiapi(method=True,name=Network.HTTPCookie.setHTTPOnly,since=0.7)
		 * @tiapi Set the http only flag on the cookie
		 * @tiarg[Boolean,enableHTTPOnly] if True sets the http only flag
		 */
		this->SetMethod("setHTTPOnly", &HTTPCookie::SetHTTPOnly);

		/**
		 * @tiapi(method=True,name=Network.HTTPCookie.isSecure,since=0.7)
		 * @tiapi Check if the secure flag is set on the cookie
		 * @tiresult[Boolean] return True if cookie is secure
		 */
		this->SetMethod("isSecure", &HTTPCookie::IsSecure);

		/**
		 * @tiapi(method=True,name=Network.HTTPCookie.setSecure,since=0.7)
		 * @tiapi Set the secure flag on the cookie
		 * @tiarg[Boolean,enableSecure] if True makes the cookie secure
 		 */
		this->SetMethod("setSecure", &HTTPCookie::SetSecure);

		/**
		 * @tiapi(method=True,name=Network.HTTPCookie.toString,since=0.7)
		 * @tiapi return a string representation of the cookie
		 * @tiresult[String] cookie representation as a string (name=value;...)
 		 */
		this->SetMethod("toString", &HTTPCookie::ToString);
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

	void HTTPCookie::GetVersion(const ValueList& args, SharedValue result)
	{
		result->SetInt(this->cookie.getVersion());
	}

	void HTTPCookie::SetVersion(const ValueList& args, SharedValue result)
	{
		args.VerifyException("setVersion", "i");
		int version = args.GetInt(0);
		if (version > 1 || version < 0)
		{
			// Version is out of range, can only be 0 or 1	
			throw ValueException::FromString("HTTPCookie version invalid, must be 0 or 1");
		}
		this->cookie.setVersion(version);
	}

	void HTTPCookie::GetDomain(const ValueList& args, SharedValue result)
	{
		result->SetString(this->cookie.getDomain().c_str());
	}

	void HTTPCookie::SetDomain(const ValueList& args, SharedValue result)
	{
		args.VerifyException("setDomain", "s");
		this->cookie.setDomain(args.GetString(0));
	}

	void HTTPCookie::GetPath(const ValueList& args, SharedValue result)
	{
		result->SetString(this->cookie.getPath().c_str());
	}

	void HTTPCookie::SetPath(const ValueList& args, SharedValue result)
	{
		args.VerifyException("setPath", "s");
		this->cookie.setPath(args.GetString(0));
	}

	void HTTPCookie::GetMaxAge(const ValueList& args, SharedValue result)
	{
		result->SetInt(this->cookie.getMaxAge());
	}

	void HTTPCookie::SetMaxAge(const ValueList& args, SharedValue result)
	{
		args.VerifyException("setMaxAge", "i");
		this->cookie.setMaxAge(args.GetInt(0));
	}

	void HTTPCookie::GetComment(const ValueList& args, SharedValue result)
	{
		result->SetString(this->cookie.getComment().c_str());
	}

	void HTTPCookie::SetComment(const ValueList& args, SharedValue result)
	{
		args.VerifyException("setComment", "s");
		this->cookie.setComment(args.GetString(0));
	}

	void HTTPCookie::IsHTTPOnly(const ValueList& args, SharedValue result)
	{
		result->SetBool(this->cookie.getHttpOnly());
	}

	void HTTPCookie::SetHTTPOnly(const ValueList& args, SharedValue result)
	{
		args.VerifyException("setHTTPOnly", "b");
		this->cookie.setHttpOnly(args.GetBool(0));
	}

	void HTTPCookie::IsSecure(const ValueList& args, SharedValue result)
	{
		result->SetBool(this->cookie.getSecure());
	}

	void HTTPCookie::SetSecure(const ValueList& args, SharedValue result)
	{
		args.VerifyException("setSecure", "b");
		this->cookie.setSecure(args.GetBool(0));
	}

	void HTTPCookie::ToString(const ValueList& args, SharedValue result)
	{
		result->SetString(this->cookie.toString().c_str());
	}

	SharedString HTTPCookie::DisplayString(int levels)
	{
		SharedString cookieString = new std::string(this->cookie.toString());
		return cookieString;
	}
}

