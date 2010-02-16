/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2010 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _ANALYTICS_BINDING_H_
#define _ANALYTICS_BINDING_H_

#include <kroll/kroll.h>
#include <Poco/Thread.h>
#include <Poco/Runnable.h>
#include <Poco/Mutex.h>
#include <Poco/Condition.h>
#include <curl/curl.h>
#include <queue>

namespace ti
{
	class AnalyticsBinding : public KEventObject, public Poco::Runnable
	{
	public:
		AnalyticsBinding();
		~AnalyticsBinding();
		void Shutdown();

	private:
		bool running;
		std::string url;
		std::string baseData;
		CURL* curlHandle;
		Poco::Thread thread;
		std::queue<std::string> events;
		Poco::Mutex eventsLock;
		KMethodRef startCallback;

		void run();
		void SendEventToAPIServer(std::string& eventData);
		void _SendEvent(const ValueList& args, KValueRef result);
		void _StartAnalyticsThread(const ValueList &args, KValueRef result);
	};
}

#endif
