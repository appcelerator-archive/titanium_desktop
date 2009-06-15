/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _FILESYSTEM_BINDING_H_
#define _FILESYSTEM_BINDING_H_

#include <vector>
#include <kroll/kroll.h>
#include <Poco/Mutex.h>
#include <Poco/Timer.h>

namespace ti
{
	class FilesystemBinding : public StaticBoundObject
	{
	public:
		FilesystemBinding(Host*, SharedKObject);
		virtual ~FilesystemBinding();

	private:
		Host *host;
		SharedKObject global;
		std::vector<SharedKObject> asyncOperations;
		Poco::Timer *timer;

		void OnAsyncOperationTimer(Poco::Timer &timer);

		/**
		 * Function: CreateTempFile
		 *   creates a new temporary file
		 *
		 * Params:
		 *
		 * Returns:
		 *   the filename for the new temporary file
		 */
		void CreateTempFile(const ValueList& args, SharedValue result);
		/**
		 * Function: CreateTempDirectory
		 *   creates a new temporary directory
		 *
		 * Params:
		 *
		 * Returns:
		 *   the directory name for the new temporary directory
		 */
		void CreateTempDirectory(const ValueList& args, SharedValue result);
		/**
		 * Function: GetFile
		 *   creates a new File object for a given filename
		 *
		 * Params:
		 *   filename - the name of the file to create the File object for
		 *
		 * Returns:
		 *   a new File object
		 */
		void GetFile(const ValueList& args, SharedValue result);
		/**
		 * Function: GetFileStream
		 *   creates a new FileStream object for a given filename
		 *
		 * Params:
		 *   filename - the name of the file to create the FileStream object for
		 *
		 * Returns:
		 *   a new FileStream object
		 */
		void GetFileStream(const ValueList& args, SharedValue result);
		/**
		 * Function: GetApplicationDirectory
		 *   returns a path to the application directory
		 *
		 * Params:
		 *
		 * Returns:
		 *   path to the Titanium Application folder
		 */
		void GetApplicationDirectory(const ValueList& args, SharedValue result);
		/**
		 * Function: GetApplicationDataDirectory
		 *   returns a path to the applications data directory
		 *
		 * Params:
		 *
		 * Returns:
		 *   path to the Titanium Data folder
		 */
		void GetApplicationDataDirectory(const ValueList& args, SharedValue result);
		/**
		 * Function: GetRuntimeHomeDirectory
		 *   returns a path to the runtime base directory
		 *
		 * Params:
		 *
		 * Returns:
		 *   path to the Titanium runtime base folder
		 */
		void GetRuntimeHomeDirectory(const ValueList& args, SharedValue result);
		/**
		 * Function: GetResourcesDirectory
		 *   returns a path to the application's Resource directory
		 *
		 * Params:
		 *
		 * Returns:
		 *   path to the Resources folder
		 */
		void GetResourcesDirectory(const ValueList& args, SharedValue result);

		/**
		 * Function: GetProgramsDirectory
		 *   return the path to the user's programs directory in a platform specific way
		 *
		 * Params:
		 *
		 * Returns:
		 *   path to the user's programs folder; null if the directory cannot be determined
		 */
		void GetProgramsDirectory(const ValueList &args, SharedValue result);
		/**
		 * Function: GetDesktopDirectory
		 *   return the path to the user's desktop directory
		 *
		 * Params:
		 *
		 * Returns:
		 *   path to the user's Desktop folder; null if the desktop directory cannot be determined
		 */
		void GetDesktopDirectory(const ValueList& args, SharedValue result);
		/**
		 * Function: GetDocumentsDirectory
		 *   return the path to the user's documents directory
		 *
		 * Params:
		 *
		 * Returns:
		 *   path to the user's document's folder; null if the documents directory cannot be determined
		 */
		void GetDocumentsDirectory(const ValueList& args, SharedValue result);
		/**
		 * Function: GetUserDirectory
		 *   return the path to the user's home directory
		 *
		 * Params:
		 *
		 * Returns:
		 *   path to the user's home directory; null if the user directory cannot be determined
		 */
		void GetUserDirectory(const ValueList& args, SharedValue result);
		/**
		 * Function: GetLineEnding
		 *   return a string that represents the line ending
		 *
		 * Params:
		 *
		 * Returns:
		 *  line ending as a string
		 */
		void GetLineEnding(const ValueList& args, SharedValue result);
		/**
		 * Function: GetSeparator
		 *   gets the separator used in file paths
		 *
		 * Params:
		 *
		 * Returns:
		 *   separator used in file paths
		 */
		void GetSeparator(const ValueList& args, SharedValue result);
		/**
		 * Function: GetRootDirectories
		 *   gets a list of strings that represent the root directories
		 *
		 * Params:
		 *
		 * Returns:
		 *   a list of strings
		 */
		void GetRootDirectories(const ValueList& args, SharedValue result);

		/**
		 * Function: ExecuteAsyncCopy
		 *   performs an asynchoronous copy operation, making a callback
		 *
		 * Params:
	 	 *   file(s) - the name of the file (as string) or an array of files
	     *   callback - function to invoke when each copy operation is completed
		 *
		 * Returns:
		 *   an async copy object
		 */
		void ExecuteAsyncCopy(const ValueList& args, SharedValue result);


		//INTERNAL ONLY
		void DeletePendingOperations(const ValueList& args, SharedValue result);
		void ResolveFileName(const ValueList& args, std::string& filename);
	};
}

#endif
