/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009-2010 Appcelerator, Inc. All Rights Reserved.
 */

#include "../network_module.h"
#include "http_client_binding.h"
#include <kroll/thread_manager.h>
#include "../common.h"
#include <sstream>

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
		requestBytes(0),
		responseStream(0),
		requestContentLength(0),
		requestDataSent(0),
		requestDataWritten(0),
		responseDataReceived(0),
		postData(0),
		sendData(0)
	{
		this->SetMethod("abort", &HTTPClientBinding::Abort);
		this->SetMethod("open", &HTTPClientBinding::Open);
		this->SetMethod("setCredentials", &HTTPClientBinding::SetCredentials);
		this->SetMethod("setBasicCredentials", &HTTPClientBinding::SetCredentials);
		this->SetMethod("setRequestHeader", &HTTPClientBinding::SetRequestHeader);
		this->SetMethod("send", &HTTPClientBinding::Send);
		this->SetMethod("sendFile", &HTTPClientBinding::Send);
		this->SetMethod("receive", &HTTPClientBinding::Receive);
		this->SetMethod("getResponseHeader", &HTTPClientBinding::GetResponseHeader);
		this->SetMethod("getResponseHeaders", &HTTPClientBinding::GetResponseHeaders);
		this->SetMethod("setCookie", &HTTPClientBinding::SetCookie);
		this->SetMethod("clearCookies", &HTTPClientBinding::ClearCookies);
		this->SetMethod("getCookie", &HTTPClientBinding::GetCookie);
		this->SetMethod("setTimeout", &HTTPClientBinding::SetTimeout);
		this->SetMethod("getTimeout", &HTTPClientBinding::GetTimeout);
		this->SetMethod("getMaxRedirects", &HTTPClientBinding::GetMaxRedirects);
		this->SetMethod("setMaxRedirects", &HTTPClientBinding::SetMaxRedirects);
		this->SetInt("readyState", 0);
		this->SetInt("UNSENT", 0);
		this->SetInt("OPENED", 1);
		this->SetInt("HEADERS_RECEIVED", 2);
		this->SetInt("LOADING", 3);
		this->SetInt("DONE", 4);
		this->SetNull("responseText");
		this->SetNull("responseXML");
		this->SetNull("responseData");
		this->SetNull("status");
		this->SetNull("statusText");
		this->SetBool("timedOut", false);
		this->SetNull("url");
		this->SetInt("dataSent", 0);
		this->SetInt("dataReceived", 0);
		this->SetBool("connected", false);
		this->SetNull("onreadystatechange");
		this->SetNull("ondatastream");
		this->SetNull("onsendstream");
		this->SetNull("onload");
		this->SetString("userAgent", GlobalObject::GetInstance()->GetString("userAgent"));

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
		if (eventName == Event::HTTP_STATE_CHANGED)
		{
			if (!this->onreadystate.isNull())
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

	static std::string ObjectToFilename(KObjectRef dataObject)
	{
		// Now try to treat this object like as a file-like object with
		// a .read() method which returns a Bytes. If this fails we'll
		// return NULL.
		KMethodRef nativePathMethod(dataObject->GetMethod("nativePath", 0));
		if (nativePathMethod.isNull())
			return "data";

		KValueRef pathValue(nativePathMethod->Call());
		if (!pathValue->IsString())
			return "data";

		// If this cast fails, it will return NULL, as we expect.
		return FileUtils::Basename(pathValue->ToString());
	}

	void HTTPClientBinding::AddScalarValueToCurlForm(SharedString propertyName,
		KValueRef value, curl_httppost** last)
	{
		if (value->IsString())
		{
			curl_formadd(&this->postData, last,
				CURLFORM_COPYNAME, propertyName->c_str(),
				CURLFORM_COPYCONTENTS, value->ToString(),
				CURLFORM_END);
		}
		else if (value->IsObject())
		{
			BytesRef bytes(ObjectToBytes(value->ToObject()));
			if (!bytes.isNull())
			{
				curl_formadd(&this->postData, last,
					CURLFORM_COPYNAME, propertyName->c_str(),
					CURLFORM_BUFFER, ObjectToFilename(value->ToObject()).c_str(),
					CURLFORM_BUFFERPTR, bytes->Get(),
					CURLFORM_BUFFERLENGTH, bytes->Length(),
					CURLFORM_END);

				// We need to preserve the Bytes data until the end of this
				// request. This prevents us from having to copy the data with
				// something like CURLFROM_COPYCONTENTS above.
				preservedPostData.push_back(bytes);
			}
		}
		else
		{
			// If we've gotten here we have not been able to convert this object
			// through any normal means, so we just use the DisplayString of the value.
			SharedString ss(value->DisplayString());
			curl_formadd(&this->postData, last,
				CURLFORM_COPYNAME, propertyName->c_str(),
				CURLFORM_COPYCONTENTS, ss->c_str(),
				CURLFORM_END);
		}
	}

	void HTTPClientBinding::BeginWithPostDataObject(KObjectRef object)
	{
		struct curl_httppost* last = 0;

		SharedStringList properties = object->GetPropertyNames();
		for (unsigned int i = 0; i < properties->size(); i++)
		{
			SharedString propertyName(properties->at(i));
			KValueRef value(object->Get(propertyName->c_str()));
			if (value->IsList())
			{
				KListRef list(value->ToList());
				for (unsigned int i = 0; i < list->Size(); i++)
					this->AddScalarValueToCurlForm(propertyName, list->At(i), &last);
			}
			else
			{
				this->AddScalarValueToCurlForm(propertyName, value, &last);
			}
		}
	}

	bool HTTPClientBinding::BeginRequest(KValueRef sendData)
	{
		if (this->curlHandle)
			throw ValueException::FromString("Tried to use an HTTPClient while "
				"another transfer was in progress");

		this->sendData = sendData;
		this->sawHTTPStatus = false;
		this->requestDataSent = 0;
		this->requestDataWritten = 0;
		this->responseDataReceived = 0;
		this->responseCookies.clear();
		this->aborted = false;
		this->requestBytes = 0;
		this->responseData.clear();

		this->SetInt("dataSent", 0);
		this->SetInt("dataReceived", 0);

		this->SetBool("timedOut", false);
		this->SetNull("responseText");
		this->SetNull("responseData");
		this->SetNull("status");
		this->SetNull("statusText");

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
		if (requestBytes.isNull())
			return 0;

		size_t bytesSent = 0;
		size_t toSend = bufferSize;
		if (requestBytes->Length() - requestDataWritten < bufferSize)
			toSend = requestBytes->Length() - requestDataWritten;

		if (toSend > 0)
		{
			memcpy(buffer, requestBytes->Get() + requestDataWritten, toSend);
			bytesSent = toSend;
		}
		

		requestDataWritten += bytesSent;
		return bytesSent;
	}

	void HTTPClientBinding::RequestDataSent(size_t sent, size_t total)
	{
		// Firing an event is a fairly expensive operation, so only fire
		// an HTTP_DATA_SENT event if we've transferred at least 250KB of data.
		// or we have completed the transfer.
		if (total == requestDataSent)
			return;

		if (sent == total || sent > this->requestDataSent + (250 * 1024))
		{
			this->requestDataSent = sent;
			this->SetInt("dataSent", sent);
			this->FireEvent(Event::HTTP_DATA_SENT);
		}
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
		BytesRef bytes(new Bytes(buffer, bufferSize, true));
		responseData.push_back(bytes);

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
				ValueList(Value::NewObject(bytes)));
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

	int CurlProgressCallback(HTTPClientBinding* client, double dltotal, double dlnow, double ultotal, double ulnow)
	{
		if (client->IsAborted())
			return CURLE_ABORTED_BY_CALLBACK;
		else
			return 0;

		client->RequestDataSent(ulnow, ultotal);
	}

	void HTTPClientBinding::SetupCurlMethodType()
	{
		// Only set up the read handler if there is data to send to the server
		// and we aren't using the POST fields (i.e. we were just given a blob
		// of data).
		if (this->requestContentLength > 0)
		{
			if (this->httpMethod == "POST")
			{
				SET_CURL_OPTION(curlHandle, CURLOPT_POST, 1);
				SET_CURL_OPTION(curlHandle, CURLOPT_POSTFIELDSIZE, requestContentLength);
				SET_CURL_OPTION(curlHandle, CURLOPT_POSTFIELDS, this->requestBytes->Get());
			}
			else
			{
				SET_CURL_OPTION(curlHandle, CURLOPT_CUSTOMREQUEST, this->httpMethod.c_str());
				SET_CURL_OPTION(curlHandle, CURLOPT_READDATA, this);
				SET_CURL_OPTION(curlHandle, CURLOPT_READFUNCTION, &CurlReadCallback);
				SET_CURL_OPTION(curlHandle, CURLOPT_INFILESIZE, requestContentLength);
			}
		}
		else
		{
			// Modify the HTTP method based on the method variable. The default
			// for cURL is to use GET, so do nothing in that case.
			if (this->httpMethod == "POST" && this->postData)
			{
				curl_easy_setopt(this->curlHandle, CURLOPT_HTTPPOST, this->postData);
			}
			else if (this->httpMethod == "POST")
			{
				SET_CURL_OPTION(curlHandle, CURLOPT_POST, 1);
				SET_CURL_OPTION(curlHandle, CURLOPT_POSTFIELDSIZE, 0);
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
		if (this->postData)
		{
			curl_formfree(this->postData);
			this->postData = 0;
			preservedPostData.clear();
		}

		if (this->curlHandle)
		{
			curl_easy_cleanup(this->curlHandle);
			this->curlHandle = 0;
		}

		if (headers)
			curl_slist_free_all(headers);
	}

	void HTTPClientBinding::SetRequestData()
	{
		if (this->sendData->IsObject())
		{
			BytesRef bytes(ObjectToBytes(this->sendData->ToObject()));
			if (!bytes.isNull())
			{
				this->requestBytes = bytes;
				this->requestContentLength = bytes->Length();
			}
			else
			{
				// Well this is just a plain-ole object, so treat it as
				// a key-value store of POST parameters.
				this->BeginWithPostDataObject(this->sendData->ToObject());
				this->requestBytes = 0;
				this->requestContentLength = 0;
			}
		}
		else if (this->sendData->IsString())
		{
			const char* sendChars = this->sendData->ToString();
			this->requestContentLength = strlen(sendChars); // Include NUL character.
			this->requestBytes = new Bytes(sendChars, this->requestContentLength);
		}
		else // Sending no data
		{
			this->requestBytes = 0;
			this->requestContentLength = 0;
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

			SET_CURL_OPTION(curlHandle, CURLOPT_HEADERFUNCTION, &CurlHeaderCallback);
			SET_CURL_OPTION(curlHandle, CURLOPT_WRITEFUNCTION, &CurlWriteCallback);
			SET_CURL_OPTION(curlHandle, CURLOPT_PROGRESSFUNCTION, &CurlProgressCallback);
			SET_CURL_OPTION(curlHandle, CURLOPT_WRITEHEADER, this);
			SET_CURL_OPTION(curlHandle, CURLOPT_WRITEDATA, this);
			SET_CURL_OPTION(curlHandle, CURLOPT_PROGRESSDATA, this);
			// non negative number means don't verify peer cert - we might want to 
			// make this configurable in the future
			SET_CURL_OPTION(curlHandle, CURLOPT_SSL_VERIFYPEER, 1);

			// Progress must be turned on for CURLOPT_PROGRESSFUNCTION to be called.
			SET_CURL_OPTION(curlHandle, CURLOPT_NOPROGRESS, 0);

			this->SetRequestData();
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
				this->SetObject("responseData", Bytes::GlobBytes(this->responseData));

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

