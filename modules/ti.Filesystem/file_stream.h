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
		/**
		 * Function: Open
		 *   Opens this file stream for reading or writing
		 *
		 * Parameters:
		 *   mode - string describing abilities of the file stream (reading/writing/etc)
		 *   binary - boolean to determine if the stream should be opened as binary file
		 *   append - boolean to determine if the stream should be appended(true) or truncated(false)
		 *
		 * Returns:
		 *   true if the file stream is opened successfully; false otherwise
		 */
		void Open(const ValueList& args, KValueRef result);
		bool Open(FileStreamMode mode, bool binary = false, bool append = false);
		/**
		 * Function: Close
		 *   Closes this file stream.  No further reads/writes can be performed on this file stream unless Open is called
		 *
		 * Parameters:
		 *
		 * Returns:
		 *   true if the file stream is closed successfully; false otherwise
		 */
		void Close(const ValueList& args, KValueRef result);
		bool Close();
		/**
		 * Function: Write
		 *   writes a string of text to a file
		 *
		 *   Typical usage is:
		 *     var fileStream = Titanium.Filesystem.getFileStream(filename);
		 *     fileStream.open(Titanium.Filesystem.MODE_READ);
		 *     var stringToWrite = "some text ... ";
		 *     var written = file.write(stringToWrite);
		 *     fileStream.close();
		 *
		 * Parameters:
		 *   text = text to write to file
		 *   append - boolean flag that indicates if the text should be appended to the file (if the file exists)
		 *
		 * Returns:
		 *   true if the text was written successfully; false otherwise
		 */
		void Write(const ValueList& args, KValueRef result);
		void Write(char *,int);
		/**
		 * Function: Read
		 *   Reads the contents of this File
		 *
		 *   Typical usage is:
		 *     var fileStream = Titanium.Filesystem.getFileStream(filename);
		 *     fileStream.open(Titanium.Filesystem.MODE_READ);
		 *     var contents = file.read();
		 *     fileStream.close();
		 *
		 * Parameters:
		 *
		 * Returns:
		 *   the text file contents as a string
		 */
		void Read(const ValueList& args, KValueRef result);
		/**
		 * Function: ReadLine
		 *   Reads the next line of this File.
		 *   The file must be open for reading the first time this method is called.
		 *
		 *   Typical usage is:
		 *     var fileStream = Titanium.Filesystem.getFileStream(filename);
		 *     fileStream.open(Titanium.Filesystem.MODE_READ);
		 *     var line = file.readLine();
		 *     while(line != null) {
		 *       // do something with the line
		 *       line = file.readLine();
		 *     }
		 *     fileStream.close();
		 *
		 * Parameters:
		 *
		 * Returns:
		 *   a string representing the next line of text in the file.
		 */
		void ReadLine(const ValueList& args, KValueRef result);
		
		/**
		 * Function: WriteLine
		 *   Write a line of this File.
		 *   The file must be open for reading the first time this method is called.
		 */
		void WriteLine(const ValueList& args, KValueRef result);

		/**
		 * Function: Ready
		 *   returns true if the file is valid for IO operations
		 */
		void Ready(const ValueList& args, KValueRef result);

		/**
		 * Function: IsOpen
		 *   returns true if the file is open
		 */
		void IsOpen(const ValueList& args, KValueRef result);

	private:
		std::string filename;
		Poco::FileIOS* stream;
	};

}

#endif 
