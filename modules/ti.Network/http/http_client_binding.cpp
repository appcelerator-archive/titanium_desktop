/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#include "http_client_binding.h"
#include <kroll/thread_manager.h>
#include <sstream>

namespace ti
{
	kroll::Logger* HTTPClientBinding::logger = 0;

	HTTPClientBinding::HTTPClientBinding(Host* host, std::string path) :
		KEventObject("Network.HTTPClient"),
		host(host),
		modulePath(path),
		buffer(8192),
		async(true),
		timeout(30000),
		thread(0),
		contentLength(0),
		dirty(false)
	{
		/**
		 * @tiapi(method=True, name=Network.HTTPClient.abort, since=0.3)
		 * @tiapi Aborts an in progress connection
		 */
		this->SetMethod("abort", &HTTPClientBinding::Abort);

		/**
		 * @tiapi(method=True, name=Network.HTTPClient.open, since=0.3) Opens an HTTP connection
		 * @tiarg[String, method] The HTTP method to use e.g. POST
		 * @tiarg[String, url] The url to connect to
		 * @tiarg[Boolean, asynchronous, optional=True] Whether or not the request should be asynchronous (default: True)
		 * @tiarg[String, username, optional=True] The HTTP username to use
		 * @tiarg[String, password, optional=True] The HTTP password to use
		 * @tiresult[Boolean] return true if supplied arguments are valid
		 */
		this->SetMethod("open", &HTTPClientBinding::Open);

		/**
		 * @tiapi(method=True, name=Network.HTTPClient.setBasicCredentials, since=0.7)
		 * @tiapi Set the basic authentication credentials
		 * @tiarg[String, username] username
 		 * @tiarg[String, password] password
		 */
		this->SetMethod("setBasicCredentials", &HTTPClientBinding::SetBasicCredentials);

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
		 * @tiarg[Object, data, optional=True] data to send
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
		 * @tiarg[Object, handler] A handler to receive the response data. Can either be Titanium.Filesystem.File or a method.
		 * @tiarg[Object, data, optional=True] data to send
		 * @tiresult[Boolean] returns true if request dispatched successfully
		 */
		this->SetMethod("receive", &HTTPClientBinding::Receive);

		/**
		 * @tiapi(method=True, name=Network.HTTPClient.getResponseHeader, since=0.3)
		 * @tiapi Returns the value of a response header
		 * @tiarg[String, name] the response header name
		 * @tiresult[String] the value of the response header
		 */
		this->SetMethod("getResponseHeader",&HTTPClientBinding::GetResponseHeader);

		/**
		 * @tiapi(method=True, name=Network.HTTPClient.setCookie, since=0.7)
		 * @tiapi Set a HTTP cookie in the request.
		 * @tiarg[String, name] the cookie name
		 * @tiarg[String, value] the cookie value
		 */
		this->SetMethod("setCookie",&HTTPClientBinding::SetCookie);

		/**
		 * @tiapi(method=True, name=Network.HTTPClient.clearCookies, since=0.7)
		 * @tiapi Clear any cookies set on the request
		 */
		this->SetMethod("clearCookies",&HTTPClientBinding::ClearCookies);

		/**
		 * @tiapi(method=True, name=Network.HTTPClient.getCookie, since=0.7)
		 * @tiapi Get a HTTP cookie from the response.
		 * @tiarg[String, name] name of the cookie to get
		 * @tiresult[Network.HTTPCookie] a cookie or null if not found
		 */
		this->SetMethod("getCookie",&HTTPClientBinding::GetCookie);

		/**
		 * @tiapi(method=True, name=Network.HTTPClient.setTimeout, since=0.4) Sets the timeout for the request
		 * @tiarg[Number, timeout] timeout value in milliseconds
		 */
		this->SetMethod("setTimeout",&HTTPClientBinding::SetTimeout);

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
		 * @tiapi The handler function that will be fired when the ready-state code of an HTTPClient object changes.
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
		 * @tiapi(property=True, name=Network.HTTPClient.maxRedirects, since=0.7)
		 * @tiapi Maxium number of redirects to follow. (Default: 5)
		 */
		this->SetInt("maxRedirects", 5);

		/**
		 * @tiapi(property=True, name=Network.HTTPClient.userAgent, since=0.7)
		 * @tiapi User agent string to use for requests. (Default: PRODUCTNAME/PRODUCT_VERSION)
		 */
		this->SetString("userAgent", PRODUCT_NAME"/"PRODUCT_VERSION);

		if (!HTTPClientBinding::logger)
			HTTPClientBinding::logger = Logger::Get(this->type);
	}

	HTTPClientBinding::~HTTPClientBinding()
	{
	}

	void HTTPClientBinding::Abort(const ValueList& args, KValueRef result)
	{
		this->abort.set();
	}

	void HTTPClientBinding::Open(const ValueList& args, KValueRef result)
	{
		args.VerifyException("open", "ss?bss");

		this->method = args.GetString(0);
		this->url = args.GetString(1);
		this->SetString("url", this->url);

		// Validate the scheme
		const std::string scheme = Poco::URI(url).getScheme();
		if (scheme != "http" && scheme != "https")
		{
			logger->Error("%s scheme is not supported", scheme.c_str());
			result->SetBool(false);
			return;
		}

		if (this->method.empty())
		{
			this->method = Poco::Net::HTTPRequest::HTTP_GET;
		}

		if (args.size() >= 3)
		{
			this->async = args.GetBool(2);
		}

		if (args.size() >= 4)
		{
			this->basicCredentials.setUsername(args.GetString(3));
			this->basicCredentials.setPassword(args.GetString(4));
		}

		// Get on*** handler functions
		this->ondatastream = this->GetMethod("ondatastream");
		this->onreadystate = this->GetMethod("onreadystatechange");
		this->onsendstream = this->GetMethod("onsendstream");
		this->onload = this->GetMethod("onload");

		this->ChangeState(1); // opened
		result->SetBool(true);
	}

	void HTTPClientBinding::SetBasicCredentials(const ValueList& args, KValueRef result)
	{
		args.VerifyException("setBasicCredentials", "ss");
		this->basicCredentials.setUsername(args.GetString(0));
		this->basicCredentials.setPassword(args.GetString(1));
	}

	void HTTPClientBinding::Send(const ValueList& args, KValueRef result)
	{
		// Get send data if provided
		args.VerifyException("send", "?s|o|0");
		KValueRef sendData(args.GetValue(0));

		// Setup output stream for data
		this->outstream = new std::ostringstream(std::ios::binary | std::ios::out);
		result->SetBool(this->BeginRequest(sendData));
	}

	void HTTPClientBinding::Receive(const ValueList& args, KValueRef result)
	{
		args.VerifyException("receive", "m|o ?s|o|0");

		// Set output handler
		this->outstream = 0;
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
				logger->Error("Unsupported object type as output handler:"
					" does not have write method");
			}
			else
			{
				this->outputHandler = writeMethod;
			}
		}
		else
		{
			logger->Error("Invalid type as output handler!");
			return;
		}

		// Get the send data if provided
		KValueRef sendData(args.GetValue(1));
		result->SetBool(this->BeginRequest(sendData));
	}

	void HTTPClientBinding::SetRequestHeader(const ValueList& args, KValueRef result)
	{
		args.VerifyException("setRequestHeader", "ss");
		std::string key = args.GetString(0);
		std::string value = args.GetString(1);
		this->headers[key] = value;
	}

	void HTTPClientBinding::GetResponseHeader(const ValueList& args, KValueRef result)
	{
		args.VerifyException("getResponseHeader", "s");
		std::string name = args.GetString(0);

		if (this->response.has(name))
		{
			result->SetString(this->response.get(name).c_str());
		}
		else
		{
			result->SetNull();
		}
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
			KObjectRef cookie = new HTTPCookie(this->responseCookies[cookieName]);
			result->SetObject(cookie);
		}
		else
		{
			// No cookie found with that name.
			result->SetNull();
		}
	}

	void HTTPClientBinding::SetTimeout(const ValueList& args, KValueRef result)
	{
		args.VerifyException("setTimeout", "i");
		this->timeout = args.GetInt(0);
	}

	bool HTTPClientBinding::FireEvent(std::string& eventName)
	{
		// We're already exposed as an AutoPtr somewhere else, so we must create
		// an AutoPtr version of ourselves with the 'shared' argument set to true.
		ValueList args(Value::NewObject(GetAutoPtr()));

		// Must invoke the on*** handler functions
		if (eventName == Event::HTTP_STATE_CHANGED && !this->onreadystate.isNull())
		{
			this->host->InvokeMethodOnMainThread(this->onreadystate, args, true);

			if (this->Get("readyState")->ToInt() == 4 && !this->onload.isNull())
			{
				this->host->InvokeMethodOnMainThread(this->onload, args, true);
			}
		}
		else if (eventName == Event::HTTP_DATA_SENT && !this->onsendstream.isNull())
		{
			this->host->InvokeMethodOnMainThread(this->onsendstream, args, true);
		}
		else if (eventName == Event::HTTP_DATA_RECEIVED && !this->ondatastream.isNull())
		{
			this->host->InvokeMethodOnMainThread(this->ondatastream, args, true);
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

	bool HTTPClientBinding::BeginRequest(KValueRef sendData)
	{
		if (this->Get("connected")->ToBool())
		{
			return false;
		}

		// Reset internal variables for new request
		if (this->dirty)
		{
			this->Reset();
		}

		// Determine what data type we have to send
		if (sendData->IsObject())
		{
			KObjectRef dataObject = sendData->ToObject();
			KMethodRef nativePathMethod(dataObject->GetMethod("nativePath"));
			KMethodRef sizeMethod(dataObject->GetMethod("size"));

			if (nativePathMethod.isNull() || sizeMethod.isNull())
			{
				std::string err("Unsupported File-like object: did not have"
					"nativePath and size methods");
				logger->Error(err);
				throw ValueException::FromString(err);
			}

			const char* filename = nativePathMethod->Call()->ToString();
			if (!filename)
			{
				std::string err("Unsupported File-like object: nativePath method"
					"did not return a String");
				logger->Error(err);
				throw ValueException::FromString(err);
			}

			this->datastream = new std::ifstream(filename,
				std::ios::in | std::ios::binary);
			if (this->datastream->fail())
			{
				std::string err("Failed to open file: ");
				err.append(filename);
				logger->Error(err);
				throw ValueException::FromString(err);
			}

			this->contentLength = sizeMethod->Call()->ToInt();
		}
		else if (sendData->IsString())
		{
			std::string dataString(sendData->ToString());
			if (!dataString.empty())
			{
				this->datastream = new std::istringstream(dataString,
						std::ios::in | std::ios::binary);
				this->contentLength = dataString.length();
			}
		}
		else
		{
			// Sending no data
			this->datastream = 0;
		}

		this->dirty = true;
		this->Set("connected", Value::NewBool(true));

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
		logger->Debug("BEFORE CHANGE STATE %d", readyState);
		this->SetInt("readyState", readyState);
		this->FireEvent(Event::HTTP_STATE_CHANGED);
	}

	void HTTPClientBinding::Reset()
	{
		this->thread = 0;
		this->abort.reset();
		this->datastream = 0;
		this->outstream = 0;
		this->contentLength = 0;
		this->SetBool("timedOut", false);
		this->SetNull("responseText");
		this->SetNull("responseXML");
		this->SetNull("status");
		this->SetNull("statusText");
	}

	void HTTPClientBinding::InitHTTPS()
	{
		static bool initialized = false;
		if (initialized)
			return;

		SharedPtr<Poco::Net::InvalidCertificateHandler> cert =
			new Poco::Net::AcceptCertificateHandler(false);
		std::string rootpem = FileUtils::Join(this->modulePath.c_str(),"rootcert.pem", NULL);
		Poco::Net::Context::Ptr context = new Poco::Net::Context(
			Poco::Net::Context::CLIENT_USE,
			"", "",
			rootpem,
			Poco::Net::Context::VERIFY_NONE,
			9, false, "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH"
		);
		Poco::Net::SSLManager::instance().initializeClient(0, cert, context);
		initialized = true;
	}

	void HTTPClientBinding::PrepareSession(Poco::URI uri)
	{
		// Create the session
		const std::string& scheme = uri.getScheme();
		if (scheme=="https")
		{
			this->InitHTTPS();
			this->session = new Poco::Net::HTTPSClientSession(
					uri.getHost(), uri.getPort());
		}
		else if (scheme=="http")
		{
			this->session = new Poco::Net::HTTPClientSession(
					uri.getHost(), uri.getPort());
		}

		// Setup proxy settings
		std::string uriString = uri.toString();
		SharedPtr<kroll::Proxy> proxy = kroll::ProxyConfig::GetProxyForURL(uriString);
		if (!proxy.isNull())
		{
			session->setProxyHost(proxy->info->getHost());
			session->setProxyPort(proxy->info->getPort());
		}

		// Set timeout
		Poco::Timespan to(0L, (long)this->timeout * 1000);
		session->setTimeout(to);
	}

	void HTTPClientBinding::SendRequestBody(std::ostream& out)
	{
		int dataSent = 0;
		int readSize = 0;

		while (!this->datastream->eof())
		{
			this->datastream->read(this->buffer.begin(), this->buffer.size());
			readSize = this->datastream->gcount();
			if (readSize <= 0)
				break;

			out.write(this->buffer.begin(), readSize);
			dataSent += readSize;
			this->SetInt("dataSent", dataSent);
			this->FireEvent(Event::HTTP_DATA_SENT);
		}
	}

	void HTTPClientBinding::ReceiveResponseBody(std::istream& in, int responseLength)
	{
		int dataReceived = 0;
		int readSize = 0;

		while (dataReceived < responseLength)
		{
			if (this->abort.tryWait(0))
			{
				this->FireEvent(Event::HTTP_ABORT);
				return;
			}

			in.read(buffer.begin(), buffer.size());
			readSize = in.gcount();
			if (readSize <= 0)
				break;

			if (this->outstream)
			{
				this->outstream->write(buffer.begin(), readSize);
			}
			else
			{
				// Pass data to handler on main thread
				std::string data(buffer.begin(), readSize);
				this->host->InvokeMethodOnMainThread(
					this->outputHandler,
					ValueList(Value::NewString(data))
				);
			}

			dataReceived += readSize;
			this->SetInt("dataReceived", dataReceived);
			this->FireEvent(Event::HTTP_DATA_RECEIVED);
		}

		// Set response text if no handler is set
		if (this->outstream)
		{
			std::string data = this->outstream->str();
			if (!data.empty())
			{
				this->SetString("responseText", data);
			}
		}
	}

	void HTTPClientBinding::GetCookies()
	{
		this->responseCookies.clear();
		try
		{
			std::vector<Poco::Net::HTTPCookie> cookies;
			this->response.getCookies(cookies);
			std::vector<Poco::Net::HTTPCookie>::iterator i;
			for (i = cookies.begin(); i != cookies.end(); i++)
			{
				Poco::Net::HTTPCookie& cookie = *i;
				this->responseCookies[cookie.getName()] = cookie;
			}
		}
		catch (Poco::Exception& e)
		{
			// Probably a bad Set-Cookie header
			logger->Error("Failed to read cookies");
		}
	}

	void HTTPClientBinding::ExecuteRequest()
	{
		// Begin the request
		try
		{
			for (int x = 0; x < this->GetInt("maxRedirects"); x++)
			{
				Poco::URI uri(this->url);

				// Prepare the HTTP session
				this->PrepareSession(uri);

				// Get request path
				std::string path(uri.getPathAndQuery());
				if (path.empty())
					path = "/";

				// Prepare the request
				Poco::Net::HTTPRequest request(this->method, path, Poco::Net::HTTPMessage::HTTP_1_1);
				request.set("User-Agent", this->GetString("userAgent").c_str());

				// Set cookies
				if (!this->requestCookies.empty())
					request.setCookies(this->requestCookies);

				// Apply basic authentication credentials
				basicCredentials.authenticate(request);

				// Set headers
				if (this->headers.size() > 0)
				{
					std::map<std::string, std::string>::iterator i = this->headers.begin();
					while (i != this->headers.end())
					{
						if (i->first.empty() || i->second.empty())
							continue;
						request.set(i->first, i->second);
						i++;
					}
				}

				// Set content length
				// FIXME: we should almost have a standard int -> string conversion
				// method that is re-useable else where in the code base.
				std::ostringstream contentLengthStr(std::ios::binary | std::ios::out);
				contentLengthStr << this->contentLength;
				request.set("Content-Length", contentLengthStr.str());

				// Send request and grab an output stream to send body
				std::ostream& out = session->sendRequest(request);

				// Output request body if we have data to send
				if (this->contentLength > 0)
					this->SendRequestBody(out);

				// Get the response
				std::istream& in = session->receiveResponse(this->response);
				int status = this->response.getStatus();
				int responseLength = this->response.getContentLength();

				// Handle redirects
				if (status == 301 || status == 302)
				{
					if (!this->response.has("Location"))
					{
						break;
					}
					this->url = this->response.get("Location");
					logger->Debug("redirect to %s", this->url.c_str());
					this->SetString("url", this->url);
					this->FireEvent(Event::HTTP_REDIRECT);
					continue;
				}

				// Get cookies from response
				this->GetCookies();

				// Set response status code and text
				this->Set("status", Value::NewInt(status));
				this->Set("statusText", Value::NewString(this->response.getReason().c_str()));

				this->ChangeState(2); // headers received
				this->ChangeState(3); // loading

				// Receive data from response
				if (responseLength > 0)
					this->ReceiveResponseBody(in, responseLength);

				this->FireEvent(Event::HTTP_DONE);
				break;
			}
		}
		catch (...)
		{
			// Timeout or IO error occurred
			logger->Debug("Timeout occurred");
			this->SetBool("timedOut", true);
			this->FireEvent(Event::HTTP_TIMEOUT);
		}

		// Destroy the session here, so that even if this HTTPClient isn't
		// garbage collected, the socket will close.
		this->session = 0;

		this->Set("connected", Value::NewBool(false));
		this->ChangeState(4); // closed
	}
}

