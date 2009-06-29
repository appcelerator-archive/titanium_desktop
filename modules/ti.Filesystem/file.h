/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#ifndef _TI_FILE_H_
#define _TI_FILE_H_
#include <kroll/kroll.h>

#ifdef OS_WIN32
#include <windows.h>
#include <commdlg.h>
#include <shellapi.h>
#include <shlobj.h>
#elif OS_OSX
#import <Foundation/Foundation.h>
#endif

#include "file_stream.h" // TODO remove this include when read/write methods are removed from File class
#include <string>

namespace ti
{
	class File : public StaticBoundObject
	{
		public:
			File(std::string filename);
			virtual ~File();
			std::string& GetFilename() { return this->filename; }
			
			virtual SharedString DisplayString(int levels=3) { return new string(GetFilename()); }
			
		private:
			std::string filename;
			ti::FileStream* readLineFS;


			/**
			 * Function: ToString
			 *   Returns the File name
			 *
			 * Parameters:
			 *
			 * Returns:
			 *   the file name
			 */
			void ToString(const ValueList& args, SharedValue result);
			/**
			 * Function: IsFile
			 *   Determines if this File represents a file
			 *
			 * Parameters:
			 *
			 * Returns:
			 *   true if this File repesents a file; false otherwise
			 */
			void IsFile(const ValueList& args, SharedValue result);
			/**
			 * Function: IsDirectory
			 *   Determines if this File represents a directory
			 *
			 * Parameters:
			 *
			 * Returns:
			 *   true if this File represents a directory; false otherwise
			 */
			void IsDirectory(const ValueList& args, SharedValue result);
			/**
			 * Function: IsHidden
			 *   Determines if this File represents a hidden file or directory
			 *
			 * Parameters:
			 *
			 * Returns:
			 *   true if this File is hidden; false otherwise
			 */
			void IsHidden(const ValueList& args, SharedValue result);
			/**
			 * Function: IsSymbolicLink
			 *   Determines if this File represents a symbolic link
			 *
			 * Parameters:
			 *
			 * Returns:
			 *   true if this File is a symbolic link; false otherwise
			 */
			void IsSymbolicLink(const ValueList& args, SharedValue result);
			/**
			 * Function: IsExecutable
			 *   Determines if this File is executable
			 *
			 * Parameters:
			 *
			 * Returns:
			 *   true if this File is executable; false otherwise
			 */
			void IsExecutable(const ValueList& args, SharedValue result);
			/**
			 * Function: IsReadonly
			 *   Determines if this File is readonly or not
			 *
			 * Parameters:
			 *
			 * Returns:
			 *   true if this File is readonly; false otherwise
			 */
			void IsReadonly(const ValueList& args, SharedValue result);
			/**
			 * Function: IsWriteable
			 *   Determines if this File is writeable or not
			 *
			 * Parameters:
			 *
			 * Returns:
			 *   true if this File is writeable; false otherwise
			 */
			void IsWriteable(const ValueList& args, SharedValue result);
			/**
			 * Function: Resolve
			 *   resolves a given relative path against this File
			 *
			 * Parameters:
			 *   path - absolute or relative path to resolve
			 *
			 * Returns:
			 *   a new File object that represents the resolved path
			 */
			void Resolve(const ValueList& args, SharedValue result);
			/**
			 * Function: Write
			 *   @deprecated - should use FileStream object
			 */
			void Write(const ValueList& args, SharedValue result);
			/**
			 * Function: Read
			 *   @deprecated - should use FileStream object
			 */
			void Read(const ValueList& args, SharedValue result);
			/**
			 * Function: ReadLine
			 *   @deprecated - should use FileStream object
			 */
			void ReadLine(const ValueList& args, SharedValue result);
			/**
			 * Function: Copy
			 *   Copies this file or directory to a given destination
			 *
			 *   If this File is a file, the file is copied to the destination specified
			 *   If this File is a directory, the directory contents is recursively copied to the destination
			 *
			 * Parameters:
			 *   destination - the destination where the file or directory is copied to
			 *
			 * Returns:
			 *   true if the copy operation is successful; false otherwise
			 */
			void Copy(const ValueList& args, SharedValue result);
			/**
			 * Function: Move
			 *   Moves this file or directory to the given destination
			 *
			 * Parameters:
			 *   destination - the destination where the file or directory is moved to
			 *
			 * Returns:
			 *   true if the move operation is successful; false otherwise
			 */
			void Move(const ValueList& args, SharedValue result);
			/**
			 * Function: Rename
			 *   Renames the file or directory to the new name
			 *
			 * Parameters:
			 *   newname - the name as a string
			 *
			 * Returns:
			 *   true if the operation is successful; false otherwise
			 */
			void Rename(const ValueList& args, SharedValue result);
			/**
			 * Function: CreateDirectory
			 *   Creates this directory
			 *
			 * Parameters:
			 *   recursive [= false] - controls if all parent directories are created if needed
			 *
			 * Returns:
			 *   true if the directory is created; false otherwise
			 */
			void CreateDirectory(const ValueList& args, SharedValue result);
			/**
			 * Function: DeleteDirectory
			 *   Deletes this directory
			 *
			 * Parameters:
			 *   deleteContents [= false] - controls if the contents inside the directory are also deleted
			 *
			 * Returns:
			 *   true if the directory is deleted; false otherwise
			 */
			void DeleteDirectory(const ValueList& args, SharedValue result);
			/**
			 * Function: DeleteFile
			 *   Deletes this file
			 *
			 * Parameters:
			 *
			 * Returns:
			 *   true if the file is deleted; false otherwise
			 */
			void DeleteFile(const ValueList& args, SharedValue result);
			/**
			 * Function: Equals
			 *
			 * Parameters:
			 *   another file/path
			 * Returns:
			 *   true if the passed in file equals this file
			 */
			void Equals(const ValueList& args, SharedValue result);
			/**
			 * Function: GetDirectoryListing
			 *   Gets a list of files/directories under this directory
			 *
			 * Parameters:
			 *
			 * Returns:
			 *   An array of file/directory names under this directory.
			 */
			void GetDirectoryListing(const ValueList& args, SharedValue result);
			/**
			 * Function: GetParent
			 *   returns the parent for this file or directory
			 *
			 * Parameters:
			 *
			 * Returns:
			 *   the parent for this file
			 */
			void GetParent(const ValueList& args, SharedValue result);
			/**
			 * Function: GetExists
			 *   Determines if this File represents an existing file or directory
			 *
			 * Parameters:
			 *
			 * Returns:
			 *   true if this File exists; false otherwise
			 */
			void GetExists(const ValueList& args, SharedValue result);
			/**
			 * Function: GetCreateTimestamp
			 *   Returns the create date as the number of seconds since midnight January 1, 1970
			 *
			 * Parameters:
			 *
			 * Returns:
			 *   seconds since midnight January 1, 1970; null if create date can't be determined
			 */
			void GetCreateTimestamp(const ValueList& args, SharedValue result);
			/**
			 * Function: GetModificationTimestamp
			 *   Returns the modification date as the number of seconds since midnight January 1, 1970
			 *
			 * Parameters:
			 *
			 * Returns:
			 *   seconds since midnight January 1, 1970; null if modification date can't be determined
			 */
			void GetModificationTimestamp(const ValueList& args, SharedValue result);
			/**
			 * Function: GetName
			 *   Returns the file name
			 *
			 * Parameters:
			 *
			 * Returns:
			 *   the file name
			 */
			void GetName(const ValueList& args, SharedValue result);
			/**
			 * Function: GetExctension
			 *   returns the file extension
			 *
			 * Parameters:
			 *
			 * Returns:
			 *   the file extension; null if this is a directory
			 */
			void GetExtension(const ValueList& args, SharedValue result);
			/**
			 * Function: GetNativePath
			 *   returns the native path for this file or directory
			 *
			 * Parameters:
			 *
			 * Returns:
			 *   the native path
			 */
			void GetNativePath(const ValueList& args, SharedValue result);
			/**
			 * Function: GetSize
			 *   return the size of this file in bytes
			 *
			 * Parameters:
			 *
			 * Returns:
			 *   the file size in bytes
			 */
			void GetSize(const ValueList& args, SharedValue result);
			/**
			 * Function: GetSpaceAvailable
			 *   returns the disk space available in bytes
			 *
			 * Parameters:
			 *
			 * Returns:
			 *   the disk space available in bytes
			 */
			void GetSpaceAvailable(const ValueList& args, SharedValue result);
			/**
			 * Function: CreateShortcut
			 *   creates a filesystem shortcut
			 *
			 * Parameters:
			 *   destination - destination directory
			 *
			 * Returns:
			 *   true if shortcut is created
			 */
			void CreateShortcut(const ValueList& args, SharedValue result);
			/**
			 * Function: SetExecutable
			 *   set executable
			 *
			 * Parameters:
			 *   onoff - true to make executable, false to make it not
			 *
			 * Returns:
			 *   true if succeeded
			 */
			void SetExecutable(const ValueList& args, SharedValue result);
			/**
			 * Function: SetReadonly
			 *   set readonly
			 *
			 * Parameters:
			 *   onoff - true to make readonly, false to make it not
			 *
			 * Returns:
			 *   true if succeeded
			 */
			void SetReadonly(const ValueList& args, SharedValue result);
			/**
			 * Function: SetWriteable
			 *   set writeable
			 *
			 * Parameters:
			 *   onoff - true to make writable, false to make it not
			 *
			 * Returns:
			 *   true if succeeded
			 */
			void SetWriteable(const ValueList& args, SharedValue result);
			/**
			 * Function: Unzip
			 *   unzip this file to destination
			 *
			 * Parameters:
			 *   dest - destination directory to unzip this file
			 *
			 * Returns:
			 *   true if succeeded
			 */
			void Unzip(const ValueList& args, SharedValue result);
	};
}

#endif 
