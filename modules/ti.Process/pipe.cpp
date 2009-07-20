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
		active(true),
		eventsThreadAdapter(0)
	{
		//TODO doc me
		SetMethod("close", &Pipe::_Close);
		SetMethod("write", &Pipe::_Write);
		SetMethod("flush", &Pipe::_Flush);
		SetMethod("attach", &Pipe::_Attach);
		SetMethod("detach", &Pipe::_Detach);
		SetMethod("isAttached", &Pipe::_IsAttached);

		this->eventsThreadAdapter =
			new Poco::RunnableAdapter<Pipe>(*this, &Pipe::FireEvents);
		this->StartEventsThread();
	}

	Pipe::~Pipe()
	{
		this->StopEventsThread();
		delete eventsThreadAdapter;
	}

	void Pipe::StopEventsThread()
	{
		active = false;
		if (eventsThread.isRunning())
			eventsThread.join();
	}

	void Pipe::StartEventsThread()
	{
		this->active = true;
		if (!eventsThread.isRunning())
			this->eventsThread.start(*this->eventsThreadAdapter);
	}

	void Pipe::Attach(SharedKObject object)
	{
		Poco::Mutex::ScopedLock lock(attachedMutex);
		attachedObjects.push_back(object);
	}

	void Pipe::Detach(SharedKObject object)
	{
		Poco::Mutex::ScopedLock lock(attachedMutex);
		std::vector<SharedKObject>::iterator i = attachedObjects.begin();
		while (i != attachedObjects.end())
		{
			SharedKObject obj = *i;
			if (obj->Equals(object))
			{
				i = attachedObjects.erase(i);
			}
			else
			{
				i++;
			}
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
			std::string error = "Target object did not have a write method";
			logger->Error(error);
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
		{ // A Null Blob on the qeueue signals a close event
			Poco::Mutex::ScopedLock lock(buffersMutex);
			buffers.push(0);
		}

		// Call the close method on our attached objects
		{
			Poco::Mutex::ScopedLock lock(attachedMutex);
			for (size_t i = 0; i < attachedObjects.size(); i++)
			{
				this->CallClose(attachedObjects.at(i));
			}
		}
	}

	void Pipe::CallClose(SharedKObject target)
	{
		SharedValue closeMethod = target->Get("close");

		if (!closeMethod->IsMethod())
		{
			//logger->Warn("Target object did not have a close method");
			return;
		}
		else
		{
			try
			{
				closeMethod->ToMethod()->Call(ValueList());
			}
			catch (ValueException &e)
			{
				SharedString ss = e.DisplayString();
				logger->Error("Exception while trying to write to target: %s",
					ss->c_str());
			}
		}
	}

	void Pipe::Flush()
	{
	}

	int Pipe::SafeBuffersSize()
	{
		int size = 0;
		{
			Poco::Mutex::ScopedLock lock(buffersMutex);
			size = buffers.size();
		}
		return size;
	}
	
	void Pipe::FireReadBuffers()
	{
		AutoBlob blob = 0;
		
		while (SafeBuffersSize() > 0)
		{
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
			else
			{
				// A null blob signifies a close event
				this->FireEvent(Event::CLOSE);
				this->FireEvent(Event::CLOSED);
			}
		}

	}
	
	void Pipe::FireEvents()
	{
		while (active || SafeBuffersSize() > 0)
		{
			FireReadBuffers();
			Poco::Thread::sleep(5);
		}
	}
}
