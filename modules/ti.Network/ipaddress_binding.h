/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _IPADDRESS_BINDING_H_
#define _IPADDRESS_BINDING_H_

#include <kroll/kroll.h>
#include <Poco/Net/IPAddress.h>
#include "Poco/Net/NetException.h"


using Poco::Net::IPAddress;
using Poco::Net::InvalidAddressException;

namespace ti
{
	class IPAddressBinding : public StaticBoundObject
	{
	public:
		IPAddressBinding(std::string);
		IPAddressBinding(IPAddress);
		virtual ~IPAddressBinding();
	protected:
		void Init();
	private:
		SharedPtr<IPAddress> address;
		bool invalid;
		
	public:
		const bool IsInvalid() const { return invalid; }
		const IPAddress* GetAddress() const { return address; }

	private:
		void ToString(const ValueList& args, SharedValue result);
		void IsInvalid(const ValueList& args, SharedValue result);
		void IsIPV4(const ValueList& args, SharedValue result);
		void IsIPV6(const ValueList& args, SharedValue result);
		void IsWildcard(const ValueList& args, SharedValue result);
		void IsBroadcast(const ValueList& args, SharedValue result);
		void IsLoopback(const ValueList& args, SharedValue result);
		void IsMulticast(const ValueList& args, SharedValue result);
		void IsUnicast(const ValueList& args, SharedValue result);
		void IsLinkLocal(const ValueList& args, SharedValue result);
		void IsSiteLocal(const ValueList& args, SharedValue result);
		void IsWellKnownMC(const ValueList& args, SharedValue result);
		void IsNodeLocalMC(const ValueList& args, SharedValue result);
		void IsLinkLocalMC(const ValueList& args, SharedValue result);
		void IsSiteLocalMC(const ValueList& args, SharedValue result);
		void IsOrgLocalMC(const ValueList& args, SharedValue result);
		void IsGlobalMC(const ValueList& args, SharedValue result);
	};
}

#endif
