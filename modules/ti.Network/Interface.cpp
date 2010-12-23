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

#include "Interface.h"

#include "IPAddress.h"

using namespace Poco::Net;

namespace Titanium {

Interface::Interface(const NetworkInterface& interface)
	: KAccessorObject("Network.Interface")
	, interface(interface)
{
	this->SetMethod("getAddress", &Interface::_GetAddress); // COMPATIBILITY

	/**
	 * @tiapi(method=True,name=Network.Interface.getIPAddress,since=0.9)
	 * Get the IP address of the interface.
	 * @tiresult[Network.IPAddress, address] The IP address of the interface.
	 */
	this->SetMethod("getIPAddress", &Interface::_GetIPAddress);

	/**
	 * @tiapi(method=True,name=Network.Interface.getName,since=0.9)
	 * Get the name of the interface.
	 * @tiresult[String, name] The name of the interface.
	 */
	this->SetMethod("getName", &Interface::_GetName);

	/**
	 * @tiapi(method=True,name=Network.Interface.getDisplayName,since=0.9)
	 * Get the display name of the interface.
	 * @tiresult[String, displayName] The display name of the interface.
	 */
	this->SetMethod("getDisplayName", &Interface::_GetDisplayName);

	/**
	 * @tiapi(method=True,name=Network.Interface.getSubnetMask,since=0.9)
	 * Get the subnet mask of the interface.
	 * @tiresult[Network.IPAddress, mask] The subnet mask of the interface.
	 */
	this->SetMethod("getSubnetMask", &Interface::_GetSubnetMask);

	/**
	 * @tiapi(method=True,name=Network.Interface.supportsIPv4,since=0.9)
	 * Whether or not this interface supports IPv4.
	 * @tiresult[Boolean, mask] Whether or not this interface supports IPv4.
	 */
	this->SetMethod("supportsIPv4", &Interface::_SupportsIPv4);

	/**
	 * @tiapi(method=True,name=Network.Interface.supportsIPv6,since=0.9)
	 * Whether or not this interface supports IPv6.
	 * @tiresult[Boolean, mask] Whether or not this interface supports IPv6.
	 */
	this->SetMethod("supportsIPv6", &Interface::_SupportsIPv6);
}

Interface::~Interface()
{
}

void Interface::_GetAddress(const ValueList& args, KValueRef result)
{
	result->SetString(interface.address().toString().c_str());
}

void Interface::_GetIPAddress(const ValueList& args, KValueRef result)
{
	result->SetObject(new IPAddress(interface.address()));
}

void Interface::_GetName(const ValueList& args, KValueRef result)
{
	result->SetString(interface.name().c_str());
}

void Interface::_GetDisplayName(const ValueList& args, KValueRef result)
{
	result->SetString(interface.displayName().c_str());
}

void Interface::_GetSubnetMask(const ValueList& args, KValueRef result)
{
	result->SetObject(new IPAddress(interface.subnetMask()));
}

void Interface::_SupportsIPv4(const ValueList& args, KValueRef result)
{
	result->SetBool(interface.supportsIPv4());
}

void Interface::_SupportsIPv6(const ValueList& args, KValueRef result)
{
	result->SetBool(interface.supportsIPv6());
}

} // namespace Titanium
