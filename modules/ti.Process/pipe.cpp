/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#include "pipe.h"
#include <vector>
#include <cstring>

#if defined(OS_WIN32)
# include "win32/win32_pipe.h"
#else
# include "posix/posix_pipe.h"
#endif

namespace ti
{
	Pipe::Pipe(const char *type) :
		KEventObject(type),
		onClose(0),
		attachedOutput(0),
		asyncOnRead(true),
		closed(false),
		eventsThread(0),
		eventsThreadAdapter(0)
	{
		//TODO doc me
		SetMethod("close", &Pipe::_Close);
		SetMethod("isClosed", &Pipe::_IsClosed);
		SetMethod("write", &Pipe::_Write);
		SetMethod("flush", &Pipe::_Flush);
		SetMethod("read", &Pipe::_Read);
		SetMethod("readLine", &Pipe::_ReadLine);
		SetMethod("attach", &Pipe::_Attach);
		SetMethod("detach", &Pipe::_Detach);
		SetMethod("isAttached", &Pipe::_IsAttached);
		SetMethod("setOnClose", &Pipe::_SetOnClose);

		this->eventsThreadAdapter =
			new Poco::RunnableAdapter<Pipe>(*this, &Pipe::FireEvents);
		this->eventsThread = new Poco::Thread();
		this->eventsThread->start(*this->eventsThreadAdapter);
	}

	/*static*/
	AutoPipe Pipe::CreatePipe()
	{
#if defined(OS_WIN32)
		AutoPipe pipe = new Win32Pipe();
#else
		AutoPipe pipe = new PosixPipe();
#endif
		return pipe;
	}

	Pipe::~Pipe()
	{

	}

	void Pipe::DataReady(AutoPipe pipe)
	{
		// if (pipe.isNull()) {
		// 	this->duplicate();
		// 	pipe = this;
		// }
		// 
		// /*if (IsAttached())
		// {
		// 	if (!(*attachedOutput)->GetMethod("write").isNull())
		// 	{
		// 		AutoPtr<Blob> data = pipe->Read();
		// 		SharedValue result = (*attachedOutput)->CallNS("write", Value::NewObject(data));
		// 	}
		// }*/
		// //else
		// {
		// 	if (onRead != NULL && !onRead->isNull())
		// 	{
		// 		ValueList args;
		// 		SharedKObject event = new StaticBoundObject();
		// 		event->SetObject("pipe", pipe);
		// 		
		// 		args.push_back(Value::NewObject(event));
		// 		try
		// 		{
		// 			logger->Debug("invoke method on main thread, asyncOnRead?%s",asyncOnRead?"true":"false");
		// 			Host::GetInstance()->InvokeMethodOnMainThread(*this->onRead, args, !asyncOnRead);
		// 		}
		// 		catch (ValueException& e)
		// 		{
		// 			logger->Error(e.DisplayString()->c_str());
		// 		}
		// 	}
		// }
	}
	
	void Pipe::Closed()
	{
	}
	
	void Pipe::Attach(SharedKObject other)
	{
		attachedOutput = new SharedKObject(other);
		Logger::Get("Process.Pipe")->Debug("attaching object.. hasWrite? %s, hasClose? %s",
			!(*attachedOutput)->Get("write")->IsUndefined()?"TRUE":"FALSE",
			!(*attachedOutput)->Get("close")->IsUndefined()?"TRUE":"FALSE");
	}
	
	void Pipe::Detach()
	{
		if (attachedOutput && !attachedOutput->isNull())
		{
			delete attachedOutput;
		}
	}
	
	bool Pipe::IsAttached()
	{
		return attachedOutput != NULL && !attachedOutput->isNull();
	}
	
	int Pipe::FindFirstLineFeed(char *data, int length, int *charsToErase)
	{
		int newline = -1;
		for (int i = 0; i < length; i++)
		{
			if (data[i] == '\n')
			{
				newline = i;
				*charsToErase = 1;
				break;
			}
			else if (data[i] == '\r')
			{
				if (i < length-1 && data[i+1] == '\n')
				{
					newline = i+1;
					*charsToErase = 2;
					break;
				}
			}
		}
		
		return newline;
	}
	
	AutoPipe Pipe::Clone()
	{
		AutoPipe pipe = CreatePipe();
		if (onClose && !onClose->isNull())
		{
			pipe->onClose = new SharedKMethod(*onClose);
		}
		
		return pipe;
	}
	
	void Pipe::_Read(const ValueList& args, SharedValue result)
	{
		int bufsize = -1;
		if (args.size() > 0)
		{
			bufsize = args.at(0)->ToInt();
		}
		
		AutoPtr<Blob> blob = Read(bufsize);
		result->SetObject(blob);
	}
	
	void Pipe::_ReadLine(const ValueList& args, SharedValue result)
	{
		AutoPtr<Blob> blob = ReadLine();
		if (blob.isNull())
		{
			result->SetNull();
		}
		else
		{
			result->SetObject(blob);
		}
	}
	
	void Pipe::_Attach(const ValueList& args, SharedValue result)
	{
		if (args.size() >= 0 && args.at(0)->IsObject())
		{
			this->Attach(args.at(0)->ToObject());
		}
	}
	
	void Pipe::_Detach(const ValueList& args, SharedValue result)
	{
		this->Detach();
	}
	
	void Pipe::_IsAttached(const ValueList& args, SharedValue result)
	{
		result->SetBool(this->IsAttached());
	}
		
	void Pipe::_SetOnClose(const ValueList& args, SharedValue result)
	{
		if (args.size() >= 0 && args.at(0)->IsMethod())
		{
			this->onClose = new SharedKMethod(args.at(0)->ToMethod());
		}
	}
	
	void Pipe::_Write(const ValueList& args, SharedValue result)
	{
		if (args.size() == 0)
		{
			throw ValueException::FromString("No data passed to write");
		}
		AutoPtr<Blob> blob = new Blob();
		if (args.at(0)->IsObject())
		{
			blob = args.at(0)->ToObject().cast<Blob>();
		}
		else if (args.at(0)->IsString())
		{
			blob = new Blob(args.at(0)->ToString());
		}
		
		if (blob.isNull())
		{
			throw ValueException::FromString("Pipe.write argument should be a Blob or string");
		}
		
		int written = this->Write(blob);
		result->SetInt(written);
		
	}
	
	void Pipe::_Flush(const ValueList& args, SharedValue result)
	{
		this->Flush();
	}
	

	void Pipe::_Close(const ValueList& args, SharedValue result)
	{
		this->Close();
	}
	
	void Pipe::_IsClosed(const ValueList& args, SharedValue result)
	{
		result->SetBool(this->IsClosed());
	}

	AutoPtr<Blob> Pipe::Read(int bufsize)
	{
		// I don't think we need this any longer...
		// if (!closed)
		// {
		// 	if (buffer.size() == 0)
		// 	{
		// 		return new Blob();
		// 	}
		// 	
		// 	if (bufsize == -1 || bufsize > (int) buffer.size())
		// 	{
		// 		bufsize = (int) buffer.size();
		// 	}	
		// 	
		// 	mutex.lock();
		// 	AutoPtr<Blob> blob = new Blob(&(buffer[0]), bufsize);
		// 	buffer.erase(buffer.begin(), buffer.begin()+bufsize);
		// 	mutex.unlock();
		// 	
		// 	return blob;
		// }
		// throw ValueException::FromString("This pipe is closed.");
		return new Blob();
	}
	
	AutoPtr<Blob> Pipe::ReadLine()
	{
		// I don't think we need this any longer -- everything is asynchronous
		// Poco::Mutex::ScopedLock lock(mutex);
		// if (!closed)
		// {
		// 	if (buffer.size() == 0) return NULL;
		// 	
		// 	int charsToErase;
		// 	int newline = FindFirstLineFeed(&(buffer[0]), buffer.size(), &charsToErase);
		// 	if (newline == -1) return NULL;
		// 	
		// 	AutoPtr<Blob> blob = new Blob(&(buffer[0]), newline-charsToErase+1);
		// 	buffer.erase(buffer.begin(), buffer.begin()+newline+1);
		// 	
		// 	return blob;
		// }
		// throw ValueException::FromString("This pipe is closed.");
		return new Blob();
	}
	
	int Pipe::GetSize()
	{
		return 0;
		// Poco::Mutex::ScopedLock lock(mutex);
		// 
		// if (!closed)
		// {
		// 	return buffer.size();
		// }
		// else throw ValueException::FromString("This pipe is closed.");
	}
	
	const char* Pipe::GetBuffer()
	{
		Poco::Mutex::ScopedLock lock(mutex);
		// return &buffer[0];
		return " ";
	}

	void Pipe::Write(char *data, int length)
	{
		//Poco::Mutex::ScopedLock lock(mutex);
	}
	
	int Pipe::Write(AutoPtr<Blob> blob)
	{
		{ // Start the callbacks
			Poco::Mutex::ScopedLock lock(buffersMutex);
			buffers.push(blob);
		}

		// For every attached pipe, write this data to it
		// for (each attached pipe)
		// {
		// 		pipe->Write(blob);
		// 		or even
		// 		pipe->Call("write", Value::NewObject(blob));
		// }

		return blob->Length();
	}

	void Pipe::Close()
	{
		Poco::Mutex::ScopedLock lock(mutex);
		if (!closed)
		{
			closed = true;
			eventsThread->join();
			delete eventsThread;
			delete eventsThreadAdapter;

			//Logger::Get("Process.Pipe")->Debug("in Pipe::Closed");
			if (IsAttached())
			{
				//Logger::Get("Process.Pipe")->Debug("I'm attached");
				if (!(*attachedOutput)->GetMethod("close").isNull())
				{
					SharedKMethod method = (*attachedOutput)->GetMethod("close");
					ValueList args;
					Host::GetInstance()->InvokeMethodOnMainThread(method, args, false);
				}
			}
			
			if (onClose && !onClose->isNull())
			{
				ValueList args;
				SharedKObject event = new StaticBoundObject();
				this->duplicate();
				AutoPtr<Pipe> autoThis = this;
				event->SetObject("pipe", autoThis);
				
				args.push_back(Value::NewObject(event));
				
				try
				{
					Host::GetInstance()->InvokeMethodOnMainThread(*this->onClose, args, false);
				}
				catch (ValueException& e)
				{
					logger->Error(e.DisplayString()->c_str());
				}
				delete onClose;
			}
		}
	}
	
	bool Pipe::IsClosed()
	{
		return closed;
	}

	void Pipe::Flush()
	{
	}

	void Pipe::FireEvents()
	{
		AutoBlob blob = 0;
		while (!closed || buffers.size() > 0)
		{
			if (buffers.size() > 0)
			{
				Poco::Mutex::ScopedLock lock(buffersMutex);
				blob = buffers.front();
				buffers.pop();
			}

			if (!blob.isNull())
			{
				this->duplicate();
				AutoPtr<KEventObject> autothis = this;
				AutoPtr<Event> event = new ReadEvent(autothis, blob);
				this->FireEvent(event);
				blob = 0;
			}
		}
	}
}
