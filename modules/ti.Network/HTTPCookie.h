/*
 * Copyright (c) 2009-2010 Appcelerator, Inc. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef HTTPCookie_h
#define HTTPCookie_h

#include <kroll/kroll.h>
#include <Poco/Net/HTTPCookie.h>

namespace Titanium {

class HTTPCookie : public KAccessorObject {
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

} // namespace Titanium

#endif
