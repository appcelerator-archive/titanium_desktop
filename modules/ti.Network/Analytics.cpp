/*
 * Copyright (c) 2010 Appcelerator, Inc. All Rights Reserved.
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

#include "Analytics.h"

#include <kroll/thread_manager.h>
#include <Poco/Environment.h>
#include <Poco/Timezone.h>
#include <Poco/NumberFormatter.h>

#include "Network.h"
#include "NetworkUtils.h"

#define SPEC_VERSION "2"

namespace Titanium {

static Logger* GetLogger()
{
	return Logger::Get("Network.Analytics");
}

static void AddQueryParameter(std::string& url, const std::string& key,
	const std::string& value, bool first=false)
{
	if (!first)
		url.append("&");

	url.append(key);
	url.append("=");
	url.append(URLUtils::EncodeURIComponent(value));
}

Analytics::Analytics()
	: KEventObject("Network.Analytics")
	, running(true)
	, curlHandle(0)
	, startCallback(0)
{
	SharedApplication app(kroll::Host::GetInstance()->GetApplication());
	this->url = app->GetStreamURL("https") + "/app-track";

	AddQueryParameter(baseData, "mid", PlatformUtils::GetMachineId(), true);
	AddQueryParameter(baseData, "guid", app->guid);
	AddQueryParameter(baseData, "app_name", app->name);
	AddQueryParameter(baseData, "app_id", app->id);
	AddQueryParameter(baseData, "app_version", app->version);
	AddQueryParameter(baseData, "sid", DataUtils::GenerateUUID());
	AddQueryParameter(baseData, "mac_addr", PlatformUtils::GetFirstMACAddress());
	AddQueryParameter(baseData, "osver", Poco::Environment::osVersion().c_str());
	AddQueryParameter(baseData, "platform", OS_NAME);
	AddQueryParameter(baseData, "version", PRODUCT_VERSION);
	AddQueryParameter(baseData, "os", Poco::Environment::osName().c_str());
	AddQueryParameter(baseData, "ostype", OS_TYPE);
	AddQueryParameter(baseData, "osarch", Poco::Environment::osArchitecture().c_str());
	AddQueryParameter(baseData, "oscpu", 
		Poco::NumberFormatter::format(PlatformUtils::GetProcessorCount()));
	AddQueryParameter(baseData, "un", PlatformUtils::GetUsername());
	AddQueryParameter(baseData, "ip", Network::GetFirstIPAddress());
	AddQueryParameter(baseData, "ver", SPEC_VERSION);

	// The 'tz' property is the amount of minutes to add to local time to get
	// UTC time. According to: http://pocoproject.org/docs/Poco.Timezone.html
	// local time = UTC + utcOffset() + dst().
	AddQueryParameter(baseData, "tz", Poco::NumberFormatter::format(
		-(Poco::Timezone::utcOffset() + Poco::Timezone::dst()) / 60));

	this->SetMethod("_sendEvent", &Analytics::_SendEvent);

	// When curl_easy_perform is called with an HTTPS address on Windows,
	// it seems to block the UI thread until the request initializes. This
	// causes a multi-second lag before the first page display. The most
	// important thing is to show the page as soon as possible, so we defer
	// starting the Analytics thread until the first page has loaded.
	// TODO: Figure out what is causing the blocking behavior in curl_easy_perform.
	this->startCallback = StaticBoundMethod::FromMethod(
		this, &Analytics::_StartAnalyticsThread);
	GlobalObject::GetInstance()->AddEventListener(
		Event::PAGE_LOADED, this->startCallback);
}

void Analytics::_StartAnalyticsThread(const ValueList &args, KValueRef result)
{
	// If we've already started the Analytics thread, bail out.
	if (this->startCallback.isNull())
		return;

	this->thread.start(*this);
	GlobalObject::GetInstance()->RemoveEventListener(
		Event::PAGE_LOADED, this->startCallback);
	this->startCallback = 0;
}

Analytics::~Analytics()
{
	this->Shutdown();
}

void Analytics::Shutdown()
{
	if (!this->running)
		return;

	this->running = false;
	this->thread.join();
}

void Analytics::_SendEvent(const ValueList &args, KValueRef result)
{
	std::string eventString(args.GetString(0));
	{
		Poco::Mutex::ScopedLock lock(eventsLock);
		events.push(eventString);
	}
}

void Analytics::run()
{
	START_KROLL_THREAD;

	this->curlHandle = curl_easy_init();

	SET_CURL_OPTION(this->curlHandle, CURLOPT_URL, url.c_str());
	SET_CURL_OPTION(this->curlHandle, CURLOPT_POST, 1);

	SetCurlProxySettings(this->curlHandle, ProxyConfig::GetProxyForURL(url));
	SetStandardCurlHandleOptions(this->curlHandle);

	while (this->running || !events.empty())
	{

		while (!this->events.empty())
		{
			std::string nextEvent;
			{
				Poco::Mutex::ScopedLock lock(eventsLock);
				nextEvent = events.front();
				events.pop();
			}
			this->SendEventToAPIServer(nextEvent);
		}

		if (this->running && this->events.empty())
			Poco::Thread::sleep(100);
	}

	curl_easy_cleanup(this->curlHandle);
	this->curlHandle = 0;

	END_KROLL_THREAD;
}

void Analytics::SendEventToAPIServer(std::string& eventData)
{
	std::string postData(baseData + "&" + eventData);
	SET_CURL_OPTION(this->curlHandle, CURLOPT_POSTFIELDSIZE, postData.length());
	SET_CURL_OPTION(this->curlHandle, CURLOPT_POSTFIELDS, postData.c_str());

	CURLcode result = curl_easy_perform(this->curlHandle);
	if (result == CURLE_OPERATION_TIMEDOUT)
	{
		GetLogger()->Error("Failed for URL (%s): %s", this->url.c_str(),
			curl_easy_strerror(result));
	}
	else if (result == CURLE_ABORTED_BY_CALLBACK)
	{
		GetLogger()->Error("Failed for URL (%s): %s", this->url.c_str(),
			curl_easy_strerror(result));
	}
	else if (result != CURLE_OK)
	{
		GetLogger()->Error("Failed for URL (%s): %s", this->url.c_str(),
			curl_easy_strerror(result));
	}
}

} // namespace Titanium
