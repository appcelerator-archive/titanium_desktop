/*
 * Copyright (c) 2008-2010 Appcelerator, Inc. All Rights Reserved.
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

#ifndef Host_h
#define Host_h

#include <kroll/kroll.h>
#include <Poco/Net/HostEntry.h>
#include <Poco/Net/IPAddress.h>

namespace Titanium {

class Host : public StaticBoundObject {
public:
	Host(const Poco::Net::IPAddress& address);
	Host(const std::string& name);
	virtual ~Host();

	bool IsInvalid() const { return invalid; }

protected:
	void Init();

private:
	Poco::Net::HostEntry host;
	std::string name;
	bool invalid;

private:
	void ToString(const ValueList& args, KValueRef result);
	void IsInvalid(const ValueList& args, KValueRef result);
	void GetName(const ValueList& args, KValueRef result);
	void GetAliases(const ValueList& args, KValueRef result);
	void GetAddresses(const ValueList& args, KValueRef result);
};

} // namespace Titanium

#endif
