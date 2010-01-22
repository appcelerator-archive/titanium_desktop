/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2010 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _INTERFACE_BINDING_H_
#define _INTERFACE_BINDING_H_

#include <kroll/kroll.h>
#include <Poco/Net/NetworkInterface.h>
#undef interface

namespace ti
{
	class InterfaceBinding : public KAccessorObject
	{
	public:
		InterfaceBinding(Poco::Net::NetworkInterface&);
		virtual ~InterfaceBinding();

	private:
		Poco::Net::NetworkInterface interface;
		void _GetAddress(const ValueList& args, KValueRef result);
		void _GetIPAddress(const ValueList& args, KValueRef result);
		void _GetName(const ValueList& args, KValueRef result);
		void _GetDisplayName(const ValueList& args, KValueRef result);
		void _GetSubnetMask(const ValueList& args, KValueRef result);
		void _SupportsIPv4(const ValueList& args, KValueRef result);
		void _SupportsIPv6(const ValueList& args, KValueRef result);
	};
}

#endif
