/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#include "pipe.h"
#include <vector>
#include <cstring>

namespace ti
{
	Pipe::Pipe(Poco::PipeIOS* pipe) : StaticBoundObject("Pipe"), pipe(pipe), closed(false)
	{
		/**
		 * @tiapi(property=True,type=boolean,name=Process.Pipe.closed,since=0.2) Whether or not a pipe is closed
		 */
		this->Set("closed",Value::NewBool(false));
		/**
		 * @tiapi(method=True,name=Process.Pipe.close,since=0.2) Closes the pipe
		 */
		this->SetMethod("close",&Pipe::Close);
		/**
		 * @tiapi(method=True,name=Process.Pipe.write,since=0.2) Writes data to the pipe
		 * @tiarg(for=Process.Pipe.write,type=string,name=data) data to write
		 */
		this->SetMethod("write",&Pipe::Write);
		/**
		 * @tiapi(method=True,name=Process.Pipe.read,since=0.2) Reads data from the pipe
		 * @tiresult(for=Process.Pipe.read,type=string) result read from pipe
		 */
		this->SetMethod("read",&Pipe::Read);
	}
	Pipe::~Pipe()
	{
		Close();
		delete pipe;
		pipe = NULL;
	}
	void Pipe::Write(const ValueList& args, SharedValue result)
	{
		if (closed)
		{
			throw ValueException::FromString("Pipe is already closed");
		}
		if (!args.at(0)->IsString())
		{
			throw ValueException::FromString("Can only write string data");
		}
		Poco::PipeOutputStream *os = dynamic_cast<Poco::PipeOutputStream*>(pipe);
		if (os==NULL)
		{
			throw ValueException::FromString("Stream is not writeable");
		}
		const char *data = args.at(0)->ToString();
		int len = (int)strlen(data);
		try
		{
			os->write(data,len);
			result->SetInt(len);
		}
		catch (Poco::Exception &e)
		{
			throw ValueException::FromString(e.what());
		}
	}
	void Pipe::Read(const ValueList& args, SharedValue result)
	{
		if (closed)
		{
			throw ValueException::FromString("Pipe is already closed");
		}
		Poco::PipeInputStream *is = dynamic_cast<Poco::PipeInputStream*>(pipe);
		if (is==NULL)
		{
			throw ValueException::FromString("Stream is not readable");
		}
		char *buf = NULL;
		try
		{
			int size = 128;
			// allow the size of the returned buffer to be 
			// set by the caller - defaults to 1024 if not specified
			if (args.size() > 0 && args.at(0)->IsInt())
			{
				size = args.at(0)->ToInt();
			}
			buf = new char[size+1];
			is->read(buf,size);
			int count = is->gcount();
			if (count <= 0)
			{
				result->SetNull();
			}
			else
			{
				buf[count] = '\0';
				result->SetString(buf);
			}
			delete [] buf;
		}
		catch (Poco::Exception &e)
		{
			if (buf) delete[] buf;
			Logger::Get("Process.Pipe")->Error(e.what());
			throw ValueException::FromString(e.what());
		}
	}
	void Pipe::Close(const ValueList& args, SharedValue result)
	{
		Close();
	}
	void Pipe::Close()
	{
		if (!closed)
		{
			pipe->close();
			closed=true;
			this->Set("closed",Value::NewBool(true));
		}
	}
}
