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
	Poco::Mutex Pipe::eventsMutex;
	std::queue<AutoPtr<Event> > Pipe::events;
	Poco::ThreadTarget Pipe::eventsThreadTarget(&Pipe::FireEvents);
	Poco::Thread Pipe::eventsThread;

	Pipe::Pipe(const char *type) :
		KEventObject(type),
		logger(Logger::Get("Process.Pipe"))
	{
		/**
		 * @tiapi(method=True,name=Process.Pipe.close,since=0.5)
		 * @tiapi Close this pipe to further reading/writing.
		 */
		SetMethod("close", &Pipe::_Close);
		
		/**
		 * @tiapi(method=True,name=Process.Pipe.write,since=0.5)
		 * @tiapi Write data to this pipe
		 * @tiarg[Blob|String, data] a Blob object or String to write to this pipe
		 * @tiresult[Number, bytesWritten] the number of bytes actually written on this pipe
		 */
		SetMethod("write", &Pipe::_Write);
		
		/**
		 * @tiapi(method=True,name=Process.Pipe.attach,since=0.5)
		 * @tiapi Attach an IO object to this pipe. An IO object is an object that
		 * @tiapi implements a public "write(Blob)". In Titanium, this include FileStreams, and Pipes.
		 * @tiapi You may also use your own custom IO implementation here.
		 */
		SetMethod("attach", &Pipe::_Attach);
		
		/**
		 * @tiapi(method=True,name=Process.Pipe.detach,since=0.5)
		 * @tiapi Detach an IO object from this pipe. see Process.Pipe.attach.
		 */
		SetMethod("detach", &Pipe::_Detach);
		
		/**
		 * @tiapi(method=True,name=Process.Pipe.isAttached,since=0.5)
		 * @tiresult[bool, isAttached] returns whether or not this pipe is attached to 1 or more IO objects
		 */
		SetMethod("isAttached", &Pipe::_IsAttached);

		if (!eventsThread.isRunning())
		{
			eventsThread.start(eventsThreadTarget);
		}
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
			this->duplicate();
			AutoPtr<Event> event = new ReadEvent(this, blob);
			Poco::Mutex::ScopedLock lock(eventsMutex);
			events.push(event);
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
		{
			Poco::Mutex::ScopedLock lock(eventsMutex);
			this->duplicate();
			this->duplicate();
			AutoPtr<Event> closeEvent = new Event(this, Event::CLOSE);
			AutoPtr<Event> closedEvent = new Event(this, Event::CLOSED);

			events.push(closeEvent);
			events.push(closedEvent);
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
			logger->Warn("Target object did not have a close method");
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

	void Pipe::FireEvents()
	{
		while (true)
		{
			AutoPtr<Event> event = 0;
			{
				Poco::Mutex::ScopedLock lock(eventsMutex);
				if (events.size() > 0)
				{
					event = events.front();
					events.pop();
				}
			}

			if (!event.isNull())
			{
				event->target->FireEvent(event);
			}

			Poco::Thread::sleep(5);
		}
	}
}
