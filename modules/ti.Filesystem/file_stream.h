/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef FILE_STREAM_H_
#define FILE_STREAM_H_

#include <kroll/kroll.h>
#include <string>

#ifdef OS_OSX
#import <Foundation/Foundation.h>
#endif

#include <Poco/FileStream.h>

namespace ti 
{
	enum FileStreamMode
	{
		MODE_READ = 1,
		MODE_APPEND = 2,
		MODE_WRITE = 3
	};


	class FileStream : public StaticBoundObject 
	{
	public:
		FileStream(std::string filename_);
		virtual ~FileStream();

		// TODO - these functions are public for now, but need to be made private when the corresponding functions
		//        in File are deleted
		void Open(const ValueList& args, KValueRef result);
		bool Open(FileStreamMode mode, bool binary = false, bool append = false);
		void Close(const ValueList& args, KValueRef result);
		bool Close();
		void Write(const ValueList& args, KValueRef result);
		void Write(char *,int);
		void Read(const ValueList& args, KValueRef result);
		void ReadLine(const ValueList& args, KValueRef result);
		void WriteLine(const ValueList& args, KValueRef result);
		void Ready(const ValueList& args, KValueRef result);
		void IsOpen(const ValueList& args, KValueRef result);

	private:
		std::string filename;
		Poco::FileIOS* stream;
	};

}

#endif 
