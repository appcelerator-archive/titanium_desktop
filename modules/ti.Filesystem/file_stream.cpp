/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#include "file_stream.h"
#include <cstring>
#include <sstream>

#include <Poco/LineEndingConverter.h>

namespace ti 
{
	FileStream::FileStream(std::string filenameIn) :
		StaticBoundObject("Filesystem.FileStream"),
		stream(0)
	{
#ifdef OS_OSX
		// in OSX, we need to expand ~ in paths to their absolute path value
		// we do that with a nifty helper method in NSString
		this->filename = [[[NSString stringWithUTF8String:filenameIn.c_str()]
			 stringByExpandingTildeInPath] fileSystemRepresentation];
#else
		this->filename = filenameIn;
#endif

		/**
		 * @tiapi(method=True,name=Filesystem.Filestream.open,since=0.2) Opens the file
		 * @tiresult(for=Filesystem.Filestream.open,type=Boolean) returns true if successful
		 */
		this->SetMethod("open",&FileStream::Open);
		/**
		 * @tiapi(method=True,name=Filesystem.Filestream.close,since=0.2) Closes the file
		 * @tiresult(for=Filesystem.Filestream.close,type=Boolean) returns true if successful
		 */
		this->SetMethod("close",&FileStream::Close);
		/**
		 * @tiapi(method=True,name=Filesystem.Filestream.read,since=0.2) Reads from the file
		 * @tiresult(for=Filesystem.Filestream.read,type=String) returns data as string
		 */
		this->SetMethod("read",&FileStream::Read);
		/**
		 * @tiapi(method=True,name=Filesystem.Filestream.readLine,since=0.2) Reads one line from the file
		 * @tiresult(for=Filesystem.Filestream.readLine,type=String) returns data as string
		 */
		this->SetMethod("readLine",&FileStream::ReadLine);
		/**
		 * @tiapi(method=True,name=Filesystem.Filestream.write,since=0.2) Writes data into the file
		 * @tiarg(for=Filesystem.Filestream.write,type=String,name=data) data to write
		 * @tiresult(for=Filesystem.Filestream.write,type=Boolean) returns true if successful
		 */
		this->SetMethod("write",&FileStream::Write);
		/**
		 * @tiapi(method=True,name=Filesystem.Filestream.writeLine,since=0.4) Writes a line into the file
		 * @tiarg(for=Filesystem.Filestream.writeLine,type=String,name=data) data to write
		 * @tiresult(for=Filesystem.Filestream.writeLine,type=Boolean) returns true if successful
		 */
		this->SetMethod("writeLine",&FileStream::WriteLine);
		/**
		 * @tiapi(method=True,name=Filesystem.Filestream.ready,since=0.4) Checks to see if file is ready for IO operations
		 * @tiresult(for=Filesystem.Filestream.ready,type=Boolean) returns true if the file open and valid for IO operations
		 */
		this->SetMethod("ready",&FileStream::Ready);
		/**
		 * @tiapi(method=True,name=Filesystem.Filestream.isOpen,since=0.4) Checks to see if file has been opened
		 * @tiresult(for=Filesystem.Filestream.isOpen,type=Boolean) returns true if file has been opened
		 */
		this->SetMethod("isOpen",&FileStream::IsOpen);
		
		/**
		 * @tiapi(property=True,name=Filesystem.Filestream.MODE_READ,since=0.4,type=Number) Mode to indicate read
		 */
		this->Set("MODE_READ",Value::NewInt(MODE_READ));
		/**
		 * @tiapi(property=True,name=Filesystem.Filestream.MODE_APPEND,since=0.4,type=Number) Mode to indicate appending
		 */
		this->Set("MODE_APPEND",Value::NewInt(MODE_APPEND));
		/**
		 * @tiapi(property=True,name=Filesystem.Filestream.MODE_WRITE,since=0.4,type=Number) Mode to indicate writing
		 */
		this->Set("MODE_WRITE",Value::NewInt(MODE_WRITE));
	}

	FileStream::~FileStream()
	{
		this->Close();
	}

	void FileStream::Open(const ValueList& args, KValueRef result)
	{
		FileStreamMode mode = MODE_READ;
		bool binary = false;
		bool append = false;

		if (args.size() >= 1)
			mode = (FileStreamMode) args.at(0)->ToInt();

		if (args.size() >= 2)
				binary = args.at(1)->ToBool();

		if (args.size() >= 3)
			append = args.at(2)->ToBool();

		bool opened = this->Open(mode, binary, append);
		result->SetBool(opened);
	}
	bool FileStream::Open(FileStreamMode mode, bool binary, bool append)
	{
		// close the prev stream if needed
		this->Close();

		try
		{
			std::ios::openmode flags = (std::ios::openmode) 0;
			bool output = false;
			if (binary)
			{
				flags|=std::ios::binary;
			}
			if (mode == MODE_APPEND)
			{
				flags|=std::ios::out|std::ios::app;
				output = true;
			}
			else if (mode == MODE_WRITE)
			{
				flags |= std::ios::out|std::ios::trunc;
				output = true;
			}
			else if (mode == MODE_READ)
			{
				flags |= std::ios::in;
			}
			if (output)
			{
				this->stream = new Poco::FileOutputStream(this->filename,flags);
#ifndef OS_WIN32
				chmod(this->filename.c_str(),S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
#endif		
			}
			else
			{
				this->stream = new Poco::FileInputStream(this->filename,flags);
			}
			return true;
		}
		catch (Poco::Exception& exc)
		{
			Logger* logger = Logger::Get("Filesystem.FileStream");
			logger->Error("Error in open. Exception: %s",exc.displayText().c_str());
			throw ValueException::FromString(exc.displayText());
		}
	}
	void FileStream::Close(const ValueList& args, KValueRef result)
	{
		bool closed = this->Close();
		result->SetBool(closed);
	}
	bool FileStream::Close()
	{
		try
		{
			if (this->stream)
			{
				Poco::FileOutputStream* fos = dynamic_cast<Poco::FileOutputStream*>(this->stream);
				if (fos)
				{
					fos->flush();
				}
				this->stream->close();
				delete this->stream;
				this->stream = NULL;
				return true;
			}
		}
		catch (Poco::Exception& exc)
		{
			Logger* logger = Logger::Get("Filesystem.FileStream");
			logger->Error("Error in close. Exception: %s",exc.displayText().c_str());
			throw ValueException::FromString(exc.displayText());
		}

		return false;
	}
	void FileStream::Write(const ValueList& args, KValueRef result)
	{
		char *text = NULL;
		int size = 0;
		if (args.at(0)->IsObject())
		{
			KObjectRef b = args.at(0)->ToObject();
			AutoPtr<Bytes> bytes = b.cast<Bytes>();
			if (!bytes.isNull())
			{
				text = (char*)bytes->Get();
				size = (int)bytes->Length();
			}
		}
		else if (args.at(0)->IsString())
		{
			text = (char*)args.at(0)->ToString();
		}
		else if (args.at(0)->IsInt())
		{
			std::stringstream ostr;
			ostr << args.at(0)->ToInt();
			text = (char*)ostr.str().c_str();
			size = ostr.str().length();
		}
		else if (args.at(0)->IsDouble())
		{
			std::stringstream ostr;
			ostr << args.at(0)->ToDouble();
			text = (char*)ostr.str().c_str();
			size = ostr.str().length();
		}
		else
		{
			throw ValueException::FromString("Could not write with type passed");
		}

		if (size==0)
		{
			size = strlen(text);
		}

		if (text == NULL)
		{
			result->SetBool(false);
			return;
		}
		if (size <= 0)
		{
			result->SetBool(false);
			return;
		}
		Write(text,size);
		result->SetBool(true);
	}
	void FileStream::Write(char *text, int size)
	{
		try
		{
			Poco::FileOutputStream* fos = dynamic_cast<Poco::FileOutputStream*>(this->stream);
			if(!fos)
			{
				throw ValueException::FromString("FileStream must be opened for writing before calling write");
			}

			fos->write(text, size);
		}
		catch (Poco::Exception& exc)
		{
			Logger* logger = Logger::Get("Filesystem.FileStream");
			logger->Error("Error in write. Exception: %s",exc.displayText().c_str());
			throw ValueException::FromString(exc.displayText());
		}
	}
	void FileStream::Read(const ValueList& args, KValueRef result)
	{
		if (!this->stream)
		{
			Logger* logger = Logger::Get("Filesystem.FileStream");
			logger->Error("Error in read. FileStream must be opened before calling read");
			throw ValueException::FromString("FileStream must be opened before calling read");
		}

		try
		{
			Poco::FileInputStream* fileStream = dynamic_cast<Poco::FileInputStream*>(this->stream);
			if (!fileStream)
			{
				Logger* logger = Logger::Get("Filesystem.FileStream");
				logger->Error("Error in read. FileInputStream is null");
				throw ValueException::FromString("FileStream must be opened for reading before calling read");
			}
			
			std::vector<char> buffer;
			char data[4096];

			while (!fileStream->eof())
			{
				fileStream->read((char*)&data, 4095);
				int length = fileStream->gcount();
				if (length > 0)
				{
					buffer.insert(buffer.end(), data, data+length);
				}
				else break;
			}

			result->SetObject(new Bytes(&(buffer[0]), buffer.size()));
		}
		catch (Poco::Exception& exc)
		{
			Logger* logger = Logger::Get("Filesystem.FileStream");
			logger->Error("Error in read. Exception: %s",exc.displayText().c_str());
			throw ValueException::FromString(exc.displayText());
		}
	}
	void FileStream::ReadLine(const ValueList& args, KValueRef result)
	{
		if (!this->stream)
		{
			Logger* logger = Logger::Get("Filesystem.FileStream");
			logger->Error("Error in readLine. FileStream must be opened before calling read");
			throw ValueException::FromString("FileStream must be opened before calling readLine");
		}

		try
		{
			Poco::FileInputStream* fis = dynamic_cast<Poco::FileInputStream*>(this->stream);
			if (!fis)
			{
				Logger* logger = Logger::Get("Filesystem.FileStream");
				logger->Error("Error in readLine. FileInputStream is null");
				throw ValueException::FromString("FileStream must be opened for reading before calling readLine");
			}

			if (fis->eof())
			{
				// close the file
				result->SetNull();
			}
			else
			{
				std::string line;
				std::getline(*fis, line);
#ifdef OS_WIN32
				// In some cases std::getline leaves a CR on the end of the line in win32 -- why God, why?
				if (!line.empty())
				{
					char lastChar = line.at(line.size()-1);
					if (lastChar == '\r') {
						line = line.substr(0, line.size()-1);
					}
				}
#endif
				if (line.empty() || line.size()==0)
				{
					if (fis->eof())
					{
						// if this is EOF, return null
						result->SetNull();
					}
					else
					{
						// this is an empty line, just empty Bytes object.
						result->SetObject(new Bytes());
					}
				}
				else
				{
					result->SetObject(new Bytes((std::string)line));
				}
			}
		}
		catch (Poco::Exception& exc)
		{
			Logger* logger = Logger::Get("Filesystem.FileStream");
			logger->Error("Error in readLine. Exception: %s",exc.displayText().c_str());
			throw ValueException::FromString(exc.displayText());
		}
	}
	void FileStream::WriteLine(const ValueList& args, KValueRef result)
	{
		if(! this->stream)
		{
			throw ValueException::FromString("FileStream must be opened before calling readLine");
		}
		char *text = NULL;
		int size = 0;
		if (args.at(0)->IsObject())
		{
			KObjectRef b = args.at(0)->ToObject();
			AutoPtr<Bytes> bytes = b.cast<Bytes>();
			if (!bytes.isNull())
			{
				text = (char*)bytes->Get();
				size = (int)bytes->Length();
			}
		}
		else if (args.at(0)->IsString())
		{
			text = (char*)args.at(0)->ToString();
		}
		else if (args.at(0)->IsInt())
		{
			std::stringstream ostr;
			ostr << args.at(0)->ToInt();
			text = (char*)ostr.str().c_str();
			size = ostr.str().length();
		}
		else if (args.at(0)->IsDouble())
		{
			std::stringstream ostr;
			ostr << args.at(0)->ToDouble();
			text = (char*)ostr.str().c_str();
			size = ostr.str().length();
		}
		else
		{
			throw ValueException::FromString("Could not write with type passed");
		}

		if (size==0)
		{
			size = strlen(text);
		}

		if (text == NULL)
		{
			result->SetBool(false);
			return;
		}
		if (size <= 0)
		{
			result->SetBool(false);
			return;
		}
		std::string astr = text;
#ifdef OS_WIN32
		astr += "\r\n";
#else
		astr += "\n";
#endif
		Write((char*)astr.c_str(),astr.length());
		result->SetBool(true);
	}

	void FileStream::Ready(const ValueList& args, KValueRef result)
	{
		Poco::FileIOS* fis = this->stream;
		if(!fis)
		{
			result->SetBool(false);
		}
		else
		{
			result->SetBool(fis->eof()==false);
		}
	}

	void FileStream::IsOpen(const ValueList& args, KValueRef result)
	{
		Poco::FileIOS* fis = this->stream;
		result->SetBool(fis!=NULL);
	}

}
