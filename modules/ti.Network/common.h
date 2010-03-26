/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2010 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _CURL_COMMON_Hj
#define _CURL_COMMON_Hj

#define SET_CURL_OPTION(handle, option, value) \
	{\
		CURLcode result = curl_easy_setopt(handle, option, value); \
		if (CURLE_OK != result) \
		{ \
			GetLogger()->Error("Failed to set cURL handle option ("#option"): %s", \
				curl_easy_strerror(result)); \
		} \
	}

void SetCurlProxySettings(CURL* curlHandle, SharedProxy proxy);
void SetStandardCurlHandleOptions(CURL* handle);
BytesRef ObjectToBytes(KObjectRef dataObject);

#endif
