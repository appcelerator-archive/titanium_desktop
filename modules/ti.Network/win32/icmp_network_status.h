/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _ICMP_NETWORK_STATUS_H_
#define _ICMP_NETWORK_STATUS_H_

namespace ti
{
	class ICMPNetworkStatus : public NetworkStatus
	{
		public:
		ICMPNetworkStatus(NetworkBinding* binding);

		void Start();
		void Shutdown(bool async=false);
		void Job();

		private:
		virtual void InitializeLoop();
		virtual bool GetStatus();
		virtual void CleanupLoop();
		bool GetStatusFromDBus();
		std::vector<std::string> hosts;
	};
}

#endif
