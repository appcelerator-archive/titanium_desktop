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

#include "IPAddress.h"

#include "Poco/Net/NetException.h"

using namespace Poco::Net;

namespace Titanium {

IPAddress::IPAddress(const std::string& ip)
	: KAccessorObject("Network.IPAddress")
	, invalid(false)
{
	this->Init();

	try
	{
		this->address = new Poco::Net::IPAddress(ip.c_str());
	}
	catch(InvalidAddressException &e)
	{
		this->invalid = true;
		this->address = new Poco::Net::IPAddress("0.0.0.0");
	}
}

IPAddress::IPAddress(const Poco::Net::IPAddress& ip)
	: invalid(false) 
{
	Poco::Net::IPAddress(ip.toString());
}

IPAddress::~IPAddress()
{
}

void IPAddress::Init()
{
	/**
	 * @tiapi(method=True,returns=String,name=Network.IPAddress.toString,since=0.2)
	 * @tiapi Return a string representation of an IPAddress object.
	 * @tiresult[String] A string representation of the IPAddress object.
	 */
	this->SetMethod("toString",&IPAddress::ToString);

	/**
	 * @tiapi(method=True,returns=Boolean,name=Network.IPAddress.isInvalid,since=0.2)
	 * @tiapi Check whether an IPAddress object is invalid.
	 * @tiresult[Boolean] True if the IPAddress object is invalid, false otherwise.
	 */
	this->SetMethod("isInvalid",&IPAddress::IsInvalid);

	/**
	 * @tiapi(method=True,returns=Boolean,name=Network.IPAddress.isIPV4,since=0.2)
	 * @tiapi Check whether an IPAddress object is an IPv4 address.
	 * @tiresult[Boolean] True if the IPAddress object is an IPv4 address , false otherwise.
	 */
	this->SetMethod("isIPV4",&IPAddress::IsIPV4);

	/**
	 * @tiapi(method=True,returns=Boolean,name=Network.IPAddress.isIPV6,since=0.2)
	 * @tiapi Check whether an IPAddress object is an IPv6 address.
	 * @tiresult[Boolean] True if the IPAddress object is an IPv6 address, false otherwise.
	 */
	this->SetMethod("isIPV6",&IPAddress::IsIPV6);

	/**
	 * @tiapi(method=True,returns=Boolean,name=Network.IPAddress.isWildcard,since=0.2)
	 * @tiapi Check whether an IPAddress object is a wildcard address.
	 * @tiresult[Boolean] True if the IPAddress object is a wildcard address, false otherwise.
	 */
	this->SetMethod("isWildcard",&IPAddress::IsWildcard);

	/**
	 * @tiapi(method=True,returns=Boolean,name=Network.IPAddress.isBroadcast,since=0.2)
	 * @tiapi Check whether an IPAddress object is a broadcast address.
	 * @tiresult[Boolean] Return true if the IPAddress object is a broadcast address, false otherwise.
	 */
	this->SetMethod("isBroadcast",&IPAddress::IsBroadcast);

	/**
	 * @tiapi(method=True,returns=Boolean,name=Network.IPAddress.isLoopback,since=0.2)
	 * @tiapi Check whether an IPAddress object is a loopback address.
	 * @tiresult[Boolean] True if the IPAddress object is a loopback address, false otherwise.
	 */
	this->SetMethod("isLoopback",&IPAddress::IsLoopback);

	/**
	 * @tiapi(method=True,returns=Boolean,name=Network.IPAddress.isMulticast,since=0.2)
	 * @tiapi Check whether an IPAddress object is a multicast address.
	 * @tiresult[Boolean] True if the IPAddress object is a multicast address, false otherwise.
	 */
	this->SetMethod("isMulticast",&IPAddress::IsMulticast);

	/**
	 * @tiapi(method=True,returns=Boolean,name=Network.IPAddress.isUnicast,since=0.2)
	 * @tiapi Check whether an IPAddress object is a unicast address.
	 * @tiresult[Boolean] True if the IPAddress object is a unicast address, false otherwise.
	 */
	this->SetMethod("isUnicast",&IPAddress::IsUnicast);

	/**
	 * @tiapi(method=True,returns=Boolean,name=Network.IPAddress.isLinkLocal,since=0.2)
	 * Check whether an IPAddress object is a link local address.
	 * @tiresult[Boolean] True if the IPAddress object is a link local address, false otherwise.
	 */
	this->SetMethod("isLinkLocal",&IPAddress::IsLinkLocal);

	/**
	 * @tiapi(method=True,returns=Boolean,name=Network.IPAddress.isSiteLocal,since=0.2)
	 * Check whether an IPAddress object is a site local address.
	 * @tiresult[Boolean] True if the IPAddress object is a site local address, false otherwise
	 */
	this->SetMethod("isSiteLocal",&IPAddress::IsSiteLocal);

	/**
	 * @tiapi(method=True,returns=Boolean,name=Network.IPAddress.isWellKnownMC,since=0.2)
	 * @tiapi Check whether an IPAddress object is a well-known multicast address.
	 * @tiresult[Boolean] True if the IPAddress object is a well-known multicast address, false otherwise.
	 */
	this->SetMethod("isWellKnownMC",&IPAddress::IsWellKnownMC);

	/**
	 * @tiapi(method=True,returns=Boolean,name=Network.IPAddress.isNodeLocalMC,since=0.2)
	 * @tiapi Check whether an IPAddress object is a node-local multicast address.
	 * @tiresult[Boolean] True if the IPAddress object is a node-local multicast address, false otherwise.
	 */
	this->SetMethod("isNodeLocalMC",&IPAddress::IsNodeLocalMC);

	/**
	 * @tiapi(method=True,returns=Boolean,name=Network.IPAddress.isLinkLocalMC,since=0.2)
	 * @tiapi Check whether an IPAddress object is a link-local multicast address.
	 * @tiresult[Boolean] True if the IPAddress object is a link-local multicast address, false otherwise.
	 */
	this->SetMethod("isLinkLocalMC",&IPAddress::IsLinkLocalMC);

	/**
	 * @tiapi(method=True,returns=Boolean,name=Network.IPAddress.isSiteLocalMC,since=0.2)
	 * @tiapi Check whether an IPAddress object is a site-local multicast address.
	 * @tiresult[Boolean] True if the IPAddress object is a site-local multicast address, false otherwise.
	 */
	this->SetMethod("isSiteLocalMC",&IPAddress::IsSiteLocalMC);

	/**
	 * @tiapi(method=True,returns=Boolean,name=Network.IPAddress.isOrgLocalMC,since=0.2)
	 * @tiapi Check whether an IPAddress object is an organization local multicast address.
	 * @tiresult[Boolean]True if the IPAddress object is an org local nulticast address, false otherwise.
	 */
	this->SetMethod("isOrgLocalMC",&IPAddress::IsOrgLocalMC);

	/**
	 * @tiapi(method=True,returns=Boolean,name=Network.IPAddress.isGlobalMC,since=0.2)
	 * @tiapi Check whether an IPAddress object is a global multicast address.
	 * @tiresult[Boolean] True if the IPAddress object is a global multicast address, false otherwise.
	 */
	this->SetMethod("isGlobalMC",&IPAddress::IsGlobalMC);
}

void IPAddress::IsInvalid(const ValueList& args, KValueRef result)
{
	result->SetBool(this->invalid);
}

void IPAddress::ToString(const ValueList& args, KValueRef result)
{
	result->SetString(this->address->toString().c_str());
}

void IPAddress::IsIPV4(const ValueList& args, KValueRef result)
{
	result->SetBool(!this->invalid && this->address->family() == Poco::Net::IPAddress::IPv4);
}

void IPAddress::IsIPV6(const ValueList& args, KValueRef result)
{
	result->SetBool(!this->invalid && this->address->family() == Poco::Net::IPAddress::IPv6);
}

void IPAddress::IsWildcard(const ValueList& args, KValueRef result)
{
	result->SetBool(!this->invalid && this->address->isWildcard());
}

void IPAddress::IsBroadcast(const ValueList& args, KValueRef result)
{
	result->SetBool(!this->invalid && this->address->isBroadcast());
}

void IPAddress::IsLoopback(const ValueList& args, KValueRef result)
{
	result->SetBool(!this->invalid && this->address->isLoopback());
}

void IPAddress::IsMulticast(const ValueList& args, KValueRef result)
{
	result->SetBool(!this->invalid && this->address->isMulticast());
}

void IPAddress::IsUnicast(const ValueList& args, KValueRef result)
{
	result->SetBool(!this->invalid && this->address->isUnicast());
}

void IPAddress::IsLinkLocal(const ValueList& args, KValueRef result)
{
	result->SetBool(!this->invalid && this->address->isLinkLocal());
}

void IPAddress::IsSiteLocal(const ValueList& args, KValueRef result)
{
	result->SetBool(!this->invalid && this->address->isSiteLocal());
}

void IPAddress::IsWellKnownMC(const ValueList& args, KValueRef result)
{
	result->SetBool(!this->invalid && this->address->isWellKnownMC());
}

void IPAddress::IsNodeLocalMC(const ValueList& args, KValueRef result)
{
	result->SetBool(!this->invalid && this->address->isNodeLocalMC());
}

void IPAddress::IsLinkLocalMC(const ValueList& args, KValueRef result)
{
	result->SetBool(!this->invalid && this->address->isLinkLocalMC());
}

void IPAddress::IsSiteLocalMC(const ValueList& args, KValueRef result)
{
	result->SetBool(!this->invalid && this->address->isSiteLocalMC());
}

void IPAddress::IsOrgLocalMC(const ValueList& args, KValueRef result)
{
	result->SetBool(!this->invalid && this->address->isOrgLocalMC());
}

void IPAddress::IsGlobalMC(const ValueList& args, KValueRef result)
{
	result->SetBool(!this->invalid && this->address->isGlobalMC());
}

} // namespace Titanium
