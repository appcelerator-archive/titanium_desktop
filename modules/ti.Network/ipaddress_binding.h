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
	class IPAddressBinding : public KAccessorObject
	{
	public:
		IPAddressBinding(std::string);
		IPAddressBinding(IPAddress);
		virtual ~IPAddressBinding();
		const bool IsInvalid() const { return invalid; }
		const IPAddress* GetAddress() const { return address; }


	private:
		bool invalid;
		SharedPtr<IPAddress> address;
		void Init();
		void ToString(const ValueList& args, KValueRef result);
		void IsInvalid(const ValueList& args, KValueRef result);
		void IsIPV4(const ValueList& args, KValueRef result);
		void IsIPV6(const ValueList& args, KValueRef result);
		void IsWildcard(const ValueList& args, KValueRef result);
		void IsBroadcast(const ValueList& args, KValueRef result);
		void IsLoopback(const ValueList& args, KValueRef result);
		void IsMulticast(const ValueList& args, KValueRef result);
		void IsUnicast(const ValueList& args, KValueRef result);
		void IsLinkLocal(const ValueList& args, KValueRef result);
		void IsSiteLocal(const ValueList& args, KValueRef result);
		void IsWellKnownMC(const ValueList& args, KValueRef result);
		void IsNodeLocalMC(const ValueList& args, KValueRef result);
		void IsLinkLocalMC(const ValueList& args, KValueRef result);
		void IsSiteLocalMC(const ValueList& args, KValueRef result);
		void IsOrgLocalMC(const ValueList& args, KValueRef result);
		void IsGlobalMC(const ValueList& args, KValueRef result);
	};
}

#endif
