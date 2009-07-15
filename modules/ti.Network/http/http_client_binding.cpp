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
		StaticBoundObject("HTTPClient"),
		host(host),modulePath(path),global(host->GetGlobalObject()),
		thread(NULL),response(NULL),async(true),filestream(NULL),
		timeout(30000),shutdown(false)
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
		 */
		this->SetMethod("open",&HTTPClientBinding::Open);

		/**
		 * @tiapi(method=True,name=Network.HTTPClient.setRequestHeader,since=0.3) Sets a request header for the connection
		 * @tiarg(for=Network.HTTPClient.setRequestHeader,name=header,type=string) request header name
		 * @tiarg(for=Network.HTTPClient.setRequestHeader,name=value,type=string) request header value
		 */
		this->SetMethod("setRequestHeader",&HTTPClientBinding::SetRequestHeader);

		/**
		 * @tiapi(method=True,name=Network.HTTPClient.send,since=0.3) Sends data through the HTTP connection
		 * @tiarg(for=Network.HTTPClient.send,type=string,name=data) data to send
		 */
		this->SetMethod("send",&HTTPClientBinding::Send);

		/**
		 * @tiapi(method=True,name=Network.HTTPClient.sendFile,since=0.3) Sends the contents of a file as body content
		 * @tiarg(for=Network.HTTPClient.sendFile,type=string,name=data) path of file to send
		 */
		this->SetMethod("sendFile",&HTTPClientBinding::SendFile);

		/**
		 * @tiapi(method=True,name=Network.HTTPClient.sendDir,since=0.3) Sends a directory as a zipped body content
		 * @tiarg(for=Network.HTTPClient.sendDir,type=string,name=data) path of directory with contents to send
		 */
		this->SetMethod("sendDir",&HTTPClientBinding::SendDir);

		/**
		 * @tiapi(method=True,name=Network.HTTPClient.getResponseHeader,since=0.3) Returns the value of a response header
		 * @tiarg(for=Network.HTTPClient.getResponseHeader,type=string,name=name) the response header name
		 * @tiresult(for=Network.HTTPClient.getResponseHeader,type=string) the value of the response header
		 */
		this->SetMethod("getResponseHeader",&HTTPClientBinding::GetResponseHeader);

		/**
		 * @tiapi(method=True,name=Network.HTTPClient.setTimeout,since=0.4) Sets the timeout for the request
		 * @tiarg(for=Network.HTTPClient.setTimeout,type=integer,name=timeout) timeout value in milliseconds
		 */
		this->SetMethod("setTimeout",&HTTPClientBinding::SetTimeout);

		/**
		 * @tiapi(property=True,type=integer,name=Network.HTTPClient.readyState,since=0.3) The ready-state status for the connection
		 */
		this->SetInt("readyState",0);

		/**
		 * @tiapi(property=True,type=integer,name=Network.HTTPClient.UNSENT,since=0.3) The UNSENT readyState property
		 */
		this->SetInt("UNSENT",0);

		/**
		 * @tiapi(property=True,type=integer,name=Network.HTTPClient.OPENED,since=0.3)
		 * @tiapi The OPENED readyState property
		 */
		this->SetInt("OPENED",1);

		/**
		 * @tiapi(property=True,type=integer,name=Network.HTTPClient.HEADERS_RECEIVED,since=0.3)
		 * @tiapi The HEADERS_RECEIVED readyState property
		 */
		this->SetInt("HEADERS_RECEIVED",2);

		/**
		 * @tiapi(property=True,type=integer,name=Network.HTTPClient.LOADING,since=0.3)
		 * @tiapi The LOADING readyState property
		 */
		this->SetInt("LOADING",3);

		/**
		 * @tiapi(property=True,type=integer,name=Network.HTTPClient.DONE,since=0.3)
		 * @tiapi The DONE readyState property
		 */
		this->SetInt("DONE",4);

		/**
		 * @tiapi(property=True,type=string,name=Network.HTTPClient.responseText,since=0.3)
		 * @tiapi The response of an HTTP request as text
		 */
		this->SetNull("responseText");

		/**
		 * @tiapi(property=True,type=object,name=Network.HTTPClient.responseXML,since=0.3)
		 * @tiapi The response of an HTTP request as parsable XML
		 */
		this->SetNull("responseXML");

		/**
		 * @tiapi(property=True,type=integer,name=Network.HTTPClient.status,since=0.3)
		 * @tiapi The response status code of an HTTP request
		 */
		this->SetNull("status");

		/**
		 * @tiapi(property=True,type=string,name=Network.HTTPClient.statusText,since=0.3)
		 * @tiapi The response status text of an HTTP Request
		 */
		this->SetNull("statusText");

		/**
		 * @tiapi(property=True,type=integer,name=Network.HTTPClient.connected,since=0.3)
		 * @tiapi Whether an HTTPClient object is connected or not
		 */
		this->SetBool("connected", false);

		/**
		 * @tiapi(property=True,type=method,name=Network.HTTPClient.onreadystatechange,since=0.3)
		 * @tiapi The handler function that will be fired when the ready-state code of an HTTPClient object changes
		 */
		this->SetNull("onreadystatechange");

		/**
		 * @tiapi(property=True,type=method,name=Network.HTTPClient.ondatastream,since=0.3)
		 * @tiapi The handler function that will be fired as stream data is received from an HTTP request
		 */
		this->SetNull("ondatastream");

		/**
		 * @tiapi(property=True,type=method,name=Network.HTTPClient.onsendstream,since=0.3)
		 * @tiapi The handler function that will be fired as the stream data is sent
		 */
		this->SetNull("onsendstream");

		this->self = Value::NewObject(this);
	}
	HTTPClientBinding::~HTTPClientBinding()
	{
		KR_DUMP_LOCATION
		this->shutdown = true;
		if (this->thread!=NULL)
		{
			delete this->thread;
			this->thread = NULL;
		}
		if (this->filestream!=NULL)
		{
			delete this->filestream;
			this->filestream = NULL;
		}
		NetworkBinding::RemoveBinding(this);
	}
	void HTTPClientBinding::Run (void* p)
	{
#ifdef OS_OSX
		NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
#endif
		HTTPClientBinding *binding = reinterpret_cast<HTTPClientBinding*>(p);

		PRINTD("HTTPClientBinding:: starting => " << binding->url);
		
		Poco::Net::HTTPResponse res;
		std::ostringstream ostr;
		int max_redirects = 5;
		int status;
		std::string url = binding->url;

		bool deletefile = false;

		for (int x=0;x<max_redirects;x++)
		{
			Poco::URI uri(url);
			std::string path(uri.getPathAndQuery());
			if (path.empty()) path = "/";
			binding->Set("connected",Value::NewBool(true));
			
			const std::string& scheme = uri.getScheme();
			SharedPtr<Poco::Net::HTTPClientSession> session;
			
			if (scheme=="https")
			{
				if (HTTPClientBinding::initialized==false)
				{
					HTTPClientBinding::initialized = true;
					SharedPtr<Poco::Net::InvalidCertificateHandler> ptrCert = new Poco::Net::AcceptCertificateHandler(false); 
					std::string rootpem = FileUtils::Join(binding->modulePath.c_str(),"rootcert.pem",NULL);
					Poco::Net::Context::Ptr ptrContext = new Poco::Net::Context(Poco::Net::Context::CLIENT_USE,"", "", rootpem, Poco::Net::Context::VERIFY_NONE, 9, false, "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH");
					Poco::Net::SSLManager::instance().initializeClient(0, ptrCert, ptrContext);
				}
				session = new Poco::Net::HTTPSClientSession(uri.getHost(), uri.getPort());
			}
			else if (scheme=="http")
			{
				session = new Poco::Net::HTTPClientSession(uri.getHost(), uri.getPort());
			}
			else
			{
				//FIXME - we need to notify of unsupported error here
			}
			
			
			// set the timeout for the request
			Poco::Timespan to((long)binding->timeout,0L);
			session->setTimeout(to);

			std::string method = binding->method;
			if (method.empty())
			{
				method = Poco::Net::HTTPRequest::HTTP_GET;
			}

			if (!binding->dirstream.empty())
			{
				method = Poco::Net::HTTPRequest::HTTP_POST;
				binding->headers["Content-Type"]="application/zip";
			}

			Poco::Net::HTTPRequest req(method, path, Poco::Net::HTTPMessage::HTTP_1_1);
			const char* ua = binding->global->Get("userAgent")->IsString() ? binding->global->Get("userAgent")->ToString() : NULL;
			PRINTD("HTTPClientBinding:: userAgent = " << ua);
			if (ua)
			{
				req.set("User-Agent",ua);
			}
			else
			{
				// crap, this means we don't have one for some reason -- just fake it
				req.set("User-Agent","Mozilla/5.0 (Macintosh; U; Intel Mac OS X 10_5_6; en-us) AppleWebKit/528.7+ (KHTML, like Gecko) "PRODUCT_NAME"/"STRING(PRODUCT_VERSION));
			}
			//FIXME: implement cookies
			//FIXME: implement username/pass
			//FIXME: use proxy settings of system

			// set the headers
			if (binding->headers.size()>0)
			{
				std::map<std::string,std::string>::iterator i = binding->headers.begin();
				while(i!=binding->headers.end())
				{
					req.set((*i).first, (*i).second);
					i++;
				}
			}

			std::string data;
			int content_len = 0;

			if (!binding->dirstream.empty())
			{
				std::string tmpdir = FileUtils::GetTempDirectory();
				Poco::File tmpPath (tmpdir);
				if (!tmpPath.exists()) {
					tmpPath.createDirectories();
				}
				std::ostringstream tmpfilename;
				tmpfilename << "ti";
				tmpfilename << rand();
				tmpfilename << ".zip";
				std::string fn(FileUtils::Join(tmpdir.c_str(),tmpfilename.str().c_str(),NULL));
				std::ofstream outfile(fn.c_str(), std::ios::binary|std::ios::out|std::ios::trunc);
				Poco::Zip::Compress compressor(outfile,true);
				Poco::Path path(binding->dirstream);
				compressor.addRecursive(path);
				compressor.close();
				outfile.close();
				deletefile = true;
				binding->filename = std::string(fn.c_str());
				binding->filestream = new Poco::FileInputStream(binding->filename);
			}

			if (!binding->datastream.empty())
			{
				data = binding->datastream;
				content_len = data.length();
			}

			// determine the content length
			if (!data.empty())
			{
				std::ostringstream l(std::stringstream::binary|std::stringstream::out);
				l << content_len;
				req.set("Content-Length",l.str());
			}
			else if (!binding->filename.empty())
			{
				Poco::File f(binding->filename);
				std::ostringstream l;
				l << f.getSize();
				const char *cl = l.str().c_str();
				content_len = atoi(cl);
				req.set("Content-Length", l.str());
			}

			// send and stream output
			std::ostream& out = session->sendRequest(req);

			// write out the data
			if (!data.empty())
			{
				out << data;
			}
			else if (binding->filestream)
			{
				// SharedKMethod sender;
				// SharedValue sv = binding->Get("onsendstream");
				// if (sv->IsMethod())
				// {
				// 	sender = sv->ToMethod()->Get("apply")->ToMethod();
				// }
				std::streamsize bufferSize = 8096;
				Poco::Buffer<char> buffer(bufferSize);
				std::streamsize len = 0;
				std::istream& istr = (*binding->filestream);
				istr.read(buffer.begin(), bufferSize);
				std::streamsize n = istr.gcount();
				int remaining = content_len;
				while (n > 0)
				{
					len += n;
					remaining -= n;
					out.write(buffer.begin(), n);
// 					if (sender.get())
// 					{
// 						try
// 						{
// #ifdef DEBUG
// 							std::cout << "ONSENDSTREAM = >> " << len <<" of " << content_len << std::endl;
// #endif
// 							ValueList args;
// 							SharedKList list = new StaticBoundList();
//
// 							args.push_back(binding->self); // reference to us
// 							args.push_back(Value::NewList(list));
//
// 							list->Append(Value::NewInt(len)); // bytes sent
// 							list->Append(Value::NewInt(content_len)); // total size
// 							list->Append(Value::NewInt(remaining)); // remaining
// 							binding->host->InvokeMethodOnMainThread(sender,args,true);
// 						}
// 						catch(std::exception &e)
// 						{
// 							std::cerr << "Caught exception dispatching HTTP callback on transmit, Error: " << e.what() << std::endl;
// 						}
// 						catch(...)
// 						{
// 							std::cerr << "Caught unknown exception dispatching HTTP callback on transmit" << std::endl;
// 						}
//					}
					if (istr)
					{
						istr.read(buffer.begin(), bufferSize);
						n = istr.gcount();
					}
					else n = 0;
				}
				binding->filestream->close();
			}

			std::istream& rs = session->receiveResponse(res);
			int total = res.getContentLength();
			status = res.getStatus();
			PRINTD("HTTPClientBinding:: response length received = " << total << " - " << status << " " << res.getReason());
			binding->Set("status",Value::NewInt(status));
			binding->Set("statusText",Value::NewString(res.getReason().c_str()));

			if (status == 301 || status == 302)
			{
				if (!res.has("Location"))
				{
					break;
				}
				url = res.get("Location");
				PRINTD("redirect to " << url);
				continue;
			}
			SharedValue totalValue = Value::NewInt(total);
			binding->response = &res;
			binding->ChangeState(2); // headers received
			binding->ChangeState(3); // loading

			int count = 0;
			char buf[8096];

			SharedKMethod streamer;
			SharedValue sv = binding->Get("ondatastream");
			if (sv->IsMethod())
			{
				streamer = sv->ToMethod()->Get("apply")->ToMethod();
			}

			while(!rs.eof() && binding->Get("connected")->ToBool())
			{
				try
				{
					rs.read((char*)&buf,8095);
					std::streamsize c = rs.gcount();
					if (c > 0)
					{
						buf[c]='\0';
						count+=c;
						if (streamer.get())
						{
							ValueList args;
							SharedKList list = new StaticBoundList();

							args.push_back(binding->self); // reference to us
							args.push_back(Value::NewList(list));

							list->Append(Value::NewInt(count)); // total count
							list->Append(totalValue); // total size
							list->Append(Value::NewObject(new Blob(buf,c))); // buffer
							list->Append(Value::NewInt(c)); // buffer length

							binding->host->InvokeMethodOnMainThread(streamer,args,binding->shutdown || !binding->async ? false : true);
						}
						else
						{
							ostr << buf;
						}
					}
				}
				catch(std::exception &e)
				{
					Logger* logger = Logger::Get("Network.HTTPClient");
					logger->Error("Caught exception dispatching HTTP callback, Error: %s",e.what());
				}
				catch(...)
				{
					Logger* logger = Logger::Get("Network.HTTPClient");
					logger->Error("Caught unknown exception dispatching HTTP callback");
				}
				if (rs.eof()) break;
			}
			break;
		}
		std::string data = ostr.str();
		if (!data.empty())
		{
#ifdef DEBUG
			if (status > 200)
			{
				PRINTD("RECEIVED = " << data);
			}
#endif
			binding->Set("responseText",Value::NewString(data.c_str()));
		}

		if (deletefile)
		{
			Poco::File f(binding->filename);
			f.remove();
		}

		binding->shutdown = true;
		binding->Set("connected",Value::NewBool(false));
		binding->ChangeState(4); // closed
		binding->response = NULL; // must be done after change state
		NetworkBinding::RemoveBinding(binding);
#ifdef OS_OSX
		[pool release];
#endif
		binding->self = NULL;
	}
	void HTTPClientBinding::Send(const ValueList& args, SharedValue result)
	{
		if (this->Get("connected")->ToBool())
		{
			throw ValueException::FromString("already connected");
		}
		if (args.size()==1)
		{
			// can be a string of data or a file
			SharedValue v = args.at(0);
			if (v->IsObject())
			{
				// probably a file
				SharedKObject obj = v->ToObject();
				this->datastream = obj->Get("toString")->ToMethod()->Call()->ToString();
			}
			else if (v->IsString())
			{
				this->datastream = v->ToString();
			}
			else if (v->IsNull() || v->IsUndefined())
			{
				this->datastream = "";
			}
			else
			{
				throw ValueException::FromString("unknown data type");
			}
		}
		this->thread = new Poco::Thread();
		this->thread->start(&HTTPClientBinding::Run,(void*)this);
		if (!this->async)
		{
			PRINTD("Waiting on HTTP Client thread to finish (sync)");
			Poco::Stopwatch sw;
			sw.start();
			while (!this->shutdown && sw.elapsedSeconds() * 1000 < this->timeout)
			{
				this->thread->tryJoin(100);
			}
			PRINTD("HTTP Client thread finished (sync)");
		}
	}
	void HTTPClientBinding::SendFile(const ValueList& args, SharedValue result)
	{
		if (this->Get("connected")->ToBool())
		{
			throw ValueException::FromString("already connected");
		}
		if (args.size()==1)
		{
			// can be a string of data or a file
			SharedValue v = args.at(0);
			if (v->IsObject())
			{
				// probably a file
				SharedKObject obj = v->ToObject();
				if (obj->Get("isFile")->IsMethod())
				{
					std::string file = obj->Get("toString")->ToMethod()->Call()->ToString();
					this->filestream = new Poco::FileInputStream(file);
					Poco::Path p(file);
					this->filename = p.getFileName();
				}
				else
				{
					this->datastream = obj->Get("toString")->ToMethod()->Call()->ToString();
				}
			}
			else if (v->IsString())
			{
				this->filestream = new Poco::FileInputStream(v->ToString());
			}
			else
			{
				throw ValueException::FromString("unknown data type");
			}
		}
		this->thread = new Poco::Thread();
		this->thread->start(&HTTPClientBinding::Run,(void*)this);
		if (!this->async)
		{
			PRINTD("Waiting on HTTP Client thread to finish");
			this->thread->join();
		}
	}
	void HTTPClientBinding::SendDir(const ValueList& args, SharedValue result)
	{
		if (this->Get("connected")->ToBool())
		{
			throw ValueException::FromString("already connected");
		}
		if (args.size()==1)
		{
			// can be a string of data or a file
			SharedValue v = args.at(0);
			if (v->IsObject())
			{
				// probably a file
				SharedKObject obj = v->ToObject();
				if (obj->Get("isFile")->IsMethod())
				{
					this->dirstream = obj->Get("toString")->ToMethod()->Call()->ToString();
				}
				else
				{
					this->dirstream = obj->Get("toString")->ToMethod()->Call()->ToString();
				}
			}
			else if (v->IsString())
			{
				this->dirstream = v->ToString();
			}
			else
			{
				throw ValueException::FromString("unknown data type");
			}
		}
		this->thread = new Poco::Thread();
		this->thread->start(&HTTPClientBinding::Run,(void*)this);
		if (!this->async)
		{
			PRINTD("Waiting on HTTP Client thread to finish");
			this->thread->join();
		}
	}

	void HTTPClientBinding::Abort(const ValueList& args, SharedValue result)
	{
		this->shutdown=true;
		this->SetBool("connected", false);
	}

	void HTTPClientBinding::Open(const ValueList& args, SharedValue result)
	{
		args.VerifyException("open", "s s ?b s s");

		this->method = args.at(0)->ToString();
		this->url = args.at(1)->ToString();

		if (args.size() >= 3)
		{
			this->async = args.GetBool(2, this->async);
		}

		if (args.size() >= 4)
		{
			this->user = args.GetString(3);
		}

		if (args.size() > 4)
		{
			this->password = args.GetString(4);
		}

		// assign it here (helps prevent deadlock)
		SharedValue v = this->Get("onreadystatechange");
		if (v->IsMethod())
		{
			this->readystate = v->ToMethod();
		}
		SharedValue vc = this->Get("onchange");
		if (vc->IsMethod())
		{
			this->onchange = vc->ToMethod();
		}
		this->ChangeState(1); // opened
	}
	void HTTPClientBinding::SetRequestHeader(const ValueList& args, SharedValue result)
	{
		const char *key = args.at(0)->ToString();
		const char *value = args.at(1)->ToString();
		this->headers[std::string(key)]=std::string(value);
	}
	void HTTPClientBinding::GetResponseHeader(const ValueList& args, SharedValue result)
	{
		if (this->response!=NULL)
		{
			std::string name = args.at(0)->ToString();
			if (this->response->has(name))
			{
				result->SetString(this->response->get(name).c_str());
			}
			else
			{
				result->SetNull();
			}
		}
		else
		{
			throw ValueException::FromString("no available response");
		}
	}
	void HTTPClientBinding::SetTimeout(const ValueList& args, SharedValue result)
	{
		this->timeout = args.at(0)->ToInt();
	}
	void HTTPClientBinding::ChangeState(int readyState)
	{
		static Logger* logger = Logger::Get("Network.HTTPClient");
		logger->Debug("BEFORE CHANGE STATE %d", readyState);
		this->SetInt("readyState",readyState);

		// Don't call onreadystate change callbacks if we are using this
		// symchronously. That would put us into deadlock.
		if (!this->async)
		{
			return;
		}

		if (!readystate.isNull())
		{
			try
			{
				ValueList args;
				args.push_back(this->self);
				SharedKMethod callMethod = this->readystate->Get("call")->ToMethod();
				
				this->host->InvokeMethodOnMainThread(callMethod, args, true);
			}
			catch (std::exception &ex)
			{
				logger->Error("Exception calling readyState. Exception: %s",ex.what());
			}
		}
		if (readyState == 4)
		{
			// onchange listener allows you to just get the final state
			// and is implemented in WebKit XHR
			if (!this->onchange.isNull())
			{
				try
				{
					ValueList args;
					args.push_back(this->self);
					SharedKMethod callMethod = this->onchange->Get("call")->ToMethod();
					this->host->InvokeMethodOnMainThread(callMethod, args, true);
				}
				catch(std::exception &ex)
				{
					logger->Error("Exception calling onchange. Exception: %s",ex.what());
				}
			}
			this->readystate = NULL;
		}
	}
}
