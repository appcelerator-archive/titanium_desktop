/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2010 Appcelerator, Inc. All Rights Reserved.
 */

#include "network_module.h"
#include "common.h"

static Logger* GetLogger()
{
	return Logger::Get("Network");
}

void SetCurlProxySettings(CURL* curlHandle, SharedProxy proxy)
{
	if (proxy.isNull())
		return;

	if (proxy->type == SOCKS)
	{
		SET_CURL_OPTION(curlHandle, CURLOPT_PROXYTYPE, CURLPROXY_SOCKS5);
	}
	else
	{
		SET_CURL_OPTION(curlHandle, CURLOPT_PROXYTYPE, CURLPROXY_HTTP);
	}

	SET_CURL_OPTION(curlHandle, CURLOPT_PROXY, proxy->host.c_str());
	if (proxy->port != 0)
	{
		SET_CURL_OPTION(curlHandle, CURLOPT_PROXYPORT, proxy->port);
	}

	if (!proxy->username.empty() || !proxy->password.empty())
	{
		// We are allowing any sort of authentication. This may not be the fastest
		// method, but at least the request will succeed.
		std::string proxyAuthString(proxy->username);
		proxyAuthString.append(":");
		proxyAuthString.append(proxy->password.c_str());

		SET_CURL_OPTION(curlHandle, CURLOPT_PROXYUSERPWD, proxyAuthString.c_str());
		SET_CURL_OPTION(curlHandle, CURLOPT_PROXYAUTH, CURLAUTH_ANY);
	}
}

void SetStandardCurlHandleOptions(CURL* handle)
{
	SET_CURL_OPTION(handle, CURLOPT_SSL_VERIFYPEER, false);
	SET_CURL_OPTION(handle, CURLOPT_CAINFO,
		::UTF8ToSystem(ti::NetworkModule::GetRootCertPath()).c_str());

	// If a timeout happens, this normally causes cURL to fire a signal.
	// Since we're running multiple threads and possibily have multiple HTTP
	// requests going at once, we need to disable this behavior.
	SET_CURL_OPTION(handle, CURLOPT_NOSIGNAL, 1);

	// Enable all supported Accept-Encoding values.
	SET_CURL_OPTION(handle, CURLOPT_ENCODING, "");

	SET_CURL_OPTION(handle, CURLOPT_FOLLOWLOCATION, 1);
	SET_CURL_OPTION(handle, CURLOPT_AUTOREFERER, 1);

	static std::string cookieJarFilename;
	if (cookieJarFilename.empty())
	{
		cookieJarFilename = FileUtils::Join(
			Host::GetInstance()->GetApplication()->GetDataPath().c_str(),
			"cookies.dat", 0);
	}

	// cURL doesn't have built in thread support, so we must handle thread-safety
	// via the CURLSH callback API.
	SET_CURL_OPTION(handle, CURLOPT_SHARE, ti::NetworkModule::GetCurlShareHandle());
	SET_CURL_OPTION(handle, CURLOPT_COOKIEFILE, cookieJarFilename.c_str());
	SET_CURL_OPTION(handle, CURLOPT_COOKIEJAR, cookieJarFilename.c_str());
}

BytesRef ObjectToBytes(KObjectRef dataObject)
{
	// If this object is a Bytes object, just do the cast and return.
	BytesRef bytes(dataObject.cast<Bytes>());
	if (!bytes.isNull())
		return bytes;

	// Now try to treat this object like as a file-like object with
	// a .read() method which returns a Bytes. If this fails we'll
	// return NULL.
	KMethodRef nativeReadMethod(dataObject->GetMethod("read", 0));
	if (nativeReadMethod.isNull())
		return 0;

	KValueRef readValue(nativeReadMethod->Call());
	if (!readValue->IsObject())
		return 0;

	// If this cast fails, it will return NULL, as we expect.
	return readValue->ToObject().cast<Bytes>();
}
