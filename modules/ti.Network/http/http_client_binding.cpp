/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#ifdef OS_OSX	//For some reason, 10.5 was fine with Cocoa headers being last, but 10.4 balks.
#import <Cocoa/Cocoa.h>
#endif

#include <kroll/kroll.h>
#include "http_client_binding.h"
#include <cstring>
#include <iostream>
#include <sstream>
#include <fstream>
#include "../network_binding.h"
#include <Poco/Buffer.h>
#include <Poco/Net/MultipartWriter.h>
#include <Poco/Net/MessageHeader.h>
#include <Poco/Net/FilePartSource.h>
#include <Poco/File.h>
#include <Poco/Timespan.h>
#include <Poco/Stopwatch.h>
#include <Poco/Net/HTMLForm.h>
#include <Poco/Zip/Compress.h>
#include <Poco/Zip/ZipCommon.h>

#ifdef verify
#define __verify verify
#undef verify
#endif

#include <Poco/Net/HTTPSClientSession.h>
#include <Poco/Net/SSLManager.h>
#include <Poco/Net/KeyConsoleHandler.h>
#include <Poco/Net/AcceptCertificateHandler.h>

#ifdef __verify
#define verify __verify
#undef __verify
#endif

namespace ti
{
	bool HTTPClientBinding::initialized = false;
	
	HTTPClientBinding::HTTPClientBinding(Host* host, std::string path) :
		KEventObject("HTTPClient"),
		host(host),
		modulePath(path),
		global(host->GetGlobalObject()),
		async(true),
		timeout(30000),
		maxRedirects(5),
		bufferSize(8192),
		userAgent(PRODUCT_NAME"/"STRING(PRODUCT_VERSION)),
		thread(0),
		contentLength(0),
		dirty(false)
	{
		/**
		 * @tiapi(method=True,name=Network.HTTPClient.abort,since=0.3) Aborts an in progress connection
		 */
		this->SetMethod("abort",&HTTPClientBinding::Abort);

		/**
		 * @tiapi(method=True,name=Network.HTTPClient.open,since=0.3) Opens an HTTP connection
		 * @tiarg[String, method] The HTTP method to use e.g. POST
		 * @tiarg[String, url] The url to connect to
		 * @tiarg[Boolean, asynchronous, optional=True] Whether or not the request should be asynchronous (default: True)
		 * @tiarg[String, username, optional=True] The HTTP username to use
		 * @tiarg[String, password, optional=True] The HTTP password to use
		 * @tiresult[Boolean] return true if supplied arguments are valid
		 */
		this->SetMethod("open",&HTTPClientBinding::Open);

		/**
		 * @tiapi(method=True,name=Network.HTTPClient.setRequestHeader,since=0.3)
		 * @tiapi Sets a request header for the connection
		 * @tiarg[String,header] request header name
		 * @tiarg[String,value] request header value
		 */
		this->SetMethod("setRequestHeader",&HTTPClientBinding::SetRequestHeader);

		/**
		 * @tiapi(method=True,name=Network.HTTPClient.send,since=0.3) Sends data through the HTTP connection
		 * @tiarg[String,data] data to send
		 * @tiresult[Boolean] returns true if request dispatched successfully
		 */
		this->SetMethod("send",&HTTPClientBinding::Send);

		/**
		 * @tiapi(method=True,name=Network.HTTPClient.sendFile,since=0.3)
		 * @tiapi Sends the contents of a file as body content
		 * @tiarg[Titanium.Filesystem.File,file] the File object to send
		 */
		this->SetMethod("sendFile",&HTTPClientBinding::Send);

		/**
		 * @tiapi(method=True,name=Network.HTTPClient.getResponseHeader,since=0.3) Returns the value of a response header
		 * @tiarg(for=Network.HTTPClient.getResponseHeader,type=String,name=name) the response header name
		 * @tiresult(for=Network.HTTPClient.getResponseHeader,type=String) the value of the response header
		 */
		this->SetMethod("getResponseHeader",&HTTPClientBinding::GetResponseHeader);

		/**
		 * @tiapi(method=True,name=Network.HTTPClient.setTimeout,since=0.4) Sets the timeout for the request
		 * @tiarg(for=Network.HTTPClient.setTimeout,type=Number,name=timeout) timeout value in milliseconds
		 */
		this->SetMethod("setTimeout",&HTTPClientBinding::SetTimeout);

		/**
		 * @tiapi(property=True,type=Number,name=Network.HTTPClient.readyState,since=0.3) The ready-state status for the connection
		 */
		this->SetInt("readyState",0);

		/**
		 * @tiapi(property=True,type=Number,name=Network.HTTPClient.UNSENT,since=0.3) The UNSENT readyState property
		 */
		this->SetInt("UNSENT",0);

		/**
		 * @tiapi(property=True,type=Number,name=Network.HTTPClient.OPENED,since=0.3)
		 * @tiapi The OPENED readyState property
		 */
		this->SetInt("OPENED",1);

		/**
		 * @tiapi(property=True,type=Number,name=Network.HTTPClient.HEADERS_RECEIVED,since=0.3)
		 * @tiapi The HEADERS_RECEIVED readyState property
		 */
		this->SetInt("HEADERS_RECEIVED",2);

		/**
		 * @tiapi(property=True,type=Number,name=Network.HTTPClient.LOADING,since=0.3)
		 * @tiapi The LOADING readyState property
		 */
		this->SetInt("LOADING",3);

		/**
		 * @tiapi(property=True,type=Number,name=Network.HTTPClient.DONE,since=0.3)
		 * @tiapi The DONE readyState property
		 */
		this->SetInt("DONE",4);

		/**
		 * @tiapi(property=True,type=String,name=Network.HTTPClient.responseText,since=0.3)
		 * @tiapi The response of an HTTP request as text
		 */
		this->SetNull("responseText");

		/**
		 * @tiapi(property=True,type=String,name=Network.HTTPClient.responseXML,since=0.3)
		 * @tiapi The response of an HTTP request as parsable XML
		 */
		this->SetNull("responseXML");

		/**
		 * @tiapi(property=True,type=Number,name=Network.HTTPClient.status,since=0.3)
		 * @tiapi The response status code of an HTTP request
		 */
		this->SetNull("status");

		/**
		 * @tiapi(property=True,type=String,name=Network.HTTPClient.statusText,since=0.3)
		 * @tiapi The response status text of an HTTP Request
		 */
		this->SetNull("statusText");

		/**
		 * @tiapi(property=True,type=Boolean,name=Network.HTTPClient.timedOut,since=0.7)
		 * @tiapi True if HTTP request timed out
		 */
		this->SetBool("timedOut", false);

		/**
		 * @tiapi(property=True,type=String,name=Network.HTTPClient.url,since=0.7)
		 * @tiapi The request URL. This value will be updated on redirect events.
		 */
		this->SetNull("url");

		/**
		 * @tiapi(property=True,type=Number,name=Network.HTTPClient.dataSent,since=0.7)
		 * @tiapi Amount of data sent to server so far. Updated on DATASENT event.
		 */
		this->SetInt("dataSent", 0);

		/**
		 * @tiapi(property=True,type=Number,name=Network.HTTPClient.dataReceived,since=0.7)
		 * @tiapi Amount of data received from server so far. Updated on DATARECV event.
		 */
		this->SetInt("dataReceived", 0);

		/**
		 * @tiapi(property=True,type=Boolean,name=Network.HTTPClient.connected,since=0.3)
		 * @tiapi Whether an HTTPClient object is connected or not
		 */
		this->SetBool("connected", false);
	}

	HTTPClientBinding::~HTTPClientBinding()
	{
	}

	void HTTPClientBinding::Abort(const ValueList& args, SharedValue result)
	{
		this->abort.set();
	}

	void HTTPClientBinding::Open(const ValueList& args, SharedValue result)
	{
		args.VerifyException("open", "ss?bss");

		this->method = args.GetString(0);
		this->url = args.GetString(1);
		this->SetString("url", this->url);

		// Validate the scheme
		const std::string scheme = Poco::URI(url).getScheme();
		if (scheme != "http" && scheme != "https")
		{
			Logger::Get("Network.HTTPClient")->Error("%s scheme is not supported", scheme.c_str());
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
			this->user = args.GetString(3);
			this->password = args.GetString(4);
		}

		this->ChangeState(1); // opened
		result->SetBool(true);
	}

	void HTTPClientBinding::Send(const ValueList& args, SharedValue result)
	{
		if (this->Get("connected")->ToBool())
		{
			result->SetBool(false);
			return;
		}

		// Reset internal variables for new request
		if (this->dirty)
		{
			this->Reset();
		}

		if (args.size()==1)
		{
			// Determine what data type we have to send
			SharedValue v = args.at(0);
			if (v->IsObject())
			{
				SharedKObject dataObject = v->ToObject();

				if (dataObject->GetType() == "File")
				{
					// TODO: send file
				}
				else
				{
					// TODO: send object properties
				}
			}
			else if (v->IsString())
			{
				std::string dataString(v->ToString());
				if (!dataString.empty())
				{
					this->datastream = new std::istringstream(dataString,
							std::ios::in | std::ios::binary);
					this->contentLength = dataString.length();
				}
			}
			else if (v->IsNull() || v->IsUndefined())
			{
				// Sending no data
				this->datastream = 0;
			}
			else
			{
				// We do not support this type!
				result->SetBool(false);
				Logger::Get("Network.HTTPClient")->Error("Unsupported datatype: %s", v->GetType().c_str());
				return;
			}
		}

		this->dirty = true;
		this->Set("connected",Value::NewBool(true));

		if (this->async)
		{
			this->thread = new Poco::Thread();
			this->thread->start(*this);
		}
		else
		{
			this->run();
		}

		result->SetBool(true);
	}

	void HTTPClientBinding::SetRequestHeader(const ValueList& args, SharedValue result)
	{
		args.VerifyException("setRequestHeader", "ss");
		std::string key = args.GetString(0);
		std::string value = args.GetString(1);
		this->headers[key]=value;
	}

	void HTTPClientBinding::GetResponseHeader(const ValueList& args, SharedValue result)
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

	void HTTPClientBinding::SetTimeout(const ValueList& args, SharedValue result)
	{
		args.VerifyException("setTimeout", "i");
		this->timeout = args.GetInt(0);
	}

	void HTTPClientBinding::ChangeState(int readyState)
	{
		static Logger* logger = Logger::Get("Network.HTTPClient");
		logger->Debug("BEFORE CHANGE STATE %d", readyState);
		this->SetInt("readyState",readyState);
		this->FireEvent(Event::HTTP_STATECHANGED);
	}

	void HTTPClientBinding::Reset()
	{
		this->thread = 0;
		this->abort.reset();
		this->datastream = 0;
		this->contentLength = 0;
		this->SetBool("timedOut", false);
		this->SetNull("responseText");
		this->SetNull("responseXML");
		this->SetNull("status");
		this->SetNull("statusText");
	}

	void HTTPClientBinding::InitHTTPS()
	{
		// TODO: make this configurable per a binding instance
		HTTPClientBinding::initialized = true;
		SharedPtr<Poco::Net::InvalidCertificateHandler> cert = 
			new Poco::Net::AcceptCertificateHandler(false); 
		std::string rootpem = FileUtils::Join(this->modulePath.c_str(),"rootcert.pem",NULL);
		Poco::Net::Context::Ptr context = new Poco::Net::Context(
			Poco::Net::Context::CLIENT_USE,
			"", "",
			rootpem, 
			Poco::Net::Context::VERIFY_NONE,
			9, false, "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH"
		);
		Poco::Net::SSLManager::instance().initializeClient(0, cert, context);
	}

	void HTTPClientBinding::run()
	{
		// We need this binding to stay alive at least until we have
		// finished this thread.
		this->duplicate();
#ifdef OS_OSX
		NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
#endif

		// Begin the request
		try{
			for (int x = 0; x < this->maxRedirects; x++)
			{			
				// Create HTTP session
				SharedPtr<Poco::Net::HTTPClientSession> session;
				Poco::URI uri(this->url);
				const std::string& scheme = uri.getScheme();
				if (scheme=="https")
				{
					if (HTTPClientBinding::initialized==false)
					{
						this->InitHTTPS();
					}
					session = new Poco::Net::HTTPSClientSession(uri.getHost(), uri.getPort());
				}
				else if (scheme=="http")
				{
					session = new Poco::Net::HTTPClientSession(uri.getHost(), uri.getPort());
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

				// Get path
				std::string path(uri.getPathAndQuery());
				if (path.empty()) 
				{
					path = "/";
				}

				// Prep the request
				Poco::Net::HTTPRequest req(this->method, path, Poco::Net::HTTPMessage::HTTP_1_1);
				req.set("User-Agent", this->userAgent.c_str());

				//FIXME: implement cookies
				//FIXME: implement username/pass

				// Set headers
				if (this->headers.size() > 0)
				{
					std::map<std::string,std::string>::iterator i = this->headers.begin();
					while(i != this->headers.end())
					{
						req.set((*i).first, (*i).second);
						i++;
					}
				}

				// Set content length
				std::ostringstream l(std::stringstream::binary|std::stringstream::out);
				l << this->contentLength;
				req.set("Content-Length",l.str());

				// Allocate buffer
				Poco::Buffer<char> buffer(this->bufferSize);

				// Send request and grab an output stream to send body
				std::ostream& out = session->sendRequest(req);

				// Output request body if we have data to send
				if (this->contentLength > 0)
				{
					int dataSent = 0;
					int readSize = 0;
					while (!this->datastream->eof())
					{
						this->datastream->read(buffer.begin(), buffer.size());
						readSize = this->datastream->gcount();
						if (readSize > 0)
						{
							out.write(buffer.begin(), readSize);
							dataSent += readSize;
							this->SetInt("dataSent", dataSent);
							this->FireEvent(Event::HTTP_DATASENT);
						}
						else
						{
							break;
						}
					}
				}

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
					PRINTD("redirect to " << this->url);
					this->SetString("url", this->url);
					this->FireEvent(Event::HTTP_REDIRECT);
					continue;
				}

				// Set response status code and text
				this->Set("status",Value::NewInt(status));
				this->Set("statusText",Value::NewString(this->response.getReason().c_str()));

				this->ChangeState(2); // headers received
				this->ChangeState(3); // loading

				// Receive data from response
				if (responseLength > 0)
				{
					std::ostringstream ostr(std::ios::out | std::ios::binary);
					bool aborted = false;
					int dataReceived = 0;
					int readSize = 0;
					while(dataReceived < responseLength)
					{
						if (this->abort.tryWait(0))
						{
							aborted = true;
							this->FireEvent(Event::HTTP_ABORT);
							break;
						}

						in.read(buffer.begin(), buffer.size());
						readSize = in.gcount();
						if (readSize > 0)
						{
							ostr.write(buffer.begin(), readSize);
							dataReceived += readSize;
							this->SetInt("dataReceived", dataReceived);
							this->FireEvent(Event::HTTP_DATARECV);
						}
						else
						{
							break;
						}
					}

					if (!aborted)
					{
						// Set response text
						std::string data = ostr.str();
						if (!data.empty())
						{
							this->SetString("responseText", data);
						}
					}
				}

				this->FireEvent(Event::HTTP_DONE);
				break;
			}
		}
		catch(...)
		{
			// Timeout or IO error occurred
			Logger::Get("Network.HTTPClient")->Debug("Timeout occurred");
			this->SetBool("timedOut", true);
			this->FireEvent(Event::HTTP_TIMEOUT);
		}

		this->Set("connected",Value::NewBool(false));
		this->ChangeState(4); // closed

#ifdef OS_OSX
		[pool release];
#endif
		this->release();  // release the binding
	}
}

