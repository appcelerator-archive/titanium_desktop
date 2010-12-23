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

#ifndef NetworkStatus_h
#define NetworkStatus_h

#include <kroll/kroll.h>
#include <Poco/RunnableAdapter.h>
#include <Poco/Thread.h>

namespace Titanium {

class Network;

class NetworkStatus : public StaticBoundObject {
public:
	NetworkStatus(Network* binding);
	virtual ~NetworkStatus();

	void Start();
	void Shutdown(bool async=false);
	void CheckStatus();
	void StatusLoop();

protected:
	void InitializeLoop();
	bool GetStatus();
	void CleanupLoop();

	Network* binding;
	bool running;

	Poco::RunnableAdapter<NetworkStatus>* adapter;
	Poco::Thread* thread;
};

} // namespace Titanium

#endif
