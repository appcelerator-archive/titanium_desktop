/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#include <kroll/kroll.h>
#include <kroll/thread_manager.h>
#include "filesystem_binding.h"
#include "file.h"
#include "async_copy.h"
#include "filesystem_utils.h"
#include "app_config.h"

#ifdef OS_OSX
#include <Cocoa/Cocoa.h>
#elif OS_WIN32
#include <windows.h>
#include <shlobj.h>
#include <process.h>
#elif OS_LINUX
#include <sys/types.h>
#include <pwd.h>
#endif

#include <Poco/TemporaryFile.h>
#include <Poco/File.h>
#include <Poco/Path.h>
#include <Poco/LineEndingConverter.h>
#include <Poco/Exception.h>

namespace ti
{
	FilesystemBinding::FilesystemBinding(Host *host, SharedKObject global) : host(host), global(global), timer(0)
	{
		/**
		 * @tiapi(method=True,name=Filesystem.createTempFile) Creates a temporary file
		 * @tiresult(for=Filesystem.createTempFile,type=Filesystem.File) a File object referencing the temporary file
		 */
		this->SetMethod("createTempFile",&FilesystemBinding::CreateTempFile);
		/**
		 * @tiapi(method=True,name=Filesystem.createTempDirectory) Creates a temporary directory
		 * @tiresult(for=Filesystem.createTempDirectory,type=Filesystem.File) a File object referencing the temporary directory
		 */
		this->SetMethod("createTempDirectory",&FilesystemBinding::CreateTempDirectory);
		/**
		 * @tiapi(method=True,name=Filesystem.getFile) Returns a file path, optionally joining multiple arguments together in an OS specific way
		 * @tiarg(for=Filesystem.getFile,name=pathname) a string that is used to form a path
		 * @tiarg(for=Filesystem.getFile,optional=true,name=...) a variable length argument list of Strings that are concatinated with pathname to form a path
		 * @tiresult(for=Filesystem.getFile,type=FileSystem.File) a File object referencing the file
		 */
		this->SetMethod("getFile",&FilesystemBinding::GetFile);
		/**
		 * @tiapi(method=True,name=Filesystem.getFileStream) Returns a Filestream object
		 * @tiresult(for=Filesystem.getFileStream,type=FileSystem.Filestream) a Filestream object referencing the file
		 */
		this->SetMethod("getFileStream",&FilesystemBinding::GetFileStream);
		/**
		 * @tiapi(method=True,name=Filesystem.getProgramsDirectory) Returns the programs directory of the current system
		 * @tiresult(for=Filesystem.getProgramsDirectory,type=FileSystem.File) a File object referencing the system programs directory
		 */
		this->SetMethod("getProgramsDirectory",&FilesystemBinding::GetProgramsDirectory);
		/**
		 * @tiapi(method=True,name=Filesystem.getApplicationDirectory) Returns the directory where the application resides
		 * @tiresult(for=Filesystem.getApplicationDirectory,type=FileSystem.File) a File object referencing the directory where the application resides
		 */
		this->SetMethod("getApplicationDirectory",&FilesystemBinding::GetApplicationDirectory);
		/**
		 * @tiapi(method=True,name=Filesystem.getApplicationDataDirectory) Returns the data directory of the application
		 * @tiresult(for=Filesystem.getApplicationDataDirectory,type=FileSystem.File) a File object referencing the data directory of the application
		 */
		this->SetMethod("getApplicationDataDirectory",&FilesystemBinding::GetApplicationDataDirectory);
		/**
		 * @tiapi(method=True,name=Filesystem.getRuntimeHomeDirectory) Returns the directory of where the Titanium runtime files are stored
		 * @tiresult(for=Filesystem.getRuntimeHomeDirectory,type=FileSystem.File) a File object referencing the directory where the Titanium runtime files are stored.
		 */
		this->SetMethod("getRuntimeHomeDirectory",&FilesystemBinding::GetRuntimeHomeDirectory);
		/**
		 * @tiapi(method=True,name=Filesystem.getResourcesDirectory) Returns the resources directory of the application
		 * @tiresult(for=Filesystem.getResourcesDirectory,type=FileSystem.File) a File object referencing the resources directory of the application
		 */
		this->SetMethod("getResourcesDirectory",&FilesystemBinding::GetResourcesDirectory);
		/**
		 * @tiapi(method=True,name=Filesystem.getDesktopDirectory) Returns the system desktop directory
		 * @tiresult(for=Filesystem.getDesktopDirectory,type=FileSystem.File) a File object referencing the system desktop directory
		 */
		this->SetMethod("getDesktopDirectory",&FilesystemBinding::GetDesktopDirectory);
		/**
		 * @tiapi(method=True,name=Filesystem.getDocumentsDirectory) Returns the system documents directory
		 * @tiresult(for=Filesystem.getDocumentsDirectory,type=FileSystem.File) a File object referencing the system documents directory
		 */
		this->SetMethod("getDocumentsDirectory",&FilesystemBinding::GetDocumentsDirectory);
		/**
		 * @tiapi(method=True,name=Filesystem.getUserDirectory) Returns the home directory of the current user
		 * @tiresult(for=Filesystem.getUserDirectory,type=FileSystem.File) a File object referencing the home directory of the current user
		 */
		this->SetMethod("getUserDirectory",&FilesystemBinding::GetUserDirectory);
		/**
		 * @tiapi(method=True,name=Filesystem.getLineEnding) Returns the line ending used by the operating system
		 * @tiresult(for=Filesystem.getLineEnding,type=String) the line ending used by the operating system
		 */
		this->SetMethod("getLineEnding",&FilesystemBinding::GetLineEnding);
		/**
		 * @tiapi(method=True,name=Filesystem.getSeparator) Returns the path separator used by the operating system
		 * @tiresult(for=Filesystem.getSeparator,type=String) the path separator used by the operating system
		 */
		this->SetMethod("getSeparator",&FilesystemBinding::GetSeparator);
		/**
		 * @tiapi(method=True,name=Filesystem.getRootDirectories) Returns the system root directory
		 * @tiresult(for=Filesystem.getRootDirectories,type=Array<FileSystem.File>) a File object referencing the system root directory
		 */
		this->SetMethod("getRootDirectories",&FilesystemBinding::GetRootDirectories);
		/**
		 * @tiapi(method=True,name=Filesystem.asyncCopy) Executes an async copy operation
		 * @tiarg(for=Filesystem.asyncCopy,name=paths,type=Array<String|Filesystem.File>|Filesystem.File)
		 * @tiarg Either a path or array of paths to copy from
		 * @tiarg(for=Filesystem.asyncCopy,name=destination,type=Filesystem.File|String) either a string or file object to copy to
		 * @tiarg(for=Filesystem.asyncCopy,name=callback,type=Function) callback to invoke on each copy completion operation
		 * @tiresult(for=Filesystem.asyncCopy,type=FileSystem.AsyncCopy) async copy object
		 */
		this->SetMethod("asyncCopy",&FilesystemBinding::ExecuteAsyncCopy);

		/**
		 * @tiapi(property=True,immutable=True,name=Filesystem.MODE_READ, since=0.3, type=Number) File read constant
		 */
		this->Set("MODE_READ", Value::NewInt(MODE_READ));
		/**
		 * @tiapi(property=True,immutable=True,name=Filesystem.MODE_WRITE, since=0.3, type=Number) File write constant
		 */
		this->Set("MODE_WRITE", Value::NewInt(MODE_WRITE));
		/**
		 * @tiapi(property=True,immutable=True,name=Filesystem.MODE_APPEND, since=0.3, type=Number) File append constant
		 */
		this->Set("MODE_APPEND", Value::NewInt(MODE_APPEND));
	}

	FilesystemBinding::~FilesystemBinding()
	{
		if (this->timer!=NULL)
		{
			this->timer->stop();
			delete this->timer;
			this->timer = NULL;
		}
	}

	void FilesystemBinding::CreateTempFile(const ValueList& args, SharedValue result)
	{
		try
		{
			Poco::TemporaryFile tempFile;
			tempFile.keepUntilExit();
			tempFile.createFile();

			ti::File* jsFile = new ti::File(tempFile.path());
			result->SetObject(jsFile);
		}
		catch (Poco::Exception& exc)
		{
			throw ValueException::FromString(exc.displayText());
		}
	}

	void FilesystemBinding::CreateTempDirectory(const ValueList& args, SharedValue result)
	{
		try
		{
			Poco::TemporaryFile tempDir;
			tempDir.keepUntilExit();
			tempDir.createDirectory();

			ti::File* jsFile = new ti::File(tempDir.path());
			result->SetObject(jsFile);
		}
		catch (Poco::Exception& exc)
		{
			throw ValueException::FromString(exc.displayText());
		}
	}

	void FilesystemBinding::ResolveFileName(const ValueList& args, std::string& filename)
	{
		if (args.at(0)->IsList())
		{
			// you can pass in an array of parts to join
			SharedKList list = args.at(0)->ToList();
			for (size_t c = 0; c < list->Size(); c++)
			{
				std::string arg = list->At(c)->ToString();
				filename = kroll::FileUtils::Join(filename.c_str(), arg.c_str(), NULL);
			}
		}
		else
		{
			// you can pass in vararg of strings which acts like  a join
			for (size_t c = 0; c < args.size(); c++)
			{
				std::string arg = FileSystemUtils::GetFileName(args.at(c))->c_str();
				filename = kroll::FileUtils::Join(filename.c_str(), arg.c_str(), NULL);
			}
		}
		if (filename.empty())
		{
			throw ValueException::FromString("invalid file type");
		}
	}

	void FilesystemBinding::GetFile(const ValueList& args, SharedValue result)
	{
		std::string filename;
		this->ResolveFileName(args, filename);
		ti::File* file = new ti::File(filename);
		result->SetObject(file);
	}

	void FilesystemBinding::GetFileStream(const ValueList& args, SharedValue result)
	{
		std::string filename;
		this->ResolveFileName(args, filename);
		ti::FileStream* fs = new ti::FileStream(filename);
		result->SetObject(fs);
	}

	void FilesystemBinding::GetApplicationDirectory(const ValueList& args, SharedValue result)
	{
		ti::File* file = new ti::File(host->GetApplication()->path);
		result->SetObject(file);
	}

	void FilesystemBinding::GetApplicationDataDirectory(const ValueList& args, SharedValue result)
	{
		std::string appid = AppConfig::Instance()->GetAppID();
		std::string dir = FileUtils::GetApplicationDataDirectory(appid);
		ti::File* file = new ti::File(dir);
		result->SetObject(file);
	}

	void FilesystemBinding::GetRuntimeHomeDirectory(const ValueList& args, SharedValue result)
	{
		std::string dir = FileUtils::GetSystemRuntimeHomeDirectory();
		ti::File* file = new ti::File(dir);
		result->SetObject(file);
	}

	void FilesystemBinding::GetResourcesDirectory(const ValueList& args, SharedValue result)
	{
		ti::File* file = new ti::File(host->GetApplication()->GetResourcesPath());
		result->SetObject(file);
	}

	void FilesystemBinding::GetProgramsDirectory(const ValueList &args, SharedValue result)
	{
#ifdef OS_WIN32
		char path[MAX_PATH];
		if (!SHGetSpecialFolderPath(NULL, path, CSIDL_PROGRAM_FILES, FALSE))
			throw ValueException::FromString("Could not get Program Files path.");
		std::string dir(path);

#elif OS_OSX
		std::string dir([[NSSearchPathForDirectoriesInDomains(
			NSApplicationDirectory, NSLocalDomainMask, YES)
			objectAtIndex: 0] UTF8String]);

#elif OS_LINUX
		// TODO: this might need to be configurable
		std::string dir("/usr/local/bin");
#endif
		ti::File* file = new ti::File(dir);
		result->SetObject(file);
	}

	void FilesystemBinding::GetDesktopDirectory(const ValueList& args, SharedValue result)
	{
#ifdef OS_WIN32
		char path[MAX_PATH];
		if (!SHGetSpecialFolderPath(NULL, path, CSIDL_DESKTOPDIRECTORY, FALSE))
			throw ValueException::FromString("Could not get Desktop path.");
		std::string dir(path);

#elif OS_OSX
		std::string dir([[NSSearchPathForDirectoriesInDomains(
			NSDesktopDirectory, NSUserDomainMask, YES)
			objectAtIndex: 0] UTF8String]);

#elif OS_LINUX
		passwd *user = getpwuid(getuid());
		std::string homeDirectory = user->pw_dir;
		std::string dir(FileUtils::Join(homeDirectory.c_str(), "Desktop", NULL));
		if (!FileUtils::IsDirectory(dir))
		{
			dir = homeDirectory;
		}
#endif
		ti::File* file = new ti::File(dir);
		result->SetObject(file);
	}

	void FilesystemBinding::GetDocumentsDirectory(const ValueList& args, SharedValue result)
	{
#ifdef OS_WIN32
		char path[MAX_PATH];
		if (!SHGetSpecialFolderPath(NULL,path,CSIDL_PERSONAL,FALSE))
			throw ValueException::FromString("Could not get Documents path.");
		std::string dir(path);

#elif OS_OSX
		std::string dir([[NSSearchPathForDirectoriesInDomains(
			NSDocumentDirectory, NSUserDomainMask, YES)
			objectAtIndex: 0] UTF8String]);

#elif OS_LINUX
		passwd* user = getpwuid(getuid());
		std::string homeDirectory = user->pw_dir;
		std::string dir(FileUtils::Join(homeDirectory.c_str(), "Documents", NULL));
		if (!FileUtils::IsDirectory(dir))
		{
			dir = homeDirectory;
		}
#endif
		ti::File* file = new ti::File(dir);
		result->SetObject(file);
	}

	void FilesystemBinding::GetUserDirectory(const ValueList& args, SharedValue result)
	{
		std::string dir;
		try
		{
			dir = Poco::Path::home().c_str();
		}
		catch (Poco::Exception& exc)
		{
			std::string error = "Could not determine home directory: ";
			error.append(exc.displayText());
			throw ValueException::FromString(error);
		}

#ifdef OS_WIN32
		// If dir is equal to C:\ get the diretory in a different way.
		// Poco uses %%HOMEPATH%% to get this directory which might be borked
		// e.g. while running in Cygwin.
		if (dir.size() == 3)
		{
			std::string odir = EnvironmentUtils::Get("USERPROFILE");
			if (!odir.empty())
			{
				dir = odir;
			}
		}
#endif

		ti::File* file = new ti::File(dir);
		result->SetObject(file);
	}
	void FilesystemBinding::GetLineEnding(const ValueList& args, SharedValue result)
	{
		try
		{
			result->SetString(Poco::LineEnding::NEWLINE_LF.c_str());
		}
		catch (Poco::Exception& exc)
		{
			throw ValueException::FromString(exc.displayText());
		}
	}
	void FilesystemBinding::GetSeparator(const ValueList& args, SharedValue result)
	{
		try
		{
			std::string sep;
			sep += Poco::Path::separator();
			result->SetString(sep.c_str());
		}
		catch (Poco::Exception& exc)
		{
			throw ValueException::FromString(exc.displayText());
		}
	}
	void FilesystemBinding::GetRootDirectories(const ValueList& args, SharedValue result)
	{
		try
		{
			Poco::Path path;
			std::vector<std::string> roots;
			path.listRoots(roots);

			SharedKList rootList = new StaticBoundList();
			for(size_t i = 0; i < roots.size(); i++)
			{
				ti::File* file = new ti::File(roots.at(i));
				SharedValue value = Value::NewObject((SharedKObject) file);
				rootList->Append(value);
			}

			SharedKList list = rootList;
			result->SetList(list);
		}
		catch (Poco::Exception& exc)
		{
			throw ValueException::FromString(exc.displayText());
		}
	}
	void FilesystemBinding::ExecuteAsyncCopy(const ValueList& args, SharedValue result)
	{
		if (args.size()!=3)
		{
			throw ValueException::FromString("invalid arguments - this method takes 3 arguments");
		}
		std::vector<std::string> files;
		if (args.at(0)->IsString())
		{
			files.push_back(args.at(0)->ToString());
		}
		else if (args.at(0)->IsList())
		{
			SharedKList list = args.at(0)->ToList();
			for (unsigned int c = 0; c < list->Size(); c++)
			{
				SharedValue v = list->At(c);
				files.push_back(FileSystemUtils::GetFileName(v)->c_str());
			}
		}
		else
		{
			files.push_back(FileSystemUtils::GetFileName(args.at(0))->c_str());
		}
		SharedValue v = args.at(1);
		std::string destination(FileSystemUtils::GetFileName(v)->c_str());
		SharedKMethod method = args.at(2)->ToMethod();
		SharedKObject copier = new ti::AsyncCopy(this,host,files,destination,method);
		result->SetObject(copier);
		asyncOperations.push_back(copier);
		// we need to create a timer thread that can cleanup operations
		if (timer==NULL)
		{
			this->SetMethod("_invoke",&FilesystemBinding::DeletePendingOperations);
			timer = new Poco::Timer(100,100);
			Poco::TimerCallback<FilesystemBinding> cb(*this, &FilesystemBinding::OnAsyncOperationTimer);
			timer->start(cb);
		}
		else
		{
			this->timer->restart(100);
		}
	}
	void FilesystemBinding::DeletePendingOperations(const ValueList& args, SharedValue result)
	{
		KR_DUMP_LOCATION
		if (asyncOperations.size()==0)
		{
			result->SetBool(true);
			return;
		}
		std::vector<SharedKObject>::iterator iter = asyncOperations.begin();

		while (iter!=asyncOperations.end())
		{
			SharedKObject c = (*iter);
			SharedValue v = c->Get("running");
			bool running = v->ToBool();
			if (!running)
			{
				asyncOperations.erase(iter);
				break;
			}
			iter++;
		}

		// return true to pause the timer
		result->SetBool(asyncOperations.size()==0);
	}
	void FilesystemBinding::OnAsyncOperationTimer(Poco::Timer &timer)
	{
		START_KROLL_THREAD;

		ValueList args = ValueList();
		SharedKMethod m = this->Get("_invoke")->ToMethod();
		SharedValue result = host->InvokeMethodOnMainThread(m, args);
		if (result->ToBool())
		{
			timer.restart(0);
		}

		END_KROLL_THREAD;
	}
}
