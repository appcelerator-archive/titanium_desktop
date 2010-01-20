/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2010 Appcelerator, Inc. All Rights Reserved.
 */
#include <kroll/kroll.h>
#include "interface_binding.h"
#include "ipaddress_binding.h"
using Poco::Net::NetworkInterface;

namespace ti
{
	InterfaceBinding::InterfaceBinding(NetworkInterface& interface) :
		KAccessorObject("Network.Interface"),
		interface(interface)
	{
		this->SetMethod("getAddress", &InterfaceBinding::_GetAddress); // COMPATIBILITY

		/**
		 * @tiapi(method=True,name=Network.Interface.getIPAddress,since=0.9)
		 * Get the IP address of the interface.
		 * @tiresult[Network.IPAddress, address] The IP address of the interface.
		 */
		this->SetMethod("getIPAddress", &InterfaceBinding::_GetIPAddress);

		/**
		 * @tiapi(method=True,name=Network.Interface.getName,since=0.9)
		 * Get the name of the interface.
		 * @tiresult[String, name] The name of the interface.
		 */
		this->SetMethod("getName", &InterfaceBinding::_GetName);

		/**
		 * @tiapi(method=True,name=Network.Interface.getDisplayName,since=0.9)
		 * Get the display name of the interface.
		 * @tiresult[String, displayName] The display name of the interface.
		 */
		this->SetMethod("getDisplayName", &InterfaceBinding::_GetDisplayName);

		/**
		 * @tiapi(method=True,name=Network.Interface.getSubnetMask,since=0.9)
		 * Get the subnet mask of the interface.
		 * @tiresult[Network.IPAddress, mask] The subnet mask of the interface.
		 */
		this->SetMethod("getSubnetMask", &InterfaceBinding::_GetSubnetMask);

		/**
		 * @tiapi(method=True,name=Network.Interface.supportsIPv4,since=0.9)
		 * Whether or not this interface supports IPv4.
		 * @tiresult[Boolean, mask] Whether or not this interface supports IPv4.
		 */
		this->SetMethod("supportsIPv4", &InterfaceBinding::_SupportsIPv4);

		/**
		 * @tiapi(method=True,name=Network.Interface.supportsIPv6,since=0.9)
		 * Whether or not this interface supports IPv6.
		 * @tiresult[Boolean, mask] Whether or not this interface supports IPv6.
		 */
		this->SetMethod("supportsIPv6", &InterfaceBinding::_SupportsIPv6);
	}

	InterfaceBinding::~InterfaceBinding()
	{
	}

	void InterfaceBinding::_GetAddress(const ValueList& args, KValueRef result)
	{
		result->SetString(interface.address().toString().c_str());
	}

	void InterfaceBinding::_GetIPAddress(const ValueList& args, KValueRef result)
	{
		result->SetObject(new IPAddressBinding(interface.address()));
	}

	void InterfaceBinding::_GetName(const ValueList& args, KValueRef result)
	{
		result->SetString(interface.name().c_str());
	}

	void InterfaceBinding::_GetDisplayName(const ValueList& args, KValueRef result)
	{
		result->SetString(interface.displayName().c_str());
	}

	void InterfaceBinding::_GetSubnetMask(const ValueList& args, KValueRef result)
	{
		result->SetObject(new IPAddressBinding(interface.subnetMask()));
	}

	void InterfaceBinding::_SupportsIPv4(const ValueList& args, KValueRef result)
	{
		result->SetBool(interface.supportsIPv4());
	}

	void InterfaceBinding::_SupportsIPv6(const ValueList& args, KValueRef result)
	{
		result->SetBool(interface.supportsIPv6());
	}
}
