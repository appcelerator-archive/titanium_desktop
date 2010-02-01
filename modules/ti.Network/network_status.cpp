/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */

#include <kroll/thread_manager.h>
#include "network_binding.h"
#include "network_status.h"

namespace ti
{
	NetworkStatus::NetworkStatus(NetworkBinding* binding) :
		StaticBoundObject("Network.NetworkStatus"),
		binding(binding),
		running(true)
	{
	}

	NetworkStatus::~NetworkStatus()
	{
		this->Shutdown();
	}

	void NetworkStatus::Start()
	{
		this->adapter = new Poco::RunnableAdapter<NetworkStatus>(
			*this, &NetworkStatus::StatusLoop);
		this->thread = new Poco::Thread();
		this->thread->start(*this->adapter);
	}

	void NetworkStatus::Shutdown(bool async)
	{
		if (!this->running)
			return;

		this->running = false;

		if (!async)
			this->thread->join();
	}

	void NetworkStatus::StatusLoop()
	{
		START_KROLL_THREAD;

		this->InitializeLoop();

		// We want to wake up and detect if we are running more
		// often than we want to test reachability, so we only
		// test reachability every 25 * .2s
		int count = 0;
		bool firedAtAll = false;
		bool previousStatus = false;
		while (this->running)
		{
			if (count == 0)
			{
				bool online = this->GetStatus();
				if (!firedAtAll || online != previousStatus)
				{
					firedAtAll = true;
					previousStatus = online;
					binding->NetworkStatusChange(online);
				}
			}

			if (count == 25)
				count = 0;
			else
				count++;

			Poco::Thread::sleep(200);
		}

		this->CleanupLoop();

		END_KROLL_THREAD;
	}
}
