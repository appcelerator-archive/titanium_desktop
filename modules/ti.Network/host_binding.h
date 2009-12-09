/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _HOST_BINDING_H_
#define _HOST_BINDING_H_

#include <kroll/kroll.h>
#include "Poco/Net/DNS.h"
#include "Poco/Net/HostEntry.h"
#include "Poco/Net/NetException.h"
#include "ipaddress_binding.h"

using Poco::Net::DNS;
using Poco::Net::IPAddress;
using Poco::Net::HostEntry;
using Poco::Net::InvalidAddressException;
using Poco::Net::HostNotFoundException;
using Poco::Net::ServiceNotFoundException;
using Poco::Net::NoAddressFoundException;

namespace ti
{
	class HostBinding : public StaticBoundObject
	{
	public:
		HostBinding(IPAddress);
		HostBinding(std::string);
		virtual ~HostBinding();
	protected:
		void Init();
	private:
		HostEntry host;
		std::string name;
		bool invalid;
		
	public:
		const bool IsInvalid() const { return invalid; }

	private:
		void ToString(const ValueList& args, KValueRef result);
		void IsInvalid(const ValueList& args, KValueRef result);
		void GetName(const ValueList& args, KValueRef result);
		void GetAliases(const ValueList& args, KValueRef result);
		void GetAddresses(const ValueList& args, KValueRef result);
	};
}

#endif
