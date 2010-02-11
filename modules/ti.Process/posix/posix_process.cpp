/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#include "posix_process.h"
#include <errno.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <unistd.h>
#if defined(OS_OSX)
# include <crt_externs.h>
# include <Poco/Path.h>
#endif
extern char **environ;

namespace ti
{
	PosixProcess::PosixProcess() :
		logger(Logger::Get("Process.PosixProcess")),
		nativeIn(new PosixPipe(false)),
		nativeOut(new PosixPipe(true)),
		nativeErr(new PosixPipe(true))
	{
		stdinPipe->Attach(this->GetNativeStdin());
	}

	PosixProcess::~PosixProcess()
	{
	}

	void PosixProcess::RecreateNativePipes()
	{
		this->nativeIn = new PosixPipe(false);
		this->nativeOut = new PosixPipe(true);
		this->nativeErr = new PosixPipe(true);
		stdinPipe->Attach(this->GetNativeStdin());
	}

	void PosixProcess::SetArguments(KListRef args)
	{
#if defined(OS_OSX)
		std::string cmd = args->At(0)->ToString();
		size_t found = cmd.rfind(".app");
		if (found != std::string::npos)
		{
			Poco::Path p(cmd);
			std::string fn = p.getFileName();
			found = fn.find(".app");
			fn = fn.substr(0,found);
			fn = kroll::FileUtils::Join(cmd.c_str(),"Contents","MacOS",fn.c_str(),NULL);
			if (FileUtils::IsFile(fn))
			{
				cmd = fn;
			}
		}
		args->At(0)->SetString(cmd.c_str());
#endif
		Process::SetArguments(args);
	}

	void PosixProcess::ForkAndExec()
	{
		nativeIn->CreateHandles();
		nativeOut->CreateHandles();
		nativeErr->CreateHandles();

		int pid = fork();
		if (pid < 0)
		{
			throw ValueException::FromFormat("Cannot fork process for %s", 
				args->At(0)->ToString());
		}
		else if (pid == 0)
		{
			// outPipe and errPipe may be the same, so we dup first and close later
			dup2(nativeIn->GetReadHandle(), STDIN_FILENO);
			dup2(nativeOut->GetWriteHandle(), STDOUT_FILENO);
			dup2(nativeErr->GetWriteHandle(), STDERR_FILENO);
			nativeIn->CloseNative();
			nativeOut->CloseNative();
			nativeErr->CloseNative();

			// close all open file descriptors other than stdin, stdout, stderr
			for (int i = 3; i < getdtablesize(); ++i)
				close(i);

			size_t i = 0;
			char** argv = new char*[args->Size() + 1];
			//argv[i++] = const_cast<char*>(command.c_str());
			for (;i < args->Size(); i++)
			{
				argv[i] = const_cast<char*>(args->At(i)->ToString());
			}
			argv[i] = NULL;

			SharedStringList envNames = environment->GetPropertyNames();
			for (i = 0; i < envNames->size(); i++)
			{
				const char* key = envNames->at(i)->c_str();
				std::string value(environment->Get(key)->ToString());
				setenv(key, value.c_str(), 1);
			}

			const char *command = args->At(0)->ToString();
			execvp(command, argv);
			_exit(72);
		}

		SetPID(pid);
		nativeIn->CloseNativeRead();
		nativeOut->CloseNativeWrite();
		nativeErr->CloseNativeWrite();
	}

	void PosixProcess::MonitorAsync()
	{
		nativeIn->StartMonitor();
		nativeOut->StartMonitor();
		nativeErr->StartMonitor();
	}

	BytesRef PosixProcess::MonitorSync()
	{
		KMethodRef readCallback =
			StaticBoundMethod::FromMethod<PosixProcess>(
				this, &PosixProcess::ReadCallback);

		// Set up the synchronous callbacks
		nativeOut->SetReadCallback(readCallback);
		nativeErr->SetReadCallback(readCallback);

		nativeIn->StartMonitor();
		nativeOut->StartMonitor();
		nativeErr->StartMonitor();

		this->ExitMonitorSync();

		// Unset the callbacks just in case these pipes are used again
		nativeOut->SetReadCallback(0);
		nativeErr->SetReadCallback(0);

		BytesRef output = 0;
		{
			Poco::Mutex::ScopedLock lock(processOutputMutex);
			output = Bytes::GlobBytes(processOutput);
		}
		return output;
	}

	int PosixProcess::Wait()
	{
		int status;
		int rc;
		do
		{
			rc = waitpid(GetPID(), &status, 0);
		} while (rc < 0 && errno == EINTR);

		if (rc != GetPID())
		{
			throw ValueException::FromFormat("Cannot wait for process: %d", GetPID());
		}

		int exitCode = WEXITSTATUS(status);
		return exitCode;
	}

	void PosixProcess::ReadCallback(const ValueList& args, KValueRef result)
	{
		if (args.at(0)->IsObject())
		{
			BytesRef bytes = args.GetObject(0).cast<Bytes>();
			if (!bytes.isNull() && bytes->Length() > 0)
			{
				Poco::Mutex::ScopedLock lock(processOutputMutex);
				processOutput.push_back(bytes);
			}
		}
	}

	void PosixProcess::SendSignal(int signal)
	{
		if (!running)
			return;

		if (kill(GetPID(), signal) != 0)
		{
			switch (errno)
			{
			case ESRCH:
				throw ValueException::FromString("Couldn't find process");
			case EPERM:
				throw ValueException::FromString("Invalid permissions for terminating process");
			default:
				throw ValueException::FromFormat("Couldn't send signal: %d to process", signal);
			}
		}
	}

	void PosixProcess::Kill()
	{
		SendSignal(SIGKILL);
	}

	void PosixProcess::Terminate()
	{
		SendSignal(SIGINT);
	}
}
