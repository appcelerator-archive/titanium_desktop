/*
 * Copyright (c) 2010 Appcelerator, Inc. All Rights Reserved.
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

#ifndef Interface_h
#define Interface_h

#include <kroll/kroll.h>
#include <Poco/Net/NetworkInterface.h>

#undef interface

namespace Titanium {

class Interface : public KAccessorObject {
public:
	Interface(const Poco::Net::NetworkInterface& interface);
	virtual ~Interface();

private:
	void _GetAddress(const ValueList& args, KValueRef result);
	void _GetIPAddress(const ValueList& args, KValueRef result);
	void _GetName(const ValueList& args, KValueRef result);
	void _GetDisplayName(const ValueList& args, KValueRef result);
	void _GetSubnetMask(const ValueList& args, KValueRef result);
	void _SupportsIPv4(const ValueList& args, KValueRef result);
	void _SupportsIPv6(const ValueList& args, KValueRef result);

	Poco::Net::NetworkInterface interface;
};

} // namespace Titanium

#endif
