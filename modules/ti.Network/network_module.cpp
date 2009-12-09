/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */
#include "network_module.h"
#include "network_binding.h"

#include <Poco/Mutex.h>

using namespace kroll;
using namespace ti;
namespace ti
{
	KROLL_MODULE(NetworkModule, STRING(MODULE_NAME), STRING(MODULE_VERSION));
	static std::string modulePath;
	static CURLSH* curlShareHandle = 0;

	static Poco::Mutex* SharedResourceMutex(curl_lock_data data)
	{
		static Poco::Mutex cookieMutex;
		static Poco::Mutex dnsMutex;
		static Poco::Mutex shareMutex;

		switch (data) {
			case CURL_LOCK_DATA_COOKIE:
				return &cookieMutex;
			case CURL_LOCK_DATA_DNS:
				return &dnsMutex;
			case CURL_LOCK_DATA_SHARE:
				return &shareMutex;
			default:
				return NULL;
		}
	}

	static void CurlLockCallback(CURL* handle, curl_lock_data data, curl_lock_access, void*)
	{
		if (Poco::Mutex* mutex = SharedResourceMutex(data))
			mutex->lock();
	}

	static void CurlUnlockCallback(CURL* handle, curl_lock_data data, void* userPtr)
	{
		if (Poco::Mutex* mutex = SharedResourceMutex(data))
			mutex->unlock();
	}

	void NetworkModule::Initialize()
	{
		curl_global_init(CURL_GLOBAL_ALL);

		modulePath = GetPath();

		// load our variables
		this->variables = new NetworkBinding(host);

		// set our ti.Network
		KValueRef value = Value::NewObject(this->variables);
		host->GetGlobalObject()->Set("Network", value);

		curlShareHandle = curl_share_init();
		curl_share_setopt(curlShareHandle, CURLSHOPT_SHARE, CURL_LOCK_DATA_COOKIE);
		curl_share_setopt(curlShareHandle, CURLSHOPT_SHARE, CURL_LOCK_DATA_DNS);
		curl_share_setopt(curlShareHandle, CURLSHOPT_LOCKFUNC, CurlLockCallback);
		curl_share_setopt(curlShareHandle, CURLSHOPT_UNLOCKFUNC, CurlUnlockCallback);
	}

	void NetworkModule::Stop()
	{
		AutoPtr<NetworkBinding> b = this->variables.cast<NetworkBinding>();
		b->Shutdown();
	}

	/*static*/
	std::string& NetworkModule::GetRootCertPath()
	{
		static std::string path;
		if (path.empty())
			path = FileUtils::Join(modulePath.c_str(), "rootcert.pem", 0);
		return path;
	}

	/*static*/
	CURLSH* NetworkModule::GetCurlShareHandle()
	{
		return curlShareHandle;
	}
}
