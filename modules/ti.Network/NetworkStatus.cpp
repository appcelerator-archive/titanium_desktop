/*
 * Copyright (c) 2008-2010 Appcelerator, Inc. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "NetworkStatus.h"

#include <kroll/thread_manager.h>

#include "Network.h"

namespace Titanium {

NetworkStatus::NetworkStatus(Network* binding)
    : StaticBoundObject("Network.NetworkStatus")
    , binding(binding)
    , running(true)
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
			// this causes several problem, on windows there's a memory leak
			// when GetStatus() is called, and on osx when a proxy is present,
			// Titanium Developer crashes. This will need to be looked at later.

            // bool online = this->GetStatus();
            bool online = true;
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

} // namespace Titanium
