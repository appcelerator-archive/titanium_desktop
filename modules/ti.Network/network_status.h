/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef OS_OSX

#ifndef _NETWORK_STATUS_H_
#define _NETWORK_STATUS_H_

#include <Poco/Thread.h>
#include <Poco/RunnableAdapter.h>
#include "network_binding.h"
using Poco::RunnableAdapter;

namespace ti
{
	class NetworkStatus : public StaticBoundObject
	{
		public:
		NetworkStatus(NetworkBinding* binding);
		virtual ~NetworkStatus();

		void Start();
		void Shutdown(bool async=false);
		void CheckStatus();
		void StatusLoop();

		protected:
		NetworkBinding* binding;
		bool previous_status;
		bool running;

		RunnableAdapter<NetworkStatus>* adapter;
		Poco::Thread *thread;

		virtual void InitializeLoop() {};
		virtual bool GetStatus() = 0;
		virtual void CleanupLoop() {};
	};
}

#endif

#endif
