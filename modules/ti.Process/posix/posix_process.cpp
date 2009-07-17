
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

namespace ti
{	
	// Parts derived from Poco::Process_UNIX
	AutoPtr<PosixProcess> PosixProcess::currentProcess = new PosixProcess();
	
	/*static*/
	AutoPtr<PosixProcess> PosixProcess::GetCurrentProcess()
	{
		return currentProcess;
	}
	
	PosixProcess::PosixProcess(SharedKList args, SharedKObject environment, 
			AutoPipe stdinPipe, AutoPipe stdoutPipe, AutoPipe stderrPipe) :
		Process(args, environment, stdinPipe, stdoutPipe, stderrPipe),
		pid(-1),
		logger(Logger::Get("Process.PosixProcess"))
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
	}
	
	PosixProcess::PosixProcess() :
		Process(),
		running(true), complete(false), current(true),
		logger(Logger::Get("Process.PosixProcess"))
	{
		pid = getpid();
#if defined(OS_OSX)
		char **environ = *_NSGetEnviron();
#endif
		for (int i = 0; environ[i] != NULL; i++)
		{
			std::string entry = environ[i];
			std::string key = entry.substr(0, entry.find("="));
			std::string val = entry.substr(entry.find("=")+1);
			
			SetEnvironment(key.c_str(), val.c_str());
		}
		
		SharedApplication app = Host::GetInstance()->GetApplication();
		for (size_t i = 0; i < app->arguments.size(); i++)
		{
			args->Append(Value::NewString(app->arguments.at(i)));
		}
	}

	PosixProcess::~PosixProcess()
	{
		Poco::Mutex::ScopedLock lock(nativeProcessesMutex);
		for (size_t i = 0; i < nativeProcesses.size(); i++)
		{
			// Tell our native processes that we are no longer active
			// They will clean themselves up when they finish executing.
			nativeProcesses.at(i)->process = 0;
		}
	}

	NativePosixProcess::NativePosixProcess(PosixProces* process) :
		process(process)
		exitStatus(-1),
		pid(-1),
		stdinPipe(new PosixPipe()),
		stdoutPipe(new PosixPipe()),
		stderrPipe(new StderrPipe()),
		exitMonitorAdapter(new Poco::RunnableAdapter<NativePosixProcess>(
			*this, &NativePosixProcess::ExitMonitor)),
		exitCallback(0)
	{
	}

	NativePosixProcess::~NativePosixProcess()
	{
		delete exitMonitorAdapter;
	}

	NativePosixProcess* NativePosixProcess::Create(
		PosixProcess* process)
	{
		NativePosixProcess* p = new NativePosixProcess(process);
		p->process = process;
		p->ForkAndExec();
		process->pid = p->pid;
		return p;
	}

	void NativePosixProcess::ForkAndExec()
	{
		this->pid = fork();
		if (this->pid < 0)
		{
			Logger* logger = Logger::Get("Process.NativePosixProcess");
			logger->Debug("Can't fork process :(");
			throw ValueException::FromFormat("Cannot fork process for %s", args->At(0)->ToString());
		}
		else if (pid == 0)
		{
			// setup redirection
			dup2(stdinPipe->GetReadHandle(), STDIN_FILENO);
			p->stdinPipe->Close();

			// outPipe and errPipe may be the same, so we dup first and close later
			dup2(stdoutPipe->GetWriteHandle(), STDOUT_FILENO);
			dup2(stderrPipe->GetWriteHandle(), STDERR_FILENO);
			stdoutPipe->Close();
			stderrPipe->Close();

			// close all open file descriptors other than stdin, stdout, stderr
			for (int i = 3; i < getdtablesize(); ++i)
				close(i);

			size_t i = 0;
			char** argv = new char*[process->args->Size() + 1];
			//argv[i++] = const_cast<char*>(command.c_str());
			for (;i < process->args->Size(); i++)
			{
				argv[i] = const_cast<char*>(process->args->At(i)->ToString());
			}
			argv[i] = NULL;

			SharedStringList envNames = process->environment->GetPropertyNames();
			for (i = 0; i < envNames->size(); i++)
			{
				setenv(envNames->at(i)->c_str(),
					environment->Get(envNames->at(i)->c_str())->ToString(), 1);
			}

			const char *command = process->args->At(0)->ToString();
			logger->Debug("execvp: %s", command);
			execvp(command, argv);
			_exit(72);
		}

		close(stdinPipe->GetReadHandle());
		close(stdoutPipe->GetWriteHandle());
		close(stderrPipe->GetWriteHandle());
	}

	NativePosixProcess::MonitorAsynchronously()
	{
		stdoutPipe->StartMonitor();
		stderrPipe->StartMonitor();

		this->exitCallback =
			StaticBoundMethod::FromMethod(this, NativePosixProcess::ExitCallback);
		this->exitMonitorThread.start(*exitMonitorAdapter);
	}

	std::string NativePosixProcess::MonitorSynchronously()
	{
		SharedKMethod readCallback =
			StaticBoundMethod::FromMethod(this, NativePosixProcess::ReadCallback);
		stdoutPipe->AddEventListener(Event::READ, readCallbcak);
		stderrPipe->AddEventListener(Event::READ, readCallbcak);

		stdoutPipe->StartMonitor();
		stderrPipe->StartMonitor();
		this->ExitMonitor();

		std::string output;
		for (size_t i = 0; i < processOutput.size(); i++)
		{
			output.append(processOutput.at(i)->Get());
		}

		return output;
	}

	void NativePosixProcess::ExitMonitor()
	{
		int status;
		int rc;
		do
		{
			rc = waitpid(this->pid, &status, 0);
		} while (rc < 0 && errno == EINTR);

		if (rc != this->pid)
		{
			throw ValueException::FromFormat("Cannot wait for process: %d", this->pid);
		}

		this->exitCode = WEXITSTATUS(status);

		stdoutPipe->Close();
		stderrPipe->Close();

		if (!exitCallback.isNull())
			InvokeMethodOnMainThread(exitCallback, ValueList());
	}

	void NativePosixProcess::ExitCallback(const ValueList& args, SharedValue result)
	{
		if (this->process)
			process->Exited(this);

		delete this;
	}

	void NativePosixProcess::ReadCallback(const ValueList& args, SharedValue result)
	{
		if (args.at(0)->IsObject())
		{
			SharedKObject data = args.GetObject(0)->GetObject("data")
			AutoBlob blob = data.cast<Blob>();
			if (!blob.isNull())
			{
				processOutput.push_back(blob);
			}
		}
	}

	void PosixProcess::Launch(bool async)
	{
		NativePosixProcess* nativeProcess = CreateNativePosixProcess();
		if (async)
		{
			nativeProcess->MonitorAsynchronously();

			Poco::Mutex::ScopedLock lock(nativeProcessesMutex);
			nativeProcesses.push_back(nativeProcess);
		}
		else
		{
			std::string output = nativeProcess->MonitorSynchronously();
			return output;
		}
	}

	int PosixProcess::GetPID()
	{
		return pid;
	}

	void PosixProcess::SendSignal(int signal)
	{
		Poco::Mutex::ScopedLock lock(nativeProcessesMutex);
		for (size_t = i; i < nativeProcesses.size(); i++)
		{
			NativePosixProcess* native = nativeProcesses.at(i);
			if (kill(native->pid, signal) != 0)
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
	}

	void PosixProcess::Kill()
	{
		SendSignal(SIGKILL);
	}

	void PosixProcess::Terminate()
	{
		SendSignal(SIGINT);
	}

	bool PosixProcess::IsRunning()
	{
		return nativeProcesses.size() > 0;
	}

	void PosixProcess::Exited(NativePosixProcess* native)
	{
		this->exitCode = native->exitCode;
		{
			Poco::Mutex::ScopedLock lock(nativeProcessesMutex);
			std::vector<NativePosixProcess*>::iterator i = nativeProcesses.begin();
			while (i != nativeProcesses.end())
			{
				NativePosixProcess* cNative = *i;
				if (cNative == native)
					i = nativeProcesses.erase(i);
				else
					i++;
			}
		}
		this->Exited();
	}

}
