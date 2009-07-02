/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */
#include "ipaddress_binding.h"

namespace ti
{
	IPAddressBinding::IPAddressBinding(std::string ip) : StaticBoundObject("IPAddress"), invalid(false)
	{
		this->Init();
		
		try
		{
			this->address = new IPAddress(ip.c_str());
		}
		catch(InvalidAddressException &e)
		{
			this->invalid = true;
			this->address = new IPAddress("0.0.0.0");
		}
	}
	IPAddressBinding::IPAddressBinding(IPAddress ip) : invalid(false) 
	{
		IPAddressBinding(ip.toString());
	}
	IPAddressBinding::~IPAddressBinding()
	{
		KR_DUMP_LOCATION
	}
	void IPAddressBinding::Init()
	{
		/**
		 * @tiapi(method=True,returns=string,name=Network.IPAddress.toString,since=0.2) Returns a string representation of an IPAddress object
		 * @tiresult(for=Network.IPAddress.toString,type=String) the string representation of the IPAddress object
		 */
		this->SetMethod("toString",&IPAddressBinding::ToString);
		/**
		 * @tiapi(method=True,returns=boolean,name=Network.IPAddress.isInvalid,since=0.2) Checks whether an IPAddress object is invalid
		 * @tiresult(for=Network.IPAddress.isInvalid,type=Boolean) true if the IPAddress object is invalid, false if otherwise
		 */
		this->SetMethod("isInvalid",&IPAddressBinding::IsInvalid);
		/**
		 * @tiapi(method=True,returns=boolean,name=Network.IPAddress.isIPV4,since=0.2) Checks whether an IPAddress object is IPv4
		 * @tiresult(for=Network.IPAddress.isIPV4,type=Boolean) returns true is the IPAddress object is IPv4, false if otherwise
		 */
		this->SetMethod("isIPV4",&IPAddressBinding::IsIPV4);
		/**
		 * @tiapi(method=True,returns=boolean,name=Network.IPAddress.isIPV6,since=0.2) Checks whether an IPAddress object is IPv6
		 * @tiresult(for=Network.IPAddress.isIPV6,type=Boolean) returns true is the IPAddress object is IPv6, false if otherwise
		 */
		this->SetMethod("isIPV6",&IPAddressBinding::IsIPV6);
		/**
		 * @tiapi(method=True,returns=boolean,name=Network.IPAddress.isWildcard,since=0.2) Checks whether an IPAddress object is a wildcard address
		 * @tiresult(for=Network.IPAddress.isWildcard,type=Boolean) returns true is the IPAddress object is a wildcard address, false if otherwise
		 */
		this->SetMethod("isWildcard",&IPAddressBinding::IsWildcard);
		/**
		 * @tiapi(method=True,returns=boolean,name=Network.IPAddress.isBroadcast,since=0.2) Checks whether an IPAddress object is a broadcast address
		 * @tiresult(for=Network.IPAddress.isBroadcast,type=Boolean) returns true if the IPAddress object is a broadcast address, false if otherwise
		 */
		this->SetMethod("isBroadcast",&IPAddressBinding::IsBroadcast);
		/**
		 * @tiapi(method=True,returns=boolean,name=Network.IPAddress.isLoopback,since=0.2) Checks whether an IPAddress object is a loopback address
		 * @tiresult(for=Network.IPAddress.isLoopback,type=Boolean) returns true if the IPAddress object is a loopback address, false if otherwise
		 */
		this->SetMethod("isLoopback",&IPAddressBinding::IsLoopback);
		/**
		 * @tiapi(method=True,returns=boolean,name=Network.IPAddress.isMulticast,since=0.2) Checks whether an IPAddress object is a multicast address
		 * @tiresult(for=Network.IPAddress.isMulticast,type=Boolean) returns true if the IPAddress object is a multicast address, false if otherwise
		 */
		this->SetMethod("isMulticast",&IPAddressBinding::IsMulticast);
		/**
		 * @tiapi(method=True,returns=boolean,name=Network.IPAddress.isUnicast,since=0.2) Checks whether an IPAddress object is a unicast address
		 * @tiresult(for=Network.IPAddress.isUnicast,type=Boolean) returns true if the IPAddress object is a unicast address, false if otherwise
		 */
		this->SetMethod("isUnicast",&IPAddressBinding::IsUnicast);
		/**
		 * @tiapi(method=True,returns=boolean,name=Network.IPAddress.isLinkLocal,since=0.2) Checks whether an IPAddress object is a link local address
		 * @tiresult(for=Network.IPAddress.isLinkLocal,type=Boolean) returns true if the IPAddress object is a link local address, false if otherwise
		 */
		this->SetMethod("isLinkLocal",&IPAddressBinding::IsLinkLocal);
		/**
		 * @tiapi(method=True,returns=boolean,name=Network.IPAddress.isSiteLocal,since=0.2) Checks whether an IPAddress object is a site local address
		 * @tiresult(for=Network.IPAddress.isSiteLocal,type=Boolean) returns true if the IPAddress object is a site local address, false if otherwise
		 */
		this->SetMethod("isSiteLocal",&IPAddressBinding::IsSiteLocal);
		/**
		 * @tiapi(method=True,returns=boolean,name=Network.IPAddress.isWellKnownMC,since=0.2) Checks whether an IPAddress object is a well-known multicast address
		 * @tiresult(for=Network.IPAddress.isWellKnownMC,type=Boolean) returns true if the IPAddress object is a well-known multicast address, false if otherwise
		 */
		this->SetMethod("isWellKnownMC",&IPAddressBinding::IsWellKnownMC);
		/**
		 * @tiapi(method=True,returns=boolean,name=Network.IPAddress.isNodeLocalMC,since=0.2) Checks whether an IPAddress object is a node-local multicast address
		 * @tiresult(for=Network.IPAddress.isNodeLocalMC,type=Boolean) returns true if the IPAddress object is a node-local multicast address, false if otherwise
		 */
		this->SetMethod("isNodeLocalMC",&IPAddressBinding::IsNodeLocalMC);
		/**
		 * @tiapi(method=True,returns=boolean,name=Network.IPAddress.isLinkLocalMC,since=0.2) Checks whether an IPAddress object is a link-local multicast address
		 * @tiresult(for=Network.IPAddress.isLinkLocalMC,type=Boolean) returns true if the IPAddress object is a link-local multicast address, false if otherwise
		 */
		this->SetMethod("isLinkLocalMC",&IPAddressBinding::IsLinkLocalMC);
		/**
		 * @tiapi(method=True,returns=boolean,name=Network.IPAddress.isSiteLocalMC,since=0.2) Checks whether an IPAddress object is a site-local multicast address
		 * @tiresult(for=Network.IPAddress.isSiteLocalMC,type=Boolean) returns true if the IPAddress object is a site-local multicast address, false if otherwise
		 */
		this->SetMethod("isSiteLocalMC",&IPAddressBinding::IsSiteLocalMC);
		/**
		 * @tiapi(method=True,returns=boolean,name=Network.IPAddress.isOrgLocalMC,since=0.2) Checks whether an IPAddress object is an organization local multicast address
		 * @tiresult(for=Network.IPAddress.isOrgLocalMC,type=Boolean) returns true if the IPAddress object is an org local multicast address, false if otherwise
		 */
		this->SetMethod("isOrgLocalMC",&IPAddressBinding::IsOrgLocalMC);
		/**
		 * @tiapi(method=True,returns=boolean,name=Network.IPAddress.isGlobalMC,since=0.2) Checks whether an IPAddress object is a global multicast address
		 * @tiresult(for=Network.IPAddress.isGlobalMC,type=Boolean) returns true if the IPAddress object is a global multicast address, false if otherwise
		 */
		this->SetMethod("isGlobalMC",&IPAddressBinding::IsGlobalMC);
	}
	void IPAddressBinding::IsInvalid(const ValueList& args, SharedValue result)
	{
		result->SetBool(this->invalid);
	}
	void IPAddressBinding::ToString(const ValueList& args, SharedValue result)
	{
		result->SetString(this->address->toString().c_str());
	}
	void IPAddressBinding::IsIPV4(const ValueList& args, SharedValue result)
	{
		result->SetBool(!this->invalid && this->address->family() == IPAddress::IPv4);
	}
	void IPAddressBinding::IsIPV6(const ValueList& args, SharedValue result)
	{
		result->SetBool(!this->invalid && this->address->family() == IPAddress::IPv6);
	}
	void IPAddressBinding::IsWildcard(const ValueList& args, SharedValue result)
	{
		result->SetBool(!this->invalid && this->address->isWildcard());
	}
	void IPAddressBinding::IsBroadcast(const ValueList& args, SharedValue result)
	{
		result->SetBool(!this->invalid && this->address->isBroadcast());
	}
	void IPAddressBinding::IsLoopback(const ValueList& args, SharedValue result)
	{
		result->SetBool(!this->invalid && this->address->isLoopback());
	}
	void IPAddressBinding::IsMulticast(const ValueList& args, SharedValue result)
	{
		result->SetBool(!this->invalid && this->address->isMulticast());
	}
	void IPAddressBinding::IsUnicast(const ValueList& args, SharedValue result)
	{
		result->SetBool(!this->invalid && this->address->isUnicast());
	}
	void IPAddressBinding::IsLinkLocal(const ValueList& args, SharedValue result)
	{
		result->SetBool(!this->invalid && this->address->isLinkLocal());
	}
	void IPAddressBinding::IsSiteLocal(const ValueList& args, SharedValue result)
	{
		result->SetBool(!this->invalid && this->address->isSiteLocal());
	}
	void IPAddressBinding::IsWellKnownMC(const ValueList& args, SharedValue result)
	{
		result->SetBool(!this->invalid && this->address->isWellKnownMC());
	}
	void IPAddressBinding::IsNodeLocalMC(const ValueList& args, SharedValue result)
	{
		result->SetBool(!this->invalid && this->address->isNodeLocalMC());
	}
	void IPAddressBinding::IsLinkLocalMC(const ValueList& args, SharedValue result)
	{
		result->SetBool(!this->invalid && this->address->isLinkLocalMC());
	}
	void IPAddressBinding::IsSiteLocalMC(const ValueList& args, SharedValue result)
	{
		result->SetBool(!this->invalid && this->address->isSiteLocalMC());
	}
	void IPAddressBinding::IsOrgLocalMC(const ValueList& args, SharedValue result)
	{
		result->SetBool(!this->invalid && this->address->isOrgLocalMC());
	}
	void IPAddressBinding::IsGlobalMC(const ValueList& args, SharedValue result)
	{
		result->SetBool(!this->invalid && this->address->isGlobalMC());
	}
}