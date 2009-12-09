/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */
#include "network_binding.h"

#ifndef OS_OSX

namespace ti
{
	NetworkStatus::NetworkStatus(NetworkBinding* binding) :
		StaticBoundObject("Network.NetworkStatus"),
		binding(binding),
		previous_status(false),
		running(true)
	{
	}

	NetworkStatus::~NetworkStatus()
	{
		this->Shutdown();
	}

	void NetworkStatus::Start()
	{
		this->adapter = new RunnableAdapter<NetworkStatus>(
			*this,
			&NetworkStatus::StatusLoop);
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
		this->InitializeLoop();

		// We want to wake up and detect if we are running more
		// often than we want to test reachability, so we only
		// test reachability every 25 * .2s
		int count = 0;
		while (this->running)
		{
			if (count == 0)
			{
				bool online = this->GetStatus();
				if (online != this->previous_status)
				{
					this->previous_status = online;
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
	}
}

#endif
