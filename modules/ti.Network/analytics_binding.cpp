/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2010 Appcelerator, Inc. All Rights Reserved.
 */

#include <kroll/thread_manager.h>
#include <Poco/Environment.h>
#include <Poco/Timezone.h>
#include <Poco/NumberFormatter.h>
#include "network_module.h"
#include "network_binding.h"
#include "analytics_binding.h"
#include "common.h"

#define SPEC_VERSION "2"
namespace ti
{

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

AnalyticsBinding::AnalyticsBinding() :
	KEventObject("Network.Analytics"),
	running(true),
	curlHandle(0),
	startCallback(0)
{
	SharedApplication app(Host::GetInstance()->GetApplication());
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
	AddQueryParameter(baseData, "ip", NetworkBinding::GetFirstIPAddress());
	AddQueryParameter(baseData, "ver", SPEC_VERSION);

	// The 'tz' property is the amount of minutes to add to local time to get
	// UTC time. According to: http://pocoproject.org/docs/Poco.Timezone.html
	// local time = UTC + utcOffset() + dst().
	AddQueryParameter(baseData, "tz", Poco::NumberFormatter::format(
		-(Poco::Timezone::utcOffset() + Poco::Timezone::dst()) / 60));

	this->SetMethod("_sendEvent", &AnalyticsBinding::_SendEvent);

	// When curl_easy_perform is called with an HTTPS address on Windows,
	// it seems to block the UI thread until the request initializes. This
	// causes a multi-second lag before the first page display. The most
	// important thing is to show the page as soon as possible, so we defer
	// starting the Analytics thread until the first page has loaded.
	// TODO: Figure out what is causing the blocking behavior in curl_easy_perform.
	this->startCallback = StaticBoundMethod::FromMethod(
		this, &AnalyticsBinding::_StartAnalyticsThread);
	GlobalObject::GetInstance()->AddEventListener(
		Event::PAGE_LOADED, this->startCallback);
}

void AnalyticsBinding::_StartAnalyticsThread(const ValueList &args, KValueRef result)
{
	// If we've already started the Analytics thread, bail out.
	if (this->startCallback.isNull())
		return;

	this->thread.start(*this);
	GlobalObject::GetInstance()->RemoveEventListener(
		Event::PAGE_LOADED, this->startCallback);
	this->startCallback = 0;
}

AnalyticsBinding::~AnalyticsBinding()
{
	this->Shutdown();
}

void AnalyticsBinding::Shutdown()
{
	if (!this->running)
		return;

	this->running = false;
	this->thread.join();
}

void AnalyticsBinding::_SendEvent(const ValueList &args, KValueRef result)
{
	std::string eventString(args.GetString(0));
	{
		Poco::Mutex::ScopedLock lock(eventsLock);
		events.push(eventString);
	}
}

void AnalyticsBinding::run()
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

void AnalyticsBinding::SendEventToAPIServer(std::string& eventData)
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
}
