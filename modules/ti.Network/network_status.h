/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _NETWORK_STATUS_H_
#define _NETWORK_STATUS_H_

#include <Poco/Thread.h>
#include <Poco/RunnableAdapter.h>
#include "network_binding.h"

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
		bool running;

		Poco::RunnableAdapter<NetworkStatus>* adapter;
		Poco::Thread* thread;

		void InitializeLoop();
		bool GetStatus();
		void CleanupLoop();
	};
}

#endif
