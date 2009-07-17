/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#include "pipe.h"
#include "native_pipe.h"
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
		closed(false),
		eventsThread(0),
		eventsThreadAdapter(0)
	{
		//TODO doc me
		SetMethod("close", &Pipe::_Close);
		SetMethod("isClosed", &Pipe::_IsClosed);
		SetMethod("write", &Pipe::_Write);
		SetMethod("flush", &Pipe::_Flush);
		SetMethod("attach", &Pipe::_Attach);
		SetMethod("detach", &Pipe::_Detach);
		SetMethod("isAttached", &Pipe::_IsAttached);
		SetMethod("setOnClose", &Pipe::_SetOnClose);

		this->eventsThreadAdapter =
			new Poco::RunnableAdapter<Pipe>(*this, &Pipe::FireEvents);
		this->eventsThread = new Poco::Thread();
		this->eventsThread->start(*this->eventsThreadAdapter);
	}

	Pipe::~Pipe()
	{

	}

	void Pipe::Attach(SharedKObject object)
	{
		Poco::Mutex::ScopedLock lock(attachedMutex);
		attachedObjects.push_back(object);
	}
	
	void Pipe::Detach(SharedKObject object)
	{
		Poco::Mutex::ScopedLock lock(attachedMutex);
		std::vector<SharedKObject>::iterator iter =
			std::find(attachedObjects.begin(), attachedObjects.end(), object);

		if (iter != attachedObjects.end())
		{
			attachedObjects.erase(iter);
		}
	}
	
	bool Pipe::IsAttached()
	{
		Poco::Mutex::ScopedLock lock(attachedMutex);
		
		return attachedObjects.size() > 0;
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
		AutoPipe pipe = new Pipe();
		if (onClose && !onClose->isNull())
		{
			pipe->onClose = new SharedKMethod(*onClose);
		}
		
		return pipe;
	}
	
	void Pipe::_Attach(const ValueList& args, SharedValue result)
	{
		args.VerifyException("attach", "o");
		this->Attach(args.at(0)->ToObject());
	}
	
	void Pipe::_Detach(const ValueList& args, SharedValue result)
	{
		args.VerifyException("detach", "o");
		this->Detach(args.at(0)->ToObject());
	}
	
	void Pipe::_IsAttached(const ValueList& args, SharedValue result)
	{
		result->SetBool(this->IsAttached());
	}
		
	void Pipe::_SetOnClose(const ValueList& args, SharedValue result)
	{
		args.VerifyException("setOnClose", "m");
		this->onClose = new SharedKMethod(args.at(0)->ToMethod());
	}
	
	void Pipe::_Write(const ValueList& args, SharedValue result)
	{
		args.VerifyException("write", "o|s");
		
		AutoBlob blob = new Blob();
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
	
	int Pipe::Write(AutoBlob blob)
	{
		{ // Start the callbacks
			Poco::Mutex::ScopedLock lock(buffersMutex);
			buffers.push(blob);
		}

		// We want this to execute on the same thread and to make all
		// our writeable objects thread safe. This will allow data to
		// flow through pipes more quickly.
		{
			Poco::Mutex::ScopedLock lock(attachedMutex);
			for (size_t i = 0; i < attachedObjects.size(); i++)
			{
				this->CallWrite(attachedObjects.at(i), blob);
			}
		}

		return blob->Length();
	}

	void Pipe::CallWrite(SharedKObject target, AutoBlob blob)
	{
		SharedKMethod writeMethod = target->GetMethod("write");

		if (writeMethod.isNull())
		{
			logger->Error("Target object did not have a write method");
			return;
		}
		else
		{
			try
			{
				writeMethod->Call(ValueList(Value::NewObject(blob)));
			}
			catch (ValueException &e)
			{
				SharedString ss = e.DisplayString();
				logger->Error("Exception while trying to write to target: %s",
					ss->c_str());
			}
		}
	}

	void Pipe::Close()
	{
		Poco::Mutex::ScopedLock lock(attachedMutex);
		if (!closed)
		{
			closed = true;
			eventsThread->join();
			delete eventsThread;
			delete eventsThreadAdapter;
			
			if (onClose && !onClose->isNull())
			{
				ValueList args;
				SharedKObject event = new StaticBoundObject();
				this->duplicate();
				AutoPipe autoThis = this;
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
	
	void Pipe::Closed()
	{
		if (IsAttached())
		{	
			this->duplicate();
			AutoPipe autoThis = this;
			for (size_t i = 0; i < attachedObjects.size(); i++)
			{
				if (attachedObjects.at(i)->GetMethod("closed").isNull())
				{
					SharedKMethod method = attachedObjects.at(i)->GetMethod("close");
					ValueList args;
					SharedKObject event = new StaticBoundObject();
					event->SetObject("pipe", autoThis);
					event->SetObject("attached", attachedObjects.at(i));
					Host::GetInstance()->InvokeMethodOnMainThread(method, args, false);
				}	
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
