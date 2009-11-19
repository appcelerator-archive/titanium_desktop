/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#include "../network_module.h"
#include "http_client_binding.h"
#include <kroll/thread_manager.h>
#include <sstream>

#define SET_CURL_OPTION(handle, option, value) \
	{\
		CURLcode result = curl_easy_setopt(handle, option, value); \
		if (CURLE_OK != result) \
		{ \
			GetLogger()->Error("Failed to set cURL handle option ("#option"): %s", \
				curl_easy_strerror(result)); \
		} \
	}

using Poco::Net::NameValueCollection;

namespace ti
{
	static Logger* GetLogger()
	{
		static Logger* logger = Logger::Get("Network.HTTPClient");
		return logger;
	}

	HTTPClientBinding::HTTPClientBinding(Host* host) :
		KEventObject("Network.HTTPClient"),
		host(host),
		async(true),
		timeout(5 * 60 * 1000),
		maxRedirects(-1),
		curlHandle(0),
		thread(0),
		requestStream(0),
		requestBlob(0),
		responseStream(0),
		requestContentLength(0),
		requestDataSent(0),
		responseDataReceived(0)
	{
		/**
		 * @tiapi(method=True, name=Network.HTTPClient.abort, since=0.3)
		 * @tiapi Aborts an in progress connection
		 */
		this->SetMethod("abort", &HTTPClientBinding::Abort);

		/**
		 * @tiapi(method=True, name=Network.HTTPClient.open, since=0.3)
		 * @tiapi Opens an HTTP connection
		 * @tiarg[String, method] The HTTP method to use e.g. POST
		 * @tiarg[String, url] The url to connect to
		 * @tiarg[Boolean, asynchronous, optional=True] Whether or not the request should be asynchronous (default: True)
		 * @tiarg[String, username, optional=True] The HTTP username to use
		 * @tiarg[String, password, optional=True] The HTTP password to use
		 * @tiresult[Boolean] return true if supplied arguments are valid
		 */
		this->SetMethod("open", &HTTPClientBinding::Open);

		/**
		 * @tiapi(method=True, name=Network.HTTPClient.setCredentials, since=0.8)
		 * @tiapi Set the authentication credentials for the HTTPClient.
		 * @tiarg[String, username] The username to use or an empty String to use none.
		 * @tiarg[String, password] The password to use or an empty String to use none.
		 *
		 * @tiapi(method=True, name=Network.HTTPClient.setBasicCredentials, since=0.7, deprecated=True)
		 * @tiapi Set the basic authentication credentials
		 * @tiarg[String, username] The username to use or an empty String to use none.
		 * @tiarg[String, password] The password to use or an empty String to use none.
		 */
		this->SetMethod("setCredentials", &HTTPClientBinding::SetCredentials);
		this->SetMethod("setBasicCredentials", &HTTPClientBinding::SetCredentials);

		/**
		 * @tiapi(method=True, name=Network.HTTPClient.setRequestHeader, since=0.3)
		 * @tiapi Sets a request header for the connection
		 * @tiarg[String, header] request header name
		 * @tiarg[String, value] request header value
		 */
		this->SetMethod("setRequestHeader", &HTTPClientBinding::SetRequestHeader);

		/**
		 * @tiapi(method=True, name=Network.HTTPClient.send, since=0.3)
		 * @tiapi Sends data through the HTTP connection
		 * @tiarg[Object|String|null, data, optional=True] Data to send to the server.
		 * @tiresult[Boolean] returns true if request dispatched successfully
		 */
		this->SetMethod("send", &HTTPClientBinding::Send);

		/**
		 * @tiapi(method=True, name=Network.HTTPClient.sendFile, since=0.3)
		 * @tiapi Sends the contents of a file as body content
		 * @tiarg[Titanium.Filesystem.File, file] the File object to send
		 */
		this->SetMethod("sendFile", &HTTPClientBinding::Send);

		/**
		 * @tiapi(method=True, name=Network.HTTPClient.receive, since=0.7)
		 * @tiapi Sends a request to the server and receive data with the provided handler.
		 * @tiarg[Object|Function, handler] A handler to receive the response data. handler can
		 * @tiarg either be Titanium.Filesystem.File or a Function.
		 * @tiarg[Object|String|null, data, optional=True] Data to send to the server.
		 * @tiresult[Boolean] returns true if request dispatched successfully
		 */
		this->SetMethod("receive", &HTTPClientBinding::Receive);

		/**
		 * @tiapi(method=True, name=Network.HTTPClient.getResponseHeader, since=0.3)
		 * @tiapi Return the value of a response header, given it's name. If the given
		 * @tiapi name occurs multiple times, this method will only return one occurence.
		 * @tiarg[String, name] The response header name.
		 * @tiresult[String] The value of the response header.
		 */
		this->SetMethod("getResponseHeader", &HTTPClientBinding::GetResponseHeader);

		/**
		 * @tiapi(method=True, name=Network.HTTPClient.getResponseHeaders, since=0.8)
		 * @tiapi Return all response headers as an array of two element arrays.
		 * @tiresult[Array<Array<String, String>>] The array of response headers.
		 */
		this->SetMethod("getResponseHeaders", &HTTPClientBinding::GetResponseHeaders);

		/**
		 * @tiapi(method=True, name=Network.HTTPClient.setCookie, since=0.7)
		 * @tiapi Set a HTTP cookie in the request.
		 * @tiarg[String, name] the cookie name
		 * @tiarg[String, value] the cookie value
		 */
		this->SetMethod("setCookie", &HTTPClientBinding::SetCookie);

		/**
		 * @tiapi(method=True, name=Network.HTTPClient.clearCookies, since=0.7)
		 * @tiapi Clear any cookies set on the request
		 */
		this->SetMethod("clearCookies", &HTTPClientBinding::ClearCookies);

		/**
		 * @tiapi(method=True, name=Network.HTTPClient.getCookie, since=0.7)
		 * @tiapi Get a HTTP cookie from the response.
		 * @tiarg[String, name] name of the cookie to get
		 * @tiresult[Network.HTTPCookie] a cookie or null if not found
		 */
		this->SetMethod("getCookie", &HTTPClientBinding::GetCookie);

		/**
		 * @tiapi(method=True, name=Network.HTTPClient.setTimeout, since=0.4)
		 * Sets the timeout for the request. The default timeout value is five
		 * minutes.
		 * @tiarg[Number, timeout] The new timeout value in milliseconds.
		 */
		this->SetMethod("setTimeout", &HTTPClientBinding::SetTimeout);

		/**
		 * @tiapi(method=True, name=Network.HTTPClient.getTimeout, since=0.8)
		 * Gets the timeout for the request.
		 * @tiresult[Number] timeout value in milliseconds
		 */
		this->SetMethod("getTimeout", &HTTPClientBinding::GetTimeout);

		/**
		 * @tiapi(method=True, name=Network.HTTPClient.getMaxRedirects, since=0.8)
		 * @tiapi Get the maximum number of redirects to follow. The default is -1, which means
		 * @tiapi that there is no maximum limit to the number of redirects to follow.
		 * @tiresult[Number] the maximum number of redirects to follow.
		 */
		this->SetMethod("getMaxRedirects", &HTTPClientBinding::GetMaxRedirects);

		/**
		 * @tiapi(method=True, name=Network.HTTPClient.setMaxRedirects, since=0.8)
		 * @tiapi Set the maximum number of redirects to follow. The default is -1, which means
		 * @tiapi that there is no maximum limit to the number of redirects to follow.
		 * @tiarg[Number, amount] the number of redirects to follow.
		 */
		this->SetMethod("setMaxRedirects", &HTTPClientBinding::SetMaxRedirects);

		/**
		 * @tiapi(property=True, type=Number, name=Network.HTTPClient.readyState, since=0.3)
		 * @tiapi The ready-state status for the connection
		 */
		this->SetInt("readyState", 0);

		/**
		 * @tiapi(property=True, type=Number, name=Network.HTTPClient.UNSENT, since=0.3)
		 * @tiai The UNSENT readyState property
		 */
		this->SetInt("UNSENT", 0);

		/**
		 * @tiapi(property=True, type=Number, name=Network.HTTPClient.OPENED, since=0.3)
		 * @tiapi The OPENED readyState property
		 */
		this->SetInt("OPENED", 1);

		/**
		 * @tiapi(property=True, type=Number, name=Network.HTTPClient.HEADERS_RECEIVED, since=0.3)
		 * @tiapi The HEADERS_RECEIVED readyState property
		 */
		this->SetInt("HEADERS_RECEIVED", 2);

		/**
		 * @tiapi(property=True, type=Number, name=Network.HTTPClient.LOADING, since=0.3)
		 * @tiapi The LOADING readyState property
		 */
		this->SetInt("LOADING", 3);

		/**
		 * @tiapi(property=True, type=Number, name=Network.HTTPClient.DONE, since=0.3)
		 * @tiapi The DONE readyState property
		 */
		this->SetInt("DONE", 4);

		/**
		 * @tiapi(property=True, type=String, name=Network.HTTPClient.responseText, since=0.3)
		 * @tiapi The response of an HTTP request as text
		 */
		this->SetNull("responseText");

		/**
		 * @tiapi(property=True, type=String, name=Network.HTTPClient.responseXML, since=0.3)
		 * @tiapi The response of an HTTP request as parsable XML
		 */
		this->SetNull("responseXML");

		/**
		 * @tiapi(property=True, type=String, name=Network.HTTPClient.responseData, since=0.8)
		 * @tiapi The response of an HTTP request as a Blob. Currently this property
		 * @tiapi is only valid after the request has been completed.
		 */
		this->SetNull("responseData");

		/**
		 * @tiapi(property=True, type=Number, name=Network.HTTPClient.status, since=0.3)
		 * @tiapi The response status code of an HTTP request
		 */
		this->SetNull("status");

		/**
		 * @tiapi(property=True, type=String, name=Network.HTTPClient.statusText, since=0.3)
		 * @tiapi The response status text of an HTTP Request
		 */
		this->SetNull("statusText");

		/**
		 * @tiapi(property=True, type=Boolean, name=Network.HTTPClient.timedOut, since=0.7)
		 * @tiapi True if HTTP request timed out
		 */
		this->SetBool("timedOut", false);

		/**
		 * @tiapi(property=True, type=String, name=Network.HTTPClient.url, since=0.7)
		 * @tiapi The request URL. This value will be updated on redirect events.
		 */
		this->SetNull("url");

		/**
		 * @tiapi(property=True, type=Number, name=Network.HTTPClient.dataSent, since=0.7)
		 * @tiapi Amount of data sent to server so far. Updated on HTTP_DATA_SENT event.
		 */
		this->SetInt("dataSent", 0);

		/**
		 * @tiapi(property=True, type=Number, name=Network.HTTPClient.dataReceived, since=0.7)
		 * @tiapi Amount of data received from server so far. Updated on HTTP_DATA_RECEIVED event.
		 */
		this->SetInt("dataReceived", 0);

		/**
		 * @tiapi(property=True, type=Boolean, name=Network.HTTPClient.connected, since=0.3)
		 * @tiapi Whether an HTTPClient object is connected or not
		 */
		this->SetBool("connected", false);

		/**
		 * @tiapi(property=True, type=Function, name=Network.HTTPClient.onreadystatechange, since=0.3)
		 * @tiapi The handler function that will be fired when the readyState code of
		 * @tiapi an HTTPClient object changes.
		 */
		this->SetNull("onreadystatechange");

		/**
		 * @tiapi(property=True, type=Function, name=Network.HTTPClient.ondatastream, since=0.3)
		 * @tiapi The handler function that will be fired as stream data is received from an HTTP request
		 */
		this->SetNull("ondatastream");

		/**
		 * @tiapi(property=True, type=Function, name=Network.HTTPClient.onsendstream, since=0.3)
		 * @tiapi The handler function that will be fired as the stream data is sent.
		 */
		this->SetNull("onsendstream");

		/**
		 * @tiapi(property=True, type=Function, name=Network.HTTPClient.onload, since=0.7)
		 * @tiapi The handler function that will be fired when request is completed
		*/
		this->SetNull("onload");

		/**
		 * @tiapi(property=True, name=Network.HTTPClient.userAgent, since=0.7)
		 * @tiapi User agent string to use for requests. (Default: PRODUCTNAME/PRODUCT_VERSION)
		 */
		this->SetString("userAgent", PRODUCT_NAME"/"PRODUCT_VERSION);
	}

	HTTPClientBinding::~HTTPClientBinding()
	{
	}

	void HTTPClientBinding::Abort(const ValueList& args, KValueRef result)
	{
		this->aborted = true;
	}

	void HTTPClientBinding::Open(const ValueList& args, KValueRef result)
	{
		args.VerifyException("open", "s s ?b s s");

		this->httpMethod = args.GetString(0);
		Poco::toUpperInPlace(this->httpMethod);
		if (this->httpMethod.empty())
			this->httpMethod = "GET";

		this->url = args.GetString(1);
		this->SetString("url", this->url);

		// TODO(mrobinson): If the scheme is a app:// or ti:// we should just
		// convert the URL to a file URL here.
		const std::string scheme = Poco::URI(url).getScheme();
		if (scheme != "http" && scheme != "https" && scheme != "file")
		{
			throw ValueException::FromFormat("%s scheme is not supported by HTTPClient",
			scheme.c_str());
			return;
		}

		if (args.size() >= 3)
		{
			this->async = args.GetBool(2);
		}

		if (args.size() >= 4)
			this->username = args.GetString(3);
		if (args.size() >= 5)
			this->password = args.GetString(4);

		// Get on*** handler functions
		this->ondatastream = this->GetMethod("ondatastream");
		this->onreadystate = this->GetMethod("onreadystatechange");
		this->onsendstream = this->GetMethod("onsendstream");
		this->onload = this->GetMethod("onload");

		this->ChangeState(1); // opened
		result->SetBool(true);
	}

	void HTTPClientBinding::SetCredentials(const ValueList& args, KValueRef result)
	{
		args.VerifyException("setCredentials", "s s");
		this->username = args.GetString(0);
		this->password = args.GetString(1);
	}

	void HTTPClientBinding::Send(const ValueList& args, KValueRef result)
	{
		// Get send data if provided
		args.VerifyException("send", "?s|o|0");
		KValueRef sendData(args.GetValue(0));

		// Setup output stream for data
		this->responseStream = new std::ostringstream(std::ios::binary | std::ios::out);
		result->SetBool(this->BeginRequest(sendData));
	}

	void HTTPClientBinding::Receive(const ValueList& args, KValueRef result)
	{
		args.VerifyException("receive", "m|o ?s|o|0");

		// Set output handler
		this->responseStream = 0;
		result->SetBool(false);

		if (args.at(0)->IsMethod())
		{
			this->outputHandler = args.at(0)->ToMethod();
		}
		else if (args.at(0)->IsObject())
		{
			KObjectRef handlerObject(args.at(0)->ToObject());
			KMethodRef writeMethod(handlerObject->GetMethod("write", 0));
			if (writeMethod.isNull())
			{
				GetLogger()->Error("Unsupported object type as output handler:"
					" does not have write method");
			}
			else
			{
				this->outputHandler = writeMethod;
			}
		}
		else
		{
			GetLogger()->Error("Invalid type as output handler!");
			return;
		}

		// Get the send data if provided
		KValueRef sendData(args.GetValue(1));
		result->SetBool(this->BeginRequest(sendData));
	}

	void HTTPClientBinding::SetRequestHeader(const ValueList& args, KValueRef result)
	{
		args.VerifyException("setRequestHeader", "s s");
		std::string key(args.GetString(0));
		std::string value(args.GetString(1));

		if (key[key.size() - 1] != ':')
			key.append(": ");

		// An empty header value tells cURL to unset this header.
		if (value.empty())
			key.append("\"\"");
		else
			key.append(value);

		this->requestHeaders.push_back(key);
	}

	void HTTPClientBinding::GetResponseHeader(const ValueList& args, KValueRef result)
	{
		args.VerifyException("getResponseHeader", "s");
		std::string name(args.GetString(0));

		if (this->responseHeaders.has(name))
		{
			result->SetString(this->responseHeaders[name].c_str());
		}
		else
		{
			result->SetNull();
		}
	}

	void HTTPClientBinding::GetResponseHeaders(const ValueList& args, KValueRef result)
	{
		KListRef headers(new StaticBoundList());

		NameValueCollection::ConstIterator i = this->responseHeaders.begin();
		while (i != this->responseHeaders.end())
		{
			KListRef headerEntry(new StaticBoundList());
			headerEntry->Append(Value::NewString(i->first));
			headerEntry->Append(Value::NewString(i->second));
			headers->Append(Value::NewList(headerEntry));
			i++;
		}

		result->SetList(headers);
	}

	void HTTPClientBinding::SetCookie(const ValueList& args, KValueRef result)
	{
		args.VerifyException("setCookie", "ss");
		this->requestCookies.add(args.GetString(0), args.GetString(1));
	}

	void HTTPClientBinding::ClearCookies(const ValueList& args, KValueRef result)
	{
		this->requestCookies.clear();
	}

	void HTTPClientBinding::GetCookie(const ValueList& args, KValueRef result)
	{
		args.VerifyException("getCookie", "s");
		std::string cookieName = args.GetString(0);

		if (this->responseCookies.find(cookieName) != this->responseCookies.end())
		{
			result->SetObject(this->responseCookies[cookieName]);
		}
		else
		{
			result->SetNull();
		}
	}

	void HTTPClientBinding::SetTimeout(const ValueList& args, KValueRef result)
	{
		args.VerifyException("setTimeout", "i");
		this->timeout = args.GetInt(0);
	}

	void HTTPClientBinding::GetTimeout(const ValueList& args, KValueRef result)
	{
		result->SetInt(this->timeout);
	}

	void HTTPClientBinding::GetMaxRedirects(const ValueList& args, KValueRef result)
	{
		result->SetInt(this->maxRedirects);
	}

	void HTTPClientBinding::SetMaxRedirects(const ValueList& args, KValueRef result)
	{
		args.VerifyException("setMaxRedirects", "n");
		this->maxRedirects = args.GetInt(0);
	}

	bool HTTPClientBinding::FireEvent(std::string& eventName)
	{
		// We're already exposed as an AutoPtr somewhere else, so we must create
		// an AutoPtr version of ourselves with the 'shared' argument set to true.
		ValueList args(Value::NewObject(GetAutoPtr()));

		// Must invoke the on*** handler functions
		if (eventName == Event::HTTP_STATE_CHANGED && !this->onreadystate.isNull())
		{
			RunOnMainThread(this->onreadystate, GetAutoPtr(), args, true);

			if (this->Get("readyState")->ToInt() == 4 && !this->onload.isNull())
			{
				RunOnMainThread(this->onload, GetAutoPtr(), args, true);
			}
		}
		else if (eventName == Event::HTTP_DATA_SENT && !this->onsendstream.isNull())
		{
			RunOnMainThread(this->onsendstream, GetAutoPtr(), args, true);
		}
		else if (eventName == Event::HTTP_DATA_RECEIVED && !this->ondatastream.isNull())
		{
			RunOnMainThread(this->ondatastream, GetAutoPtr(), args, true);
		}

		return KEventObject::FireEvent(eventName);
	}

	void HTTPClientBinding::run()
	{
		START_KROLL_THREAD;

		// We need this binding to stay alive at least until we have
		// finished this thread. So save 'this' in an AutoPtr.
		KObjectRef save(this, true);
		this->ExecuteRequest();

		END_KROLL_THREAD;
	}

	void HTTPClientBinding::BeginWithFileLikeObject(KObjectRef dataObject)
	{
		KMethodRef nativePathMethod(dataObject->GetMethod("nativePath"));
		KMethodRef sizeMethod(dataObject->GetMethod("size"));

		if (nativePathMethod.isNull() || sizeMethod.isNull())
		{
			std::string err("Unsupported File-like object: did not have"
				"nativePath and size methods");
			GetLogger()->Error(err);
			throw ValueException::FromString(err);
		}

		KValueRef filenameValue(nativePathMethod->Call());
		if (!filenameValue->IsString())
		{
			std::string err("Unsupported File-like object: nativePath method"
				"did not return a String");
			GetLogger()->Error(err);
			throw ValueException::FromString(err);
		}

		std::string filename(filenameValue->ToString());
		this->requestStream = new std::ifstream(
			filename.c_str(), std::ios::in | std::ios::binary);
		if (this->requestStream->fail())
		{
			std::string err("Failed to open file: ");
			err.append(filename);
			GetLogger()->Error(err);
			throw ValueException::FromString(err);
		}

		this->requestContentLength = sizeMethod->Call()->ToInt();
	}

	bool HTTPClientBinding::BeginRequest(KValueRef sendData)
	{
		if (this->curlHandle)
			throw ValueException::FromString("Tried to use an HTTPClient while "
				"another transfer was in progress");

		this->sawHTTPStatus = false;
		this->requestDataSent = 0;
		this->responseDataReceived = 0;
		this->responseCookies.clear();
		this->aborted = false;
		this->requestBlob = 0;
		this->requestStream = 0;
		this->responseData.clear();

		this->SetInt("dataSent", 0);
		this->SetInt("dataReceived", 0);

		this->SetBool("timedOut", false);
		this->SetNull("responseText");
		this->SetNull("responseData");
		this->SetNull("status");
		this->SetNull("statusText");

		if (sendData->IsObject())
		{
			KObjectRef dataObject(sendData->ToObject());
			BlobRef blob(dataObject.cast<Blob>());
			if (blob.isNull())
			{
				this->BeginWithFileLikeObject(dataObject);
			}
			else
			{
				this->requestBlob = blob;
			}
		}
		else if (sendData->IsString())
		{
			const char* sendChars = sendData->ToString();
			this->requestContentLength = strlen(sendChars);
			this->requestBlob = new Blob(sendChars, this->requestContentLength);
		}
		else // Sending no data
		{
			this->requestStream = 0;
			this->requestBlob = 0;
			this->requestContentLength = 0;
		}

		if (this->async)
		{
			this->thread = new Poco::Thread();
			this->thread->start(*this);
		}
		else
		{
			this->ExecuteRequest();
		}

		return true;
	}

	void HTTPClientBinding::ChangeState(int readyState)
	{
		GetLogger()->Debug("Changing readyState from %d to %d for url:%s",
			this->GetInt("readyState", 0), readyState, this->url.c_str());
		this->SetInt("readyState", readyState);
		this->FireEvent(Event::HTTP_STATE_CHANGED);

		if (readyState == 4)
			this->FireEvent(Event::HTTP_DONE);
	}

	static void SplitParameters(const std::string::const_iterator& begin,
		const std::string::const_iterator& end, NameValueCollection& parameters)
	{
		std::string pname;
		std::string pvalue;
		pname.reserve(32);
		pvalue.reserve(64);
		std::string::const_iterator it = begin;
		while (it != end)
		{
			pname.clear();
			pvalue.clear();
			while (it != end && std::isspace(*it)) ++it;
			while (it != end && *it != '=' && *it != ';') pname += *it++;
			Poco::trimRightInPlace(pname);
			if (it != end && *it != ';') ++it;
			while (it != end && std::isspace(*it)) ++it;
			while (it != end && *it != ';')
			{
				if (*it == '"')
				{
					++it;
					while (it != end && *it != '"')
					{
						if (*it == '\\')
						{
							++it;
							if (it != end) pvalue += *it++;
						}
						else pvalue += *it++;
					}
					if (it != end) ++it;
				}
				else if (*it == '\\')
				{
					++it;
					if (it != end) pvalue += *it++;
				}
				else pvalue += *it++;
			}
			Poco::trimRightInPlace(pvalue);
			if (!pname.empty()) parameters.add(pname, pvalue);
			if (it != end) ++it;
		}
	}

	void HTTPClientBinding::GetResponseCookie(std::string cookieLine)
	{
		NameValueCollection cookiePairs;
		SplitParameters(cookieLine.begin(), cookieLine.end(), cookiePairs);
		Poco::Net::HTTPCookie pocoCookie(cookiePairs);
		responseCookies[pocoCookie.getName()] = new HTTPCookie(pocoCookie);
	}

	struct curl_slist* HTTPClientBinding::SetRequestHeaders(CURL* handle)
	{
		if (requestHeaders.empty())
			return NULL;

		struct curl_slist* curlHeaders = NULL;
		for (size_t i = 0; i < requestHeaders.size(); i++)
			curlHeaders = curl_slist_append(curlHeaders, requestHeaders[i].c_str());

		SET_CURL_OPTION(curlHandle, CURLOPT_HTTPHEADER, curlHeaders);
		return curlHeaders;
	}

	void SetRequestCookies(CURL* handle, NameValueCollection& cookies)
	{
		if (cookies.empty())
			return;

		std::string cookieString;
		NameValueCollection::ConstIterator i = cookies.begin();
		while (i != cookies.end())
		{
			cookieString.append(i->first);
			cookieString.append("=");
			cookieString.append(i->second);
			cookieString.append(";");

			i++;
		}

		SET_CURL_OPTION(handle, CURLOPT_COOKIE, cookieString.c_str());
	}

	size_t HTTPClientBinding::WriteRequestDataToBuffer(char* buffer, size_t bufferSize)
	{
		size_t bytesSent = 0;
		if (!requestStream.isNull() && !requestStream->eof())
		{
			requestStream->read(buffer, bufferSize);
			bytesSent = requestStream->gcount();

			if (requestStream->eof())
				requestStream->close();
		}
		else if (!requestBlob.isNull())
		{
			size_t toSend = bufferSize;
			if (requestBlob->Length() - requestDataSent < bufferSize)
				toSend = requestBlob->Length() - requestDataSent;

			if (toSend > 0)
			{
				memcpy(buffer, requestBlob->Get() + requestDataSent, toSend);
				bytesSent = toSend;
			}
		}

		this->requestDataSent += bytesSent;
		this->SetInt("dataSent", requestDataSent);
		this->FireEvent(Event::HTTP_DATA_SENT);

		return bytesSent;
	}

	// This callback is invoked when cURL needs to send data to the server.
	static size_t CurlReadCallback(void* ptr, size_t size, size_t nmemb,
		HTTPClientBinding* client)
	{
		size_t bufferSize = size * nmemb;
		return client->WriteRequestDataToBuffer(static_cast<char*>(ptr), bufferSize);
	}

	void HTTPClientBinding::ParseHTTPStatus(std::string& header)
	{
		size_t numberOfSpaces = 0;
		size_t position = 0;
		while (numberOfSpaces < 2 && position < header.size())
		{
			if (header[position] == ' ')
				numberOfSpaces++;
			position++;
		}

		if (position >= header.size())
			return;

		this->SetString("statusText", FileUtils::Trim(header.substr(position)).c_str());
	}

	void HTTPClientBinding::GotHeader(std::string& header)
	{
		// We want to keep all the headers here and only set them on the client
		// in chunks. The reason for this is that this is that cURL uses this
		// callback for all headers even those used in authentication negotiation
		// and redirects. We only want the last chunk of headers to be on the client
		// at the end of the request.
		if (header == "\r\n" || header == "\n") // This is the end of header chunk
		{
			this->sawHTTPStatus = false;
			this->responseHeaders = this->nextResponseHeaders;
			this->nextResponseHeaders.clear();

			// TODO(mrobinson): We need to parse the status text eventually.
			long httpStatus = 0;
			curl_easy_getinfo(this->curlHandle, CURLINFO_RESPONSE_CODE, &httpStatus);
			this->SetInt("status", httpStatus);

			const char* effectiveURL;
			curl_easy_getinfo(this->curlHandle, CURLINFO_EFFECTIVE_URL, &effectiveURL);

			// Update the URL in the case that this is redirect
			this->url = effectiveURL;
			this->SetString("url", this->url);
		}
		else // Normal header
		{
			if (!sawHTTPStatus && header.find("HTTP") == 0)
			{
				this->ParseHTTPStatus(header);
				sawHTTPStatus = true;
				return;
			}

			size_t splitPos = header.find(":");
			if (splitPos == std::string::npos)
				return;

			std::string headerName(header.substr(0, splitPos));
			std::string headerValue(FileUtils::Trim(header.substr(splitPos + 1)));
			nextResponseHeaders.add(headerName, headerValue);

			Poco::toLowerInPlace(headerName);
			if (headerName == "set-cookie")
				this->GetResponseCookie(headerValue);
		}
	}

	static size_t CurlHeaderCallback(void* ptr, size_t size, size_t nmemb,
		HTTPClientBinding* client)
	{
		size_t headerLineSize = size * nmemb;
		std::string header(static_cast<char*>(ptr), headerLineSize);
		client->GotHeader(header);

		return headerLineSize;
	}

	void HTTPClientBinding::DataReceived(char* buffer, size_t bufferSize)
	{
		// Pass data to handler on main thread
		BlobRef blob(new Blob(buffer, bufferSize, true));
		responseData.push_back(blob);

		if (this->responseStream)
		{
			this->responseStream->write(buffer, bufferSize);
			std::string data(this->responseStream->str());
			if (!data.empty())
				this->SetString("responseText", data);
		}

		if (this->outputHandler)
		{
			RunOnMainThread(this->outputHandler, GetAutoPtr(),
				ValueList(Value::NewObject(blob)));
		}

		responseDataReceived += bufferSize;
		this->SetInt("dataReceived", responseDataReceived);
		this->FireEvent(Event::HTTP_DATA_RECEIVED);
	}

	// This callback is invoked when cURL needs to handle data coming from the server
	static size_t CurlWriteCallback(void* buffer, size_t size, size_t nmemb,
		HTTPClientBinding* client)
	{
		size_t dataLength = size * nmemb;
		client->DataReceived(static_cast<char*>(buffer), dataLength);
		return dataLength;
	}

	int CurlProgressCallback(HTTPClientBinding *client, double , double , double , double)
	{
		if (client->IsAborted())
			return CURLE_ABORTED_BY_CALLBACK;
		else
			return 0;
	}

	void HTTPClientBinding::SetupCurlMethodType()
	{
		// Modify the HTTP method based on the method variable. The default
		// in cURL is to use GET.
		if (this->httpMethod == "POST")
		{
			SET_CURL_OPTION(curlHandle, CURLOPT_POST, 1);
		}
		else if (this->httpMethod == "PUT")
		{
			SET_CURL_OPTION(curlHandle, CURLOPT_UPLOAD, 1);
		}
		else if (this->httpMethod == "HEAD")
		{
			SET_CURL_OPTION(curlHandle, CURLOPT_NOBODY, 1);
		}
		else if (this->httpMethod != "GET")
		{
			// This may work and it may not depending on the method type.
			// DELETE should work, in particular.
			SET_CURL_OPTION(curlHandle, CURLOPT_CUSTOMREQUEST, this->httpMethod.c_str());
		}

		// Only set up the read handler if there is data to send to the server.
		if (this->requestContentLength > 0)
		{
			SET_CURL_OPTION(curlHandle, CURLOPT_READDATA, this);
			SET_CURL_OPTION(curlHandle, CURLOPT_READFUNCTION, &CurlReadCallback);

			if (this->httpMethod == "PUT")
			{
				SET_CURL_OPTION(curlHandle, CURLOPT_INFILESIZE, requestContentLength);
			}
			else if (this->httpMethod == "POST")
			{
				SET_CURL_OPTION(curlHandle, CURLOPT_POSTFIELDSIZE, requestContentLength);
			}
		}
	}

	void HTTPClientBinding::HandleCurlResult(CURLcode result)
	{
		if (result == CURLE_OPERATION_TIMEDOUT)
		{
			GetLogger()->Error("Failed for URL (%s): %s", this->url.c_str(),
				curl_easy_strerror(result));
			this->SetBool("timedOut", true);
			this->FireEvent(Event::HTTP_TIMEOUT);
		}
		else if (result == CURLE_ABORTED_BY_CALLBACK)
		{
			GetLogger()->Error("Failed for URL (%s): %s", this->url.c_str(),
				curl_easy_strerror(result));
			this->FireEvent(Event::HTTP_ABORT);
		}
		else if (result != CURLE_OK)
		{
			GetLogger()->Error("Failed for URL (%s): %s", this->url.c_str(),
				curl_easy_strerror(result));
		}
		else
		{
			this->ChangeState(2); // Headers received
			this->ChangeState(3); // Loading
		}
	}

	void HTTPClientBinding::CleanupCurl(curl_slist* headers)
	{
		if (this->curlHandle)
		{
			curl_easy_cleanup(this->curlHandle);
			this->curlHandle = 0;
		}

		if (headers)
			curl_slist_free_all(headers);
	}

	static void SetStandardCurlHandleOptions(CURL* handle)
	{
		SET_CURL_OPTION(handle, CURLOPT_SSL_VERIFYPEER, false);
		SET_CURL_OPTION(handle, CURLOPT_CAINFO, NetworkModule::GetRootCertPath().c_str());

		// If a timeout happens, this normally causes cURL to fire a signal.
		// Since we're running multiple threads and possibily have multiple HTTP
		// requests going at once, we need to disable this behavior.
		SET_CURL_OPTION(handle, CURLOPT_NOSIGNAL, 1);

		// See also: CURLOPT_HEADERDATA, CURLOPT_WRITEFUNCTION, etc below.
		SET_CURL_OPTION(handle, CURLOPT_HEADERFUNCTION, &CurlHeaderCallback);
		SET_CURL_OPTION(handle, CURLOPT_WRITEFUNCTION, &CurlWriteCallback);
		SET_CURL_OPTION(handle, CURLOPT_PROGRESSFUNCTION, &CurlProgressCallback);
		SET_CURL_OPTION(handle, CURLOPT_NOPROGRESS, 0);

		// Enable all supported Accept-Encoding values.
		SET_CURL_OPTION(handle, CURLOPT_ENCODING, "");

		SET_CURL_OPTION(handle, CURLOPT_FOLLOWLOCATION, 1);
		SET_CURL_OPTION(handle, CURLOPT_AUTOREFERER, 1);

		static std::string cookieJarFilename;
		if (cookieJarFilename.empty())
		{
			cookieJarFilename = FileUtils::Join(
				Host::GetInstance()->GetApplication()->GetDataPath().c_str(),
				"network_httpclient_cookies.dat", 0);
		}

		// cURL doesn't have built in thread support, so we must handle thread-safety
		// via the CURLSH callback API.
		SET_CURL_OPTION(handle, CURLOPT_SHARE, NetworkModule::GetCurlShareHandle());
		SET_CURL_OPTION(handle, CURLOPT_COOKIEFILE, cookieJarFilename.c_str());
		SET_CURL_OPTION(handle, CURLOPT_COOKIEJAR, cookieJarFilename.c_str());
	}

	static void SetCurlProxySettings(CURL* curlHandle, SharedProxy proxy)
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

	void HTTPClientBinding::ExecuteRequest()
	{
		struct curl_slist* curlHeaders = 0;
		char curlErrorBuffer[CURL_ERROR_SIZE];

		try
		{
			this->curlHandle = curl_easy_init();
			SetStandardCurlHandleOptions(curlHandle);

			// This error buffer cannot be shared, because it's not protected by a mutex.
			SET_CURL_OPTION(curlHandle, CURLOPT_URL, url.c_str());
			SET_CURL_OPTION(curlHandle, CURLOPT_ERRORBUFFER, &curlErrorBuffer);

			SET_CURL_OPTION(curlHandle, CURLOPT_WRITEHEADER, this);
			SET_CURL_OPTION(curlHandle, CURLOPT_WRITEDATA, this);
			SET_CURL_OPTION(curlHandle, CURLOPT_PROGRESSDATA, this);

			this->SetupCurlMethodType();

			SET_CURL_OPTION(curlHandle, CURLOPT_MAXREDIRS, this->maxRedirects);
			SET_CURL_OPTION(curlHandle, CURLOPT_USERAGENT,
				this->GetString("userAgent").c_str());

			curlHeaders = SetRequestHeaders(curlHandle);
			SetCurlProxySettings(curlHandle, ProxyConfig::GetProxyForURL(url));

			if (this->timeout > 0)
			{
				SET_CURL_OPTION(curlHandle, CURLOPT_TIMEOUT_MS, this->timeout);
				SET_CURL_OPTION(curlHandle, CURLOPT_DNS_CACHE_TIMEOUT, this->timeout/1000);
			}

			SetRequestCookies(curlHandle, this->requestCookies);

			if (!this->username.empty() || !this->password.empty())
			{
				std::string usernamePassword(this->username);
				usernamePassword.append(":");
				usernamePassword.append(this->password);
				SET_CURL_OPTION(curlHandle, CURLOPT_USERPWD, usernamePassword.c_str());
			}

			this->Set("connected", Value::NewBool(true));
			this->HandleCurlResult(curl_easy_perform(curlHandle));
			this->Set("connected", Value::NewBool(false));

			if (!responseData.empty())
				this->SetObject("responseData", Blob::GlobBlobs(this->responseData));

			this->ChangeState(4); // Done
		}
		catch (ValueException& e)
		{
			GetLogger()->Error("Request to %s failed because: %s",
			this->url.c_str(), e.ToString().c_str());

			this->CleanupCurl(curlHeaders);
			if (!async)
				throw e;
		}

		CleanupCurl(curlHeaders);
	}
}

