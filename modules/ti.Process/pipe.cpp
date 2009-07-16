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
	Pipe::Pipe(const char *type) : StaticBoundObject(type),
		onRead(0), onClose(0), attachedOutput(0), asyncOnRead(true)
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
		SetMethod("setOnRead", &Pipe::_SetOnRead);
		SetMethod("setOnClose", &Pipe::_SetOnClose);
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
	
	void Pipe::DataReady(AutoPipe pipe)
	{
		if (pipe.isNull()) {
			this->duplicate();
			pipe = this;
		}
		
		/*if (IsAttached())
		{
			if (!(*attachedOutput)->GetMethod("write").isNull())
			{
				AutoPtr<Blob> data = pipe->Read();
				SharedValue result = (*attachedOutput)->CallNS("write", Value::NewObject(data));
			}
		}*/
		//else
		{
			if (onRead != NULL && !onRead->isNull())
			{
				ValueList args;
				SharedKObject event = new StaticBoundObject();
				event->SetObject("pipe", pipe);
				
				args.push_back(Value::NewObject(event));
				try
				{
					logger->Debug("invoke method on main thread, asyncOnRead?%s",asyncOnRead?"true":"false");
					Host::GetInstance()->InvokeMethodOnMainThread(*this->onRead, args, !asyncOnRead);
				}
				catch (ValueException& e)
				{
					logger->Error(e.DisplayString()->c_str());
				}
			}
		}
	}
	
	void Pipe::Closed()
	{
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
	
	void Pipe::SetOnRead(SharedKMethod onReadCallback)
	{
		if (this->onRead && onReadCallback.get() == this->onRead->get()) return;
		
		if (IsAttached())
		{
			Detach();
		}
		
		if (!onReadCallback.isNull())
		{
			this->onRead = new SharedKMethod(onReadCallback);
		}
		else
		{
			this->onRead = NULL;
		}
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
		if (onRead && !onRead->isNull())
		{
			pipe->SetOnRead(*onRead);
		}
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
		
	void Pipe::_SetOnRead(const ValueList& args, SharedValue result)
	{
		if (args.size() >= 0 && args.at(0)->IsMethod())
		{
			SetOnRead(args.at(0)->ToMethod());
		}
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
}
