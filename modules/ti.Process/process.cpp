/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#include <vector>
#include "process.h"
#include "pipe.h"
#include <Poco/Path.h>
#include <Poco/RunnableAdapter.h>
#include <Poco/ScopedLock.h>
#include <Poco/PipeImpl.h>

using Poco::RunnableAdapter;

namespace ti
{
	Process::Process(ProcessBinding* parent, std::string& cmd, std::vector<std::string>& args) :
		StaticBoundObject("Process"),
		running(false),
		complete(false),
		pid(-1),
		exitCode(-1),
		errp(0),
		outp(0),
		inp(0),
		logger(Logger::Get("Process.Process"))
	{
		this->parent = parent;
		this->errp = new Poco::Pipe();
		this->outp = new Poco::Pipe();
		this->inp = new Poco::Pipe();

		logger->Debug("Running process %s", cmd.c_str());
		try
		{
			this->arguments = args;
			this->command = cmd;
		}
		catch (std::exception &e)	
		{
			throw ValueException::FromString(e.what());
		}

		/**
		 * @tiapi(property=True,type=string,name=Process.Process.command,version=0.2)
		 * @tiapi The command used for the Process object
		 */
		this->SetString("command", cmd);

		/**
		 * @tiapi(property=True,type=integer,name=Process.Process.pid,version=0.2)
		 * @tiapi The process id of the Process object
		 */
		this->Set("pid",Value::NewInt(-1));

		/**
		 * @tiapi(property=True,type=boolean,name=Process.Process.running,version=0.2)
		 * @tiapi The running status of the Process object
		 */
		this->SetBool("running", false);

		/**
		 * @tiapi(property=True,type=object,name=Process.Process.err,version=0.2)
		 * @tiapi The Pipe object of the error stream
		 */
		this->err = new Pipe(new Poco::PipeInputStream(*errp));
		this->shared_error = new SharedKObject(this->err);
		this->SetObject("err", *shared_error);

		/**
		 * @tiapi(property=True,type=object,name=Process.Process.out,version=0.2)
		 * @tiapi The Pipe object of the output stream
		 */
		this->out = new Pipe(new Poco::PipeInputStream(*outp));
		this->shared_output = new SharedKObject(this->out);
		this->SetObject("out", *shared_output);

		/**
		 * @tiapi(property=True,type=object,name=Process.Process.in,version=0.2)
		 * @tiapi The Pipe object of the input stream
		 */
		this->in = new Pipe(new Poco::PipeOutputStream(*inp));
		this->shared_input = new SharedKObject(this->in);
		this->SetObject("in", *shared_input);

		/**
		 * @tiapi(method=True,name=Process.Process.terminate,version=0.2)
		 * @tiapi Terminates a running process
		 */
		this->SetMethod("terminate", &Process::Terminate);

		/**
		 * @tiapi(property=True,type=integer,name=Process.Process.exitCode,version=0.4)
		 * @tiapi The exit code or null if not yet exited
		 */
		this->SetNull("exitCode");

		/**
		 * @tiapi(property=True,type=method,name=Process.Process.onread,since=0.4)
		 * @tiapi The function handler to call when sys out is read
		 */
		this->SetNull("onread");

		/**
		 * @tiapi(property=True,type=method,name=Process.Process.onexit,since=0.4)
		 * @tiapi The function handler to call when the process exits
		 */
		this->SetNull("onexit");

		// setup threads which can read output and also monitor the exit
		this->monitorAdapter = new RunnableAdapter<Process>(*this, &Process::Monitor);
		this->exitMonitorThread.start(*monitorAdapter);
	}

	Process::~Process()
	{
		Terminate();

		if (this->exitMonitorThread.isRunning())
		{
			try
			{
				this->exitMonitorThread.join();
			}
			catch (Poco::Exception& e)
			{
				logger->Error(
					"Exception while try to join with exit monitor thread: %s",
					e.displayText().c_str());
			}
		}

		if (this->stdOutThread.isRunning())
		{
			try
			{
				this->stdOutThread.join();
			}
			catch (Poco::Exception& e)
			{
				logger->Error(
					"Exception while try to join with stdout thread: %s",
					e.displayText().c_str());
			}
		}

		if (this->stdErrorThread.isRunning())
		{
			try
			{
				this->stdErrorThread.join();
			}
			catch (Poco::Exception& e)
			{
				logger->Error(
					"Exception while try to join with stderr thread: %s",
					e.displayText().c_str());
			}
		}

		delete monitorAdapter;
		delete stdOutAdapter;
		delete stdErrorAdapter;
		delete shared_output;
		delete shared_input;
		delete shared_error;
	}

	void Process::StartReadThreads()
	{
		this->logger->Debug("Starting output handler threads...");
		if (!stdOutThread.isRunning())
		{
			this->stdOutAdapter = new RunnableAdapter<Process>(*this, &Process::ReadStdOut);
			stdOutThread.start(*stdOutAdapter);
		}

		if (!stdErrorThread.isRunning())
		{
			RunnableAdapter<Process> adapter(*this, &Process::ReadStdError);
			this->stdErrorAdapter = new RunnableAdapter<Process>(*this, &Process::ReadStdError);
			stdErrorThread.start(*stdErrorAdapter);
		}
	}

	void Process::Monitor()
	{
		this->Set("running", Value::NewBool(true));
		this->running = true;

		try
		{
			Poco::ProcessHandle ph = Poco::Process::launch(
				this->command, this->arguments,
				this->inp, this->outp, this->errp);
			this->StartReadThreads();
			this->pid = (int) ph.id();
			this->Set("pid", Value::NewInt(this->pid));

			this->exitCode = ph.wait();
			this->Set("exitCode", Value::NewInt(this->exitCode));
			logger->Debug("%s exited with return code %i", 
				this->command.c_str(), this->exitCode);
		}
		catch (Poco::SystemException &se)
		{
			logger->Error("System Exception starting: %s, message: %s",
				this->command.c_str(), se.what());
		}
		catch (std::exception &e)
		{
			logger->Error("Exception starting: %s, message: %s",
				this->command.c_str(), e.what());
		}

		this->Set("running", Value::NewBool(false));
		this->running = false;

		this->InvokeOnExitCallback();

		// We must set complete to true after we call the callback, so
		// that it is only called once -- see this->Set(...)
		this->complete = true;

		this->parent->Terminated(this);
	}

	void Process::InvokeOnExitCallback()
	{
		SharedValue sv = this->Get("onexit");
		if (!sv->IsMethod())
		{
			return;
		}

		ValueList args(Value::NewInt(this->exitCode));
		SharedKMethod callback = sv->ToMethod();
		this->parent->GetHost()->InvokeMethodOnMainThread(
			callback, args, false);
	}

	void Process::InvokeOnReadCallback(bool isStdError)
	{
		SharedValue sv = this->Get("onread");
		if (!sv->IsMethod())
		{
			return;
		}

		std::string output;
		if (isStdError)
		{
			Poco::ScopedLock<Poco::Mutex> lock(outputBufferMutex);
			output = stdErrorBuffer.str();
			stdErrorBuffer.str("");
		}
		else
		{
			Poco::ScopedLock<Poco::Mutex> lock(outputBufferMutex);
			output = stdOutBuffer.str();
			stdOutBuffer.str("");
		}

		if (!output.empty())
		{
			ValueList args(
				Value::NewString(output),
				Value::NewBool(isStdError));
			SharedKMethod callback = sv->ToMethod();
			this->parent->GetHost()->InvokeMethodOnMainThread(
				callback, args, false);
		}
	}

	void Process::ReadStdOut()
	{
		while (this->running)
		{
			SharedValue result = Value::NewUndefined();
			this->out->Read(ValueList(), result);

			if (result->IsString())
			{
				Poco::ScopedLock<Poco::Mutex> lock(outputBufferMutex);
				stdOutBuffer << result->ToString();
				this->InvokeOnReadCallback(false);
			}
		}
	}

	void Process::ReadStdError()
	{
		while (this->running)
		{
			SharedValue result = Value::NewUndefined();
			this->err->Read(ValueList(), result);

			if (result->IsString())
			{
				Poco::ScopedLock<Poco::Mutex> lock(outputBufferMutex);
				stdErrorBuffer << result->ToString();
				this->InvokeOnReadCallback(true);
			}
		}
	}

	void Process::Terminate(const ValueList& args, SharedValue result)
	{
		Terminate();
	}

	void Process::Terminate()
	{
		if (running)
		{
			this->running = false;
#ifdef OS_WIN32
			// win32 needs a kill to terminate process
			Poco::Process::kill(this->pid);
#else
			// this sends a more graceful SIGINT instead of SIGKILL
			// which is important for programs that manage child processes
			// and handle their own signals
			Poco::Process::requestTermination(this->pid);
#endif			
			this->Set("running", Value::NewBool(false));
			this->parent->Terminated(this);
		}
	}

	void Process::Set(const char *name, SharedValue value)
	{
		// We need to check the previous value of certain incomming values
		// *before* we actually do the Set(...) on this object.
		bool flushOnRead = 
			(!strcmp("onread", name)) && (!this->Get("onread")->IsMethod());
		bool flushOnExit = 
			(!strcmp("onexit", name)) && (!this->Get("onexit")->IsMethod());


		StaticBoundObject::Set(name, value);

		if (flushOnRead)
		{
			// If we had no previous onread callback flush our output
			// buffers, so that onread will be called even when it is
			// attached after a process finishes executing.
			this->InvokeOnReadCallback(false);
			this->InvokeOnReadCallback(true);
		}

		// this->complete is the signal that monitor thread has already
		// attempted to call the onexit callback. If it's false, the monitor
		// thread will take care of calling it.
		if (flushOnExit && this->complete)
		{
			this->InvokeOnExitCallback();
		}
	}
}

