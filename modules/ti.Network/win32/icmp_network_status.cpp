/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */
#include "../network_binding.h"
#include <Poco/Net/ICMPClient.h>
using Poco::Net::ICMPClient;

namespace ti
{
	ICMPNetworkStatus::ICMPNetworkStatus(NetworkBinding* binding) 
	 : NetworkStatus(binding)
	{
		this->hosts.push_back("appcelerator.com");
		this->hosts.push_back("appcelerator.org");
		this->hosts.push_back("google.com");
		this->hosts.push_back("yahoo.com");
	}

	void ICMPNetworkStatus::InitializeLoop()
	{
	}

	void ICMPNetworkStatus::CleanupLoop()
	{
	}

	bool ICMPNetworkStatus::GetStatus()
	{
		for (int i = 0; i < this->hosts.size(); i++)
		{
			if (ICMPClient::pingIPv4(this->hosts.at(i)) > 0)
			{
				std::cout << "Successfully pinged " << this->hosts.at(i) << std::endl;
				return true;
			}
		}

		return false;
	}
}
