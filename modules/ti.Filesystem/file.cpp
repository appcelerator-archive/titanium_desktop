/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#include "file.h"
#include "filesystem_utils.h"

#include <Poco/File.h>
#include <Poco/Path.h>
#include <Poco/FileStream.h>
#include <Poco/Exception.h>

#ifndef OS_WIN32
#include <sys/types.h>
#include <sys/stat.h>
#endif

#ifdef OS_LINUX
#include <sys/statvfs.h>
#endif

#ifdef OS_WIN32
#define MIN_PATH_LENGTH 3
#else
#define MIN_PATH_LENGTH 1
#endif


namespace ti
{
	File::File(std::string filename) :
		StaticBoundObject("Filesystem.File"),
		readLineFS(0)
	{

		Poco::Path pocoPath(Poco::Path::expand(filename));
		this->filename = pocoPath.absolute().toString();

		// If the filename we were given contains a trailing slash, just remove it
		// so that users can count on reproducible results from toString.
		size_t length = this->filename.length();
		if (length > MIN_PATH_LENGTH && this->filename[length - 1] == Poco::Path::separator())
		{
			this->filename.resize(length - 1);
		}

		/**
		 * @tiapi(method=True,name=Filesystem.File.toString,since=0.2) Returns a string representation of the File object
		 * @tiresult(for=Filesystem.File.toString,type=String) the string representation of the File object
		 */
		this->SetMethod("toString",&File::ToString);
		/**
		 * @tiapi(method=True,name=Filesystem.File.toString,since=0.7)
		 * @tiapi Return a string representation of the File object's URL
		 * @tiresult[String] A string containing the file:// URL to this file.
		 */
		this->SetMethod("toURL", &File::ToURL);
		/**
		 * @tiapi(method=True,name=Filesystem.File.isFile,since=0.2) Checks whether a File object references a file
		 * @tiresult(for=Filesystem.File.isFile,type=Boolean) true if the File object references a file, false if otherwise
		 */
		this->SetMethod("isFile",&File::IsFile);
		/**
		 * @tiapi(method=True,name=Filesystem.File.isDirectory,since=0.2) Checks whether a File object references a directory
		 * @tiresult(for=Filesystem.File.isDirectory,type=Boolean) true if the File object references a directory, false if otherwise
		 */
		this->SetMethod("isDirectory",&File::IsDirectory);
		/**
		 * @tiapi(method=True,name=Filesystem.File.isHidden,since=0.2) Checks whether a file or directory is hidden
		 * @tiresult(for=Filesystem.File.isHidden,type=Boolean) true if the file or directory is hidden, false if otherwise
		 */
		this->SetMethod("isHidden",&File::IsHidden);
		/**
		 * @tiapi(method=True,name=Filesystem.File.isSymbolicLink,since=0.2) Checks whether the File object references a symbolic link
		 * @tiresult(for=Filesystem.File.isSymbolicLink,type=Boolean) true if the File object references a symbolic link, false if otherwise
		 */
		this->SetMethod("isSymbolicLink",&File::IsSymbolicLink);
		/**
		 * @tiapi(method=True,name=Filesystem.File.isExecutable,since=0.2) Checks whether a file is an executable file
		 * @tiresult(for=Filesystem.File.isExecutable,type=Boolean) true if the file is an executable file, false if otherwise
		 */
		this->SetMethod("isExecutable",&File::IsExecutable);
		/**
		 * @tiapi(method=True,name=Filesystem.File.isReadonly,since=0.2) Checks whether a file or directory is read-only
		 * @tiresult(for=Filesystem.File.isReadonly,type=Boolean) true if the file or directory is read-only, false if otherwise
		 */
		this->SetMethod("isReadonly",&File::IsReadonly);
		/**
		 * @tiapi(method=True,name=Filesystem.File.isWriteable,since=0.2) Checks whether a file or directory is writeable
		 * @tiresult(for=Filesystem.File.isWriteable,type=Boolean) true if the file or directory is writeable, false if otherwise
		 */
		this->SetMethod("isWriteable",&File::IsWriteable);
		/**
		 * @tiapi(method=True,name=Filesystem.File.resolve,since=0.2) Resolves a File object to a file path
		 * @tiarg(for=Filesystem.File.resolve,name=path) path to resolve
		 * @tiresult(for=Filesystem.File.resolve,type=Filesystem.File) a File object referencing the path
		 */
		this->SetMethod("resolve",&File::Resolve);
		/**
		 * @tiapi(method=True,name=Filesystem.File.write,since=0.2) Writes data to the file
		 * @tiarg(for=Filesystem.File.write,type=String|Blob,name=data) data to write
		 */
		this->SetMethod("write",&File::Write);
		/**
		 * @tiapi(method=True,name=Filesystem.File.read,since=0.2) Reads a file and returns the content as a string
		 * @tiresult(for=Filesystem.File.read,type=Blob) The contents of the file as a blob
		 */
		this->SetMethod("read",&File::Read);
		/**
		 * @tiapi(method=True,name=Filesystem.File.readLine,since=0.2) Returns one line (separated by line ending) from a file
		 * @tiresult(for=Filesystem.File.readLine,type=String) a string of data from the file
		 */
		this->SetMethod("readLine",&File::ReadLine);
		/**
		 * @tiapi(method=True,name=Filesystem.File.copy,since=0.2) Copies a file to a specified location
		 * @tiarg(for=Filesystem.File.copy,type=String,name=destination) destination to copy to
		 * @tiresult(for=Filesystem.File.copy,type=Boolean) true if the file was successfully copied, false if otherwise
		 */
		this->SetMethod("copy",&File::Copy);
		/**
		 * @tiapi(method=True,name=Filesystem.File.move,since=0.2) Moves a file to a specified location
		 * @tiarg(for=Filesystem.File.move,type=String,name=destination) destination to move to
		 * @tiresult(for=Filesystem.File.move,type=Boolean) true if the file was successfully moved, false if otherwise
		 */
		this->SetMethod("move",&File::Move);
		/**
		 * @tiapi(method=True,name=Filesystem.File.rename,since=0.2) Renames a file
		 * @tiarg(for=Filesystem.File.rename,type=String,name=destination) destination to rename to
		 * @tiresult(for=Filesystem.File.rename,type=Boolean) true if the file was successfully renamed, false if otherwise
		 */
		this->SetMethod("rename",&File::Rename);
		/**
		 * @tiapi(method=True,name=Filesystem.File.createDirectory,since=0.2) Creates a new directory
		 * @tiresult(for=Filesystem.File.createDirectory,type=Boolean) true if the directory was succesfully created, false if otherwise
		 */
		this->SetMethod("createDirectory",&File::CreateDirectory);
		/**
		 * @tiapi(method=True,name=Filesystem.File.deleteDirectory,since=0.2) Deletes a directory
		 * @tiresult(for=Filesystem.File.deleteDirectory,type=Boolean) true if the directory was successfully deleted, false if otherwise
		 */
		this->SetMethod("deleteDirectory",&File::DeleteDirectory);
		/**
		 * @tiapi(method=True,name=Filesystem.File.deleteFile,since=0.2) Deletes a file
		 * @tiresult(for=Filesystem.File.deleteFile,type=Boolean) true if the file was successfully deleted, false if otherwise
		 */
		this->SetMethod("deleteFile",&File::DeleteFile);
		/**
		 * @tiapi(method=True,returns=Array<Filesystem.File>,name=Filesystem.File.getDirectoryListing,since=0.2) Returns a list containing the names of items inside a directory.
		 * @tiresult(for=Filesystem.File.getDirectoryListing,type=Array<Filesystem.File>) an array of files inside the directory
		 */
		this->SetMethod("getDirectoryListing",&File::GetDirectoryListing);
		/**
		 * @tiapi(method=True,returns=Filesystem.File,name=Filesystem.File.parent,since=0.2) Returns the parent directory of a file or directory
		 * @tiresult(for=Filesystem.File.parent,type=Filesystem.File) the parent directory of the file or directory
		 */
		this->SetMethod("parent",&File::GetParent);
		/**
		 * @tiapi(method=True,name=Filesystem.File.exists,since=0.2) Checks whether a file or directory exists in the users system
		 * @tiresult(for=Filesystem.File.exists,type=Boolean) returns true if the file or directory exists, false if otherwise
		 */
		this->SetMethod("exists",&File::GetExists);
		/**
		 * @tiapi(method=True,name=Filesystem.File.createTimestamp,since=0.2) Returns the created timestamp of a file or directory
		 * @tiresult(for=Filesystem.File.createTimestamp,type=Number) the created timestamp of the file or directory
		 */
		this->SetMethod("createTimestamp",&File::GetCreateTimestamp);
		/**
		 * @tiapi(method=True,name=Filesystem.File.modificationTimestamp,since=0.2) Returns the modification timestamp of a file or directory
		 * @tiresult(for=Filesystem.File.modificationTimestamp,type=Number) the modification timestamp of the a file or directory
		 */
		this->SetMethod("modificationTimestamp",&File::GetModificationTimestamp);
		/**
		 * @tiapi(method=True,name=Filesystem.File.name,since=0.2) Returns the name of a file or directory
		 * @tiresult(for=Filesystem.File.name,type=String) the name of the file or directory
		 */
		this->SetMethod("name",&File::GetName);
		/**
		 * @tiapi(method=True,name=Filesystem.File.extension,since=0.2) Returns the extension of a file
		 * @tiresult(for=Filesystem.File.extension,type=String) extension of the file
		 */
		this->SetMethod("extension",&File::GetExtension);
		/**
		 * @tiapi(method=True,name=Filesystem.File.nativePath,since=0.2) Returns the full native path of a file or directory
		 * @tiresult(for=Filesystem.File.nativePath,type=String) full native path of the file or directory
		 */
		this->SetMethod("nativePath",&File::GetNativePath);
		/**
		 * @tiapi(method=True,name=Filesystem.File.size,since=0.2) Returns the size of the file in bytes
		 * @tiresult(for=Filesystem.File.size,type=Number) file size of a file or directory in bytes
		 */
		this->SetMethod("size",&File::GetSize);
		/**
		 * @tiapi(method=True,name=Filesystem.File.spaceAvailable,since=0.2) Returns the space available on the filesystem
		 * @tiresult(for=Filesystem.File.spaceAvailable,type=Number) the space available on the filesystem
		 */
		this->SetMethod("spaceAvailable",&File::GetSpaceAvailable);
		/**
		 * @tiapi(method=True,name=Filesystem.File.createShortcut,since=0.2) Creates a shortcut to a file or directory
		 * @tiarg(for=Filesystem.File.createShortcut,name=destination) the destination path where the shortcut will be created
		 * @tiresult(for=Filesystem.File.createShortcut,type=Boolean) returns true if the shortcut was successfully created, false if otherwise
		 */
		this->SetMethod("createShortcut",&File::CreateShortcut);
		/**
		 * @tiapi(method=True,name=Filesystem.File.setExecutable,since=0.2) Makes the file or directory executable
		 * @tiresult(for=Filesystem.File.setExecutable,type=Boolean) returns true if successful
		 */
		this->SetMethod("setExecutable",&File::SetExecutable);
		/**
		 * @tiapi(method=True,name=Filesystem.File.setReadonly,since=0.2) Makes the file or directory readonly
		 * @tiresult(for=Filesystem.File.setReadonly,type=Boolean) returns true if successful
		 */
		this->SetMethod("setReadonly",&File::SetReadonly);
		/**
		 * @tiapi(method=True,name=Filesystem.File.setWriteable,since=0.2) Makes the file or directory writeable
		 * @tiresult(for=Filesystem.File.setWriteable,type=Boolean) returns true if successful
		 */
		this->SetMethod("setWriteable",&File::SetWriteable);
		/**
		 * @tiapi(method=True,name=Filesystem.File.unzip,since=0.3)
		 * @tiapi If this file is s zip file, unzip it into the given destination directory.
		 * @tiarg[Filesystem.File|String,destination] Directory to unzip the file to.
		 * @tiresult[Boolean] True if the operation was succesful, false otherwise.
		 */
		this->SetMethod("unzip",&File::Unzip);
	}

	File::~File()
	{
		if (this->readLineFS)
			delete this->readLineFS;
	}

	void File::ToURL(const ValueList& args, KValueRef result)
	{
		std::string url(URLUtils::PathToFileURL(this->filename));
		result->SetString(url.c_str());
	}

	void File::ToString(const ValueList& args, KValueRef result)
	{
		Logger::Get("File")->Debug("ToString: %s", filename.c_str());
		result->SetString(this->filename.c_str());
	}

	void File::IsFile(const ValueList& args, KValueRef result)
	{
		try
		{
			Poco::File file(this->filename);
			bool isFile = file.isFile();
			result->SetBool(isFile);
		}
		catch (Poco::FileNotFoundException &fnf)
		{
			result->SetBool(false);
		}
		catch (Poco::PathNotFoundException &fnf)
		{
			result->SetBool(false);
		}
		catch (Poco::Exception& exc)
		{
			throw ValueException::FromString(exc.displayText());
		}
	}
	void File::IsDirectory(const ValueList& args, KValueRef result)
	{
		try
		{
			Poco::File dir(this->filename);
			bool isDir = dir.isDirectory();
			result->SetBool(isDir);
		}
		catch (Poco::FileNotFoundException &fnf)
		{
			result->SetBool(false);
		}
		catch (Poco::PathNotFoundException &fnf)
		{
			result->SetBool(false);
		}
		catch (Poco::Exception& exc)
		{
			throw ValueException::FromString(exc.displayText());
		}
	}
	void File::IsHidden(const ValueList& args, KValueRef result)
	{
		try
		{
			Poco::File file(this->filename);
			bool isHidden = file.isHidden();
			result->SetBool(isHidden);
		}
		catch (Poco::FileNotFoundException &fnf)
		{
			result->SetBool(false);
		}
		catch (Poco::PathNotFoundException &fnf)
		{
			result->SetBool(false);
		}
		catch (Poco::Exception& exc)
		{
			throw ValueException::FromString(exc.displayText());
		}
	}
	void File::IsSymbolicLink(const ValueList& args, KValueRef result)
	{
		try
		{
			Poco::File file(this->filename);
			bool isLink = file.isLink();
			result->SetBool(isLink);
		}
		catch (Poco::FileNotFoundException &fnf)
		{
			result->SetBool(false);
		}
		catch (Poco::PathNotFoundException &fnf)
		{
			result->SetBool(false);
		}
		catch (Poco::Exception& exc)
		{
			throw ValueException::FromString(exc.displayText());
		}
	}
	void File::IsExecutable(const ValueList& args, KValueRef result)
	{
		try
		{
			Poco::File file(this->filename);
			result->SetBool(file.canExecute());
		}
		catch (Poco::FileNotFoundException &fnf)
		{
			result->SetBool(false);
		}
		catch (Poco::PathNotFoundException &fnf)
		{
			result->SetBool(false);
		}
		catch (Poco::Exception& exc)
		{
			throw ValueException::FromString(exc.displayText());
		}
	}
	void File::IsReadonly(const ValueList& args, KValueRef result)
	{
		try
		{
#ifdef OS_WIN32
			Poco::File file(this->filename);
			result->SetBool(file.canRead() && !file.canWrite());
#else
			struct stat sb;
			stat(this->filename.c_str(),&sb);
			// can others read it?
			if ((sb.st_mode & S_IROTH)==S_IROTH)
			{
				result->SetBool(false);
			}
			else
			{
				result->SetBool(true);
			}
#endif
		}
		catch (Poco::FileNotFoundException &fnf)
		{
			result->SetBool(false);
		}
		catch (Poco::PathNotFoundException &fnf)
		{
			result->SetBool(false);
		}
		catch (Poco::Exception& exc)
		{
			throw ValueException::FromString(exc.displayText());
		}
	}
	void File::IsWriteable(const ValueList& args, KValueRef result)
	{
		try
		{
			Poco::File file(this->filename);
			result->SetBool(file.canWrite());
		}
		catch (Poco::FileNotFoundException &fnf)
		{
			result->SetBool(false);
		}
		catch (Poco::PathNotFoundException &fnf)
		{
			result->SetBool(false);
		}
		catch (Poco::Exception& exc)
		{
			throw ValueException::FromString(exc.displayText());
		}
	}
	void File::Resolve(const ValueList& args, KValueRef result)
	{
		try
		{
			std::string pathToResolve = args.at(0)->ToString();

			Poco::Path path(this->filename);
			path.resolve(pathToResolve);

			ti::File* file = new ti::File(path.toString());
			result->SetObject(file);
		}
		catch (Poco::Exception& exc)
		{
			throw ValueException::FromString(exc.displayText());
		}
	}
	void File::Write(const ValueList& args, KValueRef result)
	{
		FileStreamMode mode = MODE_WRITE;

		if(args.size() > 1)
		{
			if(args.at(1)->ToBool())
			{
				mode = MODE_APPEND;
			}
		}

		Logger* logger = Logger::Get("Filesystem.File");
		logger->Trace("write called for %s",this->filename.c_str());

		ti::FileStream fs(this->filename);
		fs.Open(mode);
		fs.Write(args, result);
		fs.Close();
#ifndef OS_WIN32
		chmod(this->filename.c_str(),S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
#endif		
	}

	void File::Read(const ValueList& args, KValueRef result)
	{
		Logger* logger = Logger::Get("Filesystem.File");
		logger->Trace("read called for %s",this->filename.c_str());

		FileStream fs(this->filename);
		fs.Open(MODE_READ);
		fs.Read(args, result);
		fs.Close();
	}
	void File::ReadLine(const ValueList& args, KValueRef result)
	{
		bool forceOpen = args.GetBool(0, false);

		if (!readLineFS || forceOpen)
		{
			// Close file if it's already open
			if (this->readLineFS)
				delete this->readLineFS;

			this->readLineFS = new ti::FileStream(this->filename);
			this->readLineFS->Open(MODE_READ);
		}

		this->readLineFS->ReadLine(args, result);

		// We've read the end of the file or errored out, so clean
		// up the Filestream object.
		if (result->IsNull())
		{
			delete this->readLineFS;
			this->readLineFS = 0;
		}
	}
	void File::Copy(const ValueList& args, KValueRef result)
	{
		try
		{
			std::string dest = FileSystemUtils::GetFileName(args.at(0))->c_str();
			Poco::File from(this->filename);
			from.copyTo(dest);
			result->SetBool(true);
		}
		catch (Poco::Exception& exc)
		{
			throw ValueException::FromString(exc.displayText());
		}
	}
	void File::Move(const ValueList& args, KValueRef result)
	{
		try
		{
			std::string dest = FileSystemUtils::GetFileName(args.at(0))->c_str();
			Poco::File from(this->filename);
			from.moveTo(dest);
			result->SetBool(true);
		}
		catch (Poco::Exception& exc)
		{
			throw ValueException::FromString(exc.displayText());
		}
	}
	void File::Rename(const ValueList& args, KValueRef result)
	{
		try
		{
			std::string name = args.at(0)->ToString();
			Poco::File f(this->filename);
			Poco::Path p(this->filename);
			p.setFileName(name);
			f.renameTo(p.toString());
			result->SetBool(true);
		}
		catch (Poco::Exception& exc)
		{
			throw ValueException::FromString(exc.displayText());
		}
	}
	void File::CreateDirectory(const ValueList& args, KValueRef result)
	{
		try
		{
			bool createParents = false;
			if(args.size() > 0)
			{
				createParents = args.at(0)->ToBool();
			}

			Poco::File dir(this->filename);
			bool created = false;
			if(! dir.exists())
			{
				if(createParents)
				{
					dir.createDirectories();
					created = true;
				}
				else {
					created = dir.createDirectory();
				}
			}
#ifndef OS_WIN32
		// directories must have execute bit by default or you can CD into them
		chmod(this->filename.c_str(),S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH|S_IXUSR|S_IXGRP|S_IXOTH);
#endif		
			result->SetBool(created);
		}
		catch (Poco::Exception& exc)
		{
			throw ValueException::FromString(exc.displayText());
		}
	}
	void File::DeleteDirectory(const ValueList& args, KValueRef result)
	{
		try
		{
			bool deleteContents = false;
			if(args.size() > 0)
			{
				deleteContents = args.at(0)->ToBool();
			}

			Poco::File dir(this->filename);
			bool deleted = false;
			if(dir.exists() && dir.isDirectory())
			{
				dir.remove(deleteContents);

				deleted = true;
			}
			result->SetBool(deleted);
		}
		catch (Poco::Exception& exc)
		{
			throw ValueException::FromString(exc.displayText());
		}
	}
	void File::DeleteFile(const ValueList& args, KValueRef result)
	{
		try
		{
			Poco::File file(this->filename);
			bool deleted = false;
			if(file.exists() && file.isFile())
			{
				file.remove();

				deleted = true;
			}
			result->SetBool(deleted);
		}
		catch (Poco::Exception& exc)
		{
			throw ValueException::FromString(exc.displayText());
		}
	}
	void File::GetDirectoryListing(const ValueList& args, KValueRef result)
	{
		try
		{
			Poco::File dir(this->filename);

			if(dir.exists() && dir.isDirectory())
			{
				std::vector<std::string> files;
				dir.list(files);

				KListRef fileList = new StaticBoundList();
				for(size_t i = 0; i < files.size(); i++)
				{
					std::string entry = files.at(i);
					// store it as the fullpath
					std::string filename = kroll::FileUtils::Join(this->filename.c_str(),entry.c_str(),NULL);
					ti::File* file = new ti::File(filename);
					KValueRef value = Value::NewObject((KObjectRef) file);
					fileList->Append(value);
				}

				KListRef list = fileList;
				result->SetList(list);
			}
			else
			{
				result->SetNull();
			}
		}
		catch (Poco::Exception& exc)
		{
			throw ValueException::FromString(exc.displayText());
		}
	}
	void File::GetParent(const ValueList& args, KValueRef result)
	{
		try
		{
			Poco::Path path(this->filename);

			ti::File* file = new ti::File(path.parent().toString());
			result->SetObject(file);
		}
		catch (Poco::Exception& exc)
		{
			throw ValueException::FromString(exc.displayText());
		}
	}
	void File::GetExists(const ValueList& args, KValueRef result)
	{
		try
		{
			Poco::File file(this->filename);
			bool exists = file.exists();
			result->SetBool(exists);
		}
		catch (Poco::Exception& exc)
		{
			throw ValueException::FromString(exc.displayText());
		}
	}
	void File::GetCreateTimestamp(const ValueList& args, KValueRef result)
	{
		try
		{
			Poco::File file(this->filename);
			Poco::Timestamp ts = file.created();

			result->SetDouble(ts.epochMicroseconds());
		}
		catch (Poco::Exception& exc)
		{
			throw ValueException::FromString(exc.displayText());
		}
	}
	void File::GetModificationTimestamp(const ValueList& args, KValueRef result)
	{
		try
		{
			Poco::File file(this->filename);
			Poco::Timestamp ts = file.getLastModified();

			result->SetDouble(ts.epochMicroseconds());
		}
		catch (Poco::Exception& exc)
		{
			throw ValueException::FromString(exc.displayText());
		}
	}
	void File::GetName(const ValueList& args, KValueRef result)
	{
		try
		{
			Poco::Path path(this->filename);
			result->SetString(path.getFileName().c_str());
		}
		catch (Poco::Exception& exc)
		{
			throw ValueException::FromString(exc.displayText());
		}
	}
	void File::GetExtension(const ValueList& args, KValueRef result)
	{
		try
		{
			Poco::Path path(this->filename);

			if(path.isDirectory())
			{
				result->SetNull();
			}
			else
			{
				result->SetString(path.getExtension().c_str());
			}
		}
		catch (Poco::Exception& exc)
		{
			throw ValueException::FromString(exc.displayText());
		}
	}
	void File::GetNativePath(const ValueList& args, KValueRef result)
	{
		try
		{
			Poco::Path path(this->filename);

			result->SetString(path.makeAbsolute().toString().c_str());
		}
		catch (Poco::Exception& exc)
		{
			throw ValueException::FromString(exc.displayText());
		}
	}
	void File::GetSize(const ValueList& args, KValueRef result)
	{
		try
		{
			Poco::File file(this->filename);

			result->SetDouble(file.getSize());
		}
		catch (Poco::Exception& exc)
		{
			throw ValueException::FromString(exc.displayText());
		}
	}
	void File::GetSpaceAvailable(const ValueList& args, KValueRef result)
	{	
		double diskSize = -1.0;
		std::string error;

#ifdef OS_OSX
		NSString *p = [NSString stringWithCString:this->filename.c_str() encoding:NSUTF8StringEncoding];
		unsigned long avail = [[[[NSFileManager defaultManager] fileSystemAttributesAtPath:p] objectForKey:NSFileSystemFreeSize] longValue];
		diskSize = (double)avail;
#elif defined(OS_WIN32)
		unsigned __int64 i64FreeBytesToCaller;
		unsigned __int64 i64TotalBytes;
		unsigned __int64 i64FreeBytes;
		if (GetDiskFreeSpaceExW(
			::UTF8ToWide(this->filename).c_str(),
			(PULARGE_INTEGER) &i64FreeBytesToCaller,
			(PULARGE_INTEGER) &i64TotalBytes,
			(PULARGE_INTEGER) &i64FreeBytes))
		{
			diskSize = (double)i64FreeBytesToCaller;
		}
		else
		{
			error = Win32Utils::QuickFormatMessage(GetLastError());
		}
#elif defined(OS_LINUX)
		struct statvfs stats;
		if (statvfs(this->filename.c_str(), &stats) == 0)
		{
			unsigned long avail = stats.f_bsize * static_cast<unsigned long>(stats.f_bavail);
			diskSize = (double)avail;
		}
#endif
		if ( diskSize >=0.0 )
		{
			result->SetDouble(diskSize);
		}
		else 
		{
			throw ValueException::FromString("Cannot determine diskspace on filename '" + this->filename + "' with error message " +error);
		}
	}
	void File::CreateShortcut(const ValueList& args, KValueRef result)
	{
		if (args.size()<1)
		{
			throw ValueException::FromString("createShortcut takes a parameter");
		}
		std::string from = this->filename;
		std::string to = args.at(0)->IsString() ? args.at(0)->ToString() : FileSystemUtils::GetFileName(args.at(0))->c_str();

#ifdef OS_OSX	//TODO: My spidey sense tells me that Cocoa might have a better way for this. --BTH
		NSMutableString* originalPath = [NSMutableString stringWithCString:from.c_str() encoding:NSUTF8StringEncoding];
		NSString* destPath = [NSString stringWithCString:to.c_str() encoding:NSUTF8StringEncoding];
		NSString* cwd = nil;
		NSFileManager* fm = [NSFileManager defaultManager];

		// support 2nd argument as a relative path to symlink for
		if (args.size()>1)
		{
			cwd = [fm currentDirectoryPath];
			NSString *p = [NSString stringWithCString:FileSystemUtils::GetFileName(args.at(1))->c_str() encoding:NSUTF8StringEncoding];
			BOOL isDirectory = NO;
			if ([fm fileExistsAtPath:p isDirectory:&isDirectory])
			{
				if (!isDirectory)
				{
					// trim it off to see if it's a directory
					p = [p stringByDeletingLastPathComponent];
				}
				[fm changeCurrentDirectoryPath:p];
				
				NSString * doomedString = [p stringByAppendingString: @"/"];
				[originalPath replaceOccurrencesOfString:doomedString withString: @"" options: NSLiteralSearch range:NSMakeRange(0,[originalPath length])];
//				originalPath = [originalPath stringByReplacingOccurrencesOfString:[NSString stringWithFormat:@"%@/",p] withString:@""];
			}
		}

		int rc = symlink([originalPath UTF8String],[destPath UTF8String]);
		BOOL worked = rc >= 0;
#ifdef DEBUG
		NSLog(@"++++ SYMLINK:%@=>%@ (%d)",originalPath,destPath,worked);
#endif
		result->SetBool(worked);
		if (cwd)
		{
			[[NSFileManager defaultManager] changeCurrentDirectoryPath:cwd];
		}
#elif defined(OS_WIN32)
		HRESULT hResult;
		IShellLinkW* psl;

		if(from.length() == 0 || to.length() == 0) {
			std::string ex = "Invalid arguments given to createShortcut()";
			throw ValueException::FromString(ex);
		}

		hResult = CoCreateInstance(CLSID_ShellLink, NULL,
			CLSCTX_INPROC_SERVER, IID_IShellLinkW, (LPVOID*)&psl);

		if(SUCCEEDED(hResult))
		{
			IPersistFile* ppf;

			// set path to the shortcut target and add description
			std::wstring wideFrom(::UTF8ToWide(from));
			psl->SetPath(wideFrom.c_str());
			psl->SetDescription(L"File shortcut");

			hResult = psl->QueryInterface(IID_IPersistFile, (LPVOID*) &ppf);

			if(SUCCEEDED(hResult))
			{
				// ensure to ends with .lnk
				if (to.substr(to.size()-4) != ".lnk") {
					to.append(".lnk");
				}
				
				WCHAR wsz[MAX_PATH];

				// ensure string is unicode
				if(MultiByteToWideChar(CP_ACP, 0, to.c_str(), -1, wsz, MAX_PATH))
				{
					// save the link
					hResult = ppf->Save(wsz, TRUE);
					ppf->Release();

					if(SUCCEEDED(hResult))
					{
						result->SetBool(true);
						return ;
					}
				}
			}
		}
		result->SetBool(false);
#elif defined(OS_LINUX)
		result->SetBool(link(this->filename.c_str(), to.c_str()) == 0);
#else
		result->SetBool(false);
#endif

	}
	void File::SetExecutable(const ValueList& args, KValueRef result)
	{
		try
		{
			Poco::File file(this->filename);
			file.setExecutable(args.at(0)->ToBool());
			result->SetBool(file.canExecute());
		}
		catch (Poco::FileNotFoundException &fnf)
		{
			result->SetBool(false);
		}
		catch (Poco::PathNotFoundException &fnf)
		{
			result->SetBool(false);
		}
		catch (Poco::Exception& exc)
		{
			throw ValueException::FromString(exc.displayText());
		}
	}
	void File::SetReadonly(const ValueList& args, KValueRef result)
	{
		try
		{
			bool readonly = args.at(0)->ToBool();
#ifndef OS_WIN32
			mode_t mode = S_IRUSR|S_IWUSR;
			Poco::File f(this->filename);
			if (f.canExecute())
			{
				mode |= S_IXUSR|S_IXGRP|S_IXOTH;
			}
			if (!readonly)
			{
				mode |= S_IRGRP | S_IROTH;
			}
			chmod(this->filename.c_str(),mode);
			result->SetBool(true);
#else
			Poco::File file(this->filename);
			file.setReadOnly(readonly);
			result->SetBool(!file.canRead());
#endif		
		}
		catch (Poco::FileNotFoundException &fnf)
		{
			result->SetBool(false);
		}
		catch (Poco::PathNotFoundException &fnf)
		{
			result->SetBool(false);
		}
		catch (Poco::Exception& exc)
		{
			throw ValueException::FromString(exc.displayText());
		}
	}
	void File::SetWriteable(const ValueList& args, KValueRef result)
	{
		try
		{
			Poco::File file(this->filename);
			file.setWriteable(args.at(0)->ToBool());
			result->SetBool(file.canWrite());
		}
		catch (Poco::FileNotFoundException &fnf)
		{
			result->SetBool(false);
		}
		catch (Poco::PathNotFoundException &fnf)
		{
			result->SetBool(false);
		}
		catch (Poco::Exception& exc)
		{
			throw ValueException::FromString(exc.displayText());
		}
	}
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
	void File::Unzip(const ValueList& args, KValueRef result)
	{
		if (args.size()!=1)
		{
			throw ValueException::FromString("invalid arguments - expected destination");
		}
		try
		{
			Poco::File from(this->filename);
			Poco::File to(FileSystemUtils::GetFileName(args.at(0))->c_str());
			std::string from_s = from.path();
			std::string to_s = to.path();
			if (!to.exists())
			{
				to.createDirectory();
			}
			if (!to.isDirectory())
			{
				throw ValueException::FromString("destination must be a directory");
			}
			kroll::FileUtils::Unzip(from_s,to_s);
			result->SetBool(true);
		}
		catch (Poco::FileNotFoundException &fnf)
		{
			result->SetBool(false);
		}
		catch (Poco::PathNotFoundException &fnf)
		{
			result->SetBool(false);
		}
		catch (Poco::Exception& exc)
		{
			throw ValueException::FromString(exc.displayText());
		}
	}
}
