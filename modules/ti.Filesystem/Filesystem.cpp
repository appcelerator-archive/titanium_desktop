/*
 * Copyright (c) 2009-2010 Appcelerator, Inc. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "Filesystem.h"

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

#include <kroll/kroll.h>
#include <kroll/thread_manager.h>
#include <Poco/TemporaryFile.h>
#include <Poco/File.h>
#include <Poco/Path.h>
#include <Poco/LineEndingConverter.h>
#include <Poco/Exception.h>

#include "File.h"
#include "FileStream.h"
#include "AsyncCopy.h"
#include "FilesystemUtils.h"

namespace Titanium {

Filesystem::Filesystem(Host *host, KObjectRef global) :
    StaticBoundObject("Filesystem"),
    host(host),
    global(global),
    timer(0)
{

    this->SetMethod("createTempFile", &Filesystem::CreateTempFile);
    this->SetMethod("createTempDirectory", &Filesystem::CreateTempDirectory);
    this->SetMethod("getFile", &Filesystem::GetFile);
    this->SetMethod("getFileStream", &Filesystem::GetFileStream);
    this->SetMethod("getProgramsDirectory", &Filesystem::GetProgramsDirectory);
    this->SetMethod("getApplicationDirectory", &Filesystem::GetApplicationDirectory);
    this->SetMethod("getApplicationDataDirectory", &Filesystem::GetApplicationDataDirectory);
    this->SetMethod("getRuntimeHomeDirectory", &Filesystem::GetRuntimeHomeDirectory);
    this->SetMethod("getResourcesDirectory", &Filesystem::GetResourcesDirectory);
    this->SetMethod("getDesktopDirectory", &Filesystem::GetDesktopDirectory);
    this->SetMethod("getDocumentsDirectory", &Filesystem::GetDocumentsDirectory);
    this->SetMethod("getUserDirectory", &Filesystem::GetUserDirectory);
    this->SetMethod("getLineEnding", &Filesystem::GetLineEnding);
    this->SetMethod("getSeparator", &Filesystem::GetSeparator);
    this->SetMethod("getRootDirectories", &Filesystem::GetRootDirectories);
    this->SetMethod("asyncCopy", &Filesystem::ExecuteAsyncCopy);

    this->SetInt("MODE_READ", FileStream::MODE_READ);
    this->SetInt("MODE_WRITE", FileStream::MODE_WRITE);
    this->SetInt("MODE_APPEND", FileStream::MODE_APPEND);
    this->SetInt("SEEK_START", std::ios::beg);
    this->SetInt("SEEK_CURRENT", std::ios::cur);
    this->SetInt("SEEK_END", std::ios::end);
}

Filesystem::~Filesystem()
{
    if (this->timer!=NULL)
    {
        this->timer->stop();
        delete this->timer;
        this->timer = NULL;
    }
}

void Filesystem::CreateTempFile(const ValueList& args, KValueRef result)
{
    try
    {
        Poco::TemporaryFile tempFile;
        tempFile.keepUntilExit();
        tempFile.createFile();

        File* jsFile = new File(tempFile.path());
        result->SetObject(jsFile);
    }
    catch (Poco::Exception& exc)
    {
        throw ValueException::FromString(exc.displayText());
    }
}

void Filesystem::CreateTempDirectory(const ValueList& args, KValueRef result)
{
    try
    {
        Poco::TemporaryFile tempDir;
        tempDir.keepUntilExit();
        tempDir.createDirectory();

        File* jsFile = new File(tempDir.path());
        result->SetObject(jsFile);
    }
    catch (Poco::Exception& exc)
    {
        throw ValueException::FromString(exc.displayText());
    }
}


void Filesystem::GetFile(const ValueList& args, KValueRef result)
{
    result->SetObject(new File(FilenameFromArguments(args)));
}

void Filesystem::GetFileStream(const ValueList& args, KValueRef result)
{
    result->SetObject(new FileStream(FilenameFromArguments(args)));
}

void Filesystem::GetApplicationDirectory(const ValueList& args, KValueRef result)
{
    result->SetObject(new File(host->GetApplication()->path));
}

void Filesystem::GetApplicationDataDirectory(const ValueList& args, KValueRef result)
{
    result->SetObject(new File(
        Host::GetInstance()->GetApplication()->GetDataPath()));
}

void Filesystem::GetRuntimeHomeDirectory(const ValueList& args, KValueRef result)
{
    std::string dir = FileUtils::GetSystemRuntimeHomeDirectory();
    File* file = new File(dir);
    result->SetObject(file);
}

void Filesystem::GetResourcesDirectory(const ValueList& args, KValueRef result)
{
    File* file = new File(host->GetApplication()->GetResourcesPath());
    result->SetObject(file);
}

void Filesystem::GetProgramsDirectory(const ValueList &args, KValueRef result)
{
#ifdef OS_WIN32
    wchar_t path[MAX_PATH];
    if (!SHGetSpecialFolderPathW(NULL, path, CSIDL_PROGRAM_FILES, FALSE))
        throw ValueException::FromString("Could not get Program Files path.");
    std::string dir(::WideToUTF8(path));

#elif OS_OSX
    std::string dir([[NSSearchPathForDirectoriesInDomains(
        NSApplicationDirectory, NSLocalDomainMask, YES)
        objectAtIndex: 0] UTF8String]);

#elif OS_LINUX
    // TODO: this might need to be configurable
    std::string dir("/usr/local/bin");
#endif
    File* file = new File(dir);
    result->SetObject(file);
}

void Filesystem::GetDesktopDirectory(const ValueList& args, KValueRef result)
{
#ifdef OS_WIN32
    wchar_t path[MAX_PATH];
    if (!SHGetSpecialFolderPathW(NULL, path, CSIDL_DESKTOPDIRECTORY, FALSE))
        throw ValueException::FromString("Could not get Desktop path.");
    std::string dir(::WideToUTF8(path));

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
    File* file = new File(dir);
    result->SetObject(file);
}

void Filesystem::GetDocumentsDirectory(const ValueList& args, KValueRef result)
{
#ifdef OS_WIN32
    wchar_t path[MAX_PATH];
    if (!SHGetSpecialFolderPathW(NULL, path, CSIDL_PERSONAL, FALSE))
        throw ValueException::FromString("Could not get Documents path.");
    std::string dir(::WideToUTF8(path));

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
    File* file = new File(dir);
    result->SetObject(file);
}

void Filesystem::GetUserDirectory(const ValueList& args, KValueRef result)
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

    File* file = new File(dir);
    result->SetObject(file);
}

void Filesystem::GetLineEnding(const ValueList& args, KValueRef result)
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

void Filesystem::GetSeparator(const ValueList& args, KValueRef result)
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

void Filesystem::GetRootDirectories(const ValueList& args, KValueRef result)
{
    try
    {
        Poco::Path path;
        std::vector<std::string> roots;
        path.listRoots(roots);

        KListRef rootList = new StaticBoundList();
        for(size_t i = 0; i < roots.size(); i++)
        {
            File* file = new File(roots.at(i));
            KValueRef value = Value::NewObject((KObjectRef) file);
            rootList->Append(value);
        }

        KListRef list = rootList;
        result->SetList(list);
    }
    catch (Poco::Exception& exc)
    {
        throw ValueException::FromString(exc.displayText());
    }
}

void Filesystem::ExecuteAsyncCopy(const ValueList& args, KValueRef result)
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
        KListRef list = args.at(0)->ToList();
        for (unsigned int c = 0; c < list->Size(); c++)
        {
            files.push_back(FilenameFromValue(list->At(c)));
        }
    }
    else
    {
        files.push_back(FilenameFromValue(args.at(0)));
    }
    KValueRef v = args.at(1);
    std::string destination(FilenameFromValue(v));
    KMethodRef method = args.at(2)->ToMethod();
    KObjectRef copier = new AsyncCopy(this,host,files,destination,method);
    result->SetObject(copier);
    asyncOperations.push_back(copier);
    // we need to create a timer thread that can cleanup operations
    if (timer==NULL)
    {
        this->SetMethod("_invoke",&Filesystem::DeletePendingOperations);
        timer = new Poco::Timer(100,100);
        Poco::TimerCallback<Filesystem> cb(*this, &Filesystem::OnAsyncOperationTimer);
        timer->start(cb);
    }
    else
    {
        this->timer->restart(100);
    }
}

void Filesystem::DeletePendingOperations(const ValueList& args, KValueRef result)
{
    KR_DUMP_LOCATION
    if (asyncOperations.size()==0)
    {
        result->SetBool(true);
        return;
    }
    std::vector<KObjectRef>::iterator iter = asyncOperations.begin();

    while (iter!=asyncOperations.end())
    {
        KObjectRef c = (*iter);
        KValueRef v = c->Get("running");
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

void Filesystem::OnAsyncOperationTimer(Poco::Timer &timer)
{
    START_KROLL_THREAD;

    ValueList args = ValueList();
    KMethodRef m = this->Get("_invoke")->ToMethod();
    KValueRef result = RunOnMainThread(m, args);
    if (result->ToBool())
    {
        timer.restart(0);
    }

    END_KROLL_THREAD;
}

} // namespace Titanium
