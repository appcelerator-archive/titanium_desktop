/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#include <vector>
#include "process.h"
#include "pipe.h"
#include "process_binding.h"
#if defined(OS_WIN32)
#include "win32/win32_process.h"
#else
#include "posix/posix_process.h"
#endif
using Poco::RunnableAdapter;

namespace ti
{
	/*static*/
	AutoProcess Process::CreateProcess()
	{
#if defined(OS_WIN32)
		AutoProcess process = new Win32Process();
#else
		AutoProcess process = new PosixProcess();
#endif
		return process;
	}

	Process::Process() :
		KEventMethod("Process.Process"),
		stdoutPipe(new Pipe()),
		stderrPipe(new Pipe()),
		stdinPipe(new Pipe()),
		environment(GetCurrentEnvironment()),
		pid(-1),
		exitCode(Value::Null),
		onRead(0),
		onExit(0),
		exitMonitorAdapter(new RunnableAdapter<Process>(
			*this, &Process::ExitMonitorAsync)),
		running(false)
	{
		/**
		 * @tiapi(method=True,name=Process.Process.getPID,since=0.5)
		 * @tiresult[Number] The PID of this process
		 */
		SetMethod("getPID", &Process::_GetPID);
		
		/**
		 * @tiapi(method=True,name=Process.Process.getExitCode,since=0.5)
		 * @tiresult[Number] The exit code of this process. If the process is still running, this will return -1
		 */
		SetMethod("getExitCode", &Process::_GetExitCode);
		
		/**
		 * @tiapi(method=True,name=Process.Process.getArguments,since=0.5)
		 * @tiresult[Array<String>] The list of arguments this process was created with
		 */
		SetMethod("getArguments", &Process::_GetArguments);
		
		/**
		 * @tiapi(method=True,name=Process.Process.getEnvironment,since=0.5)
		 * @tiarg[String, key] an environment key
		 * @tiresult[String|Object] Either the string value of the given key or the entire environment object
		 */
		SetMethod("getEnvironment", &Process::_GetEnvironment);
		
		/**
		 * @tiapi(method=True,name=Process.Process.setEnvironment,since=0.5) Set an environment variable for this process
		 * @tiarg[String, key] an environment key
		 * @tiarg[String, value] the value
		 */
		SetMethod("setEnvironment", &Process::_SetEnvironment);
		
		/**
		 * @tiapi(method=True,name=Process.Process.cloneEnvironment,since=0.5)
		 * @tiresult[Object, environment] A clone of this process' environment
		 */
		SetMethod("cloneEnvironment", &Process::_CloneEnvironment);
		
		/**
		 * @tiapi(method=True,name=Process.Process.launch,since=0.5)
		 * @tiapi Launch this process asynchronously (not waiting for it's exit)
		 */
		SetMethod("launch", &Process::_Launch);
		
		/**
		 * @tiapi(method=True,name=Process.Process.terminate,since=0.5)
		 * @tiapi Terminate this process (SIGTERM in Unix, TerminateProcess in Windows)
		 */
		SetMethod("terminate", &Process::_Terminate);
		
		/**
		 * @tiapi(method=True,name=Process.Process.kill,since=0.5)
		 * @tiapi Kill this process (SIGINT in Unix, TerminateProcess in Windows)
		 */
		SetMethod("kill", &Process::_Kill);
		
		/**
		 * @tiapi(method=True,name=Process.Process.sendSignal,since=0.5)
		 * @tiapi Send a signal (e.g. Process.SIGHUP) to this Process
		 * @tiapi NOTE: this method does nothing in Windows
		 * @tiarg[Number|String, signal] The name of the signal to send.
		 */
		SetMethod("sendSignal", &Process::_SendSignal);
		
		/**
		 * @tiapi(method=True,name=Process.Process.setOnRead,since=0.5)
		 * @tiapi Set an onRead event handler for this process stdout and stderr
		 * @tiarg[Function, onRead] a handler that is passed an event, with a "data" Blob full of data read from the pipe
		 */
		SetMethod("setOnRead", &Process::_SetOnRead);
		
		/**
		 * @tiapi(method=True,name=Process.Process.setOnExit,since=0.5)
		 * @tiapi Set an onExit event handler for this process
		 * @tiarg[Function, onExit] a function
		 */
		SetMethod("setOnExit", &Process::_SetOnExit);
		
		/**
		 * @tiapi(method=True,name=Process.Process.getStdin,since=0.5)
		 * @tiresult[Process.Pipe] the standard input pipe for this process
		 */
		SetMethod("getStdin", &Process::_GetStdin);
		
		/**
		 * @tiapi(method=True,name=Process.Process.getStdout,since=0.5)
		 * @tiresult[Process.Pipe] the standard output pipe for this process
		 */
		SetMethod("getStdout", &Process::_GetStdout);
		
		/**
		 * @tiapi(method=True,name=Process.Process.getStderr,since=0.5)
		 * @tiresult[Process.Pipe] the standard error pipe for this process
		 */
		SetMethod("getStderr", &Process::_GetStderr);
		
		/**
		 * @tiapi(method=True,name=Process.Process.isRunning,since=0.5)
		 * @tiresult[Boolean] whether or not this process is actively running
		 */
		SetMethod("isRunning", &Process::_IsRunning);
		
		/**
		 * @tiapi(method=True,name=Process.Process.toString,since=0.5)
		 * @tiresult[String] the arguments of this process as a string
		 */
		SetMethod("toString", &Process::_ToString);
	}

	Process::~Process()
	{
		delete exitMonitorAdapter;
	}

	void Process::Exited(bool async)
	{
		this->GetNativeStdout()->Close();
		this->GetNativeStderr()->Close();

		this->DetachPipes();
		this->RecreateNativePipes();
		this->running = false;

		if (async)
		{
			this->duplicate();
			AutoPtr<Event> event = new Event(this, Event::EXIT);
			Pipe::FireEventAsynchronously(event);
		}
		else
		{
			this->FireEvent(Event::EXIT);
		}
	}

	void Process::SetOnRead(KMethodRef newOnRead)
	{
		if (running)
		{
			this->GetNativeStdout()->AddEventListener(Event::READ, newOnRead);
			this->GetNativeStderr()->AddEventListener(Event::READ, newOnRead);
			this->GetNativeStdout()->RemoveEventListener(Event::READ, onRead);
			this->GetNativeStderr()->RemoveEventListener(Event::READ, onRead);
		}
		this->onRead = newOnRead;
	}

	void Process::SetOnExit(KMethodRef newOnExit)
	{
		this->AddEventListener(Event::EXIT, newOnExit);
		if (!this->onExit.isNull())
			this->RemoveEventListener(Event::EXIT, this->onExit);
		this->onExit = newOnExit;
	}

	KObjectRef Process::CloneEnvironment()
	{
		SharedStringList properties = environment->GetPropertyNames();
		KObjectRef clonedEnvironment = new StaticBoundObject();
		for (size_t i = 0; i < properties->size(); i++)
		{
			std::string property = *properties->at(i);
			std::string value = environment->Get(property.c_str())->ToString();
			clonedEnvironment->Set(property.c_str(), Value::NewString(value.c_str()));
		}
		return clonedEnvironment;
	}

	std::string Process::ArgumentsToString()
	{
		std::ostringstream str;
		for (size_t i = 0; i < this->args->Size(); i++)
		{
			str << " \"" << this->args->At(i)->ToString() << "\" ";
		}
		return str.str();
	}

	void Process::AttachPipes(bool async)
	{
		this->GetNativeStdout()->Attach(stdoutPipe);
		this->GetNativeStderr()->Attach(stderrPipe);

		if (async && !onRead.isNull())
		{
			this->GetNativeStdout()->AddEventListener(Event::READ, onRead);
			this->GetNativeStderr()->AddEventListener(Event::READ, onRead);
		}
	}

	void Process::DetachPipes()
	{
		// We don't detach event listeners because we want any pending
		// READ events to fire. It should be okay though, because
		// native pipes should not be re-used.
		stdinPipe->Detach(this->GetNativeStdin());
		this->GetNativeStdout()->Detach(stdoutPipe);
		this->GetNativeStderr()->Detach(stderrPipe);
	}

	void Process::LaunchAsync()
	{
		this->running = true;
		this->exitCode = Value::Null;

		this->AttachPipes(true);
		ForkAndExec();
		MonitorAsync();

		this->exitCallback = StaticBoundMethod::FromMethod<Process>(
			this, &Process::ExitCallback);
		this->exitMonitorThread.start(*exitMonitorAdapter);
	}

	BlobRef Process::LaunchSync()
	{
		this->running = true;
		this->exitCode = Value::Null;

		this->AttachPipes(false);
		ForkAndExec();
		BlobRef output = MonitorSync();

		// Manually fire read events here so that
		// we can be precise about the ordering.
		if (!onRead.isNull())
		{
			this->GetNativeStdout()->AddEventListener(Event::READ, onRead);
			AutoPtr<Event> event = new ReadEvent(this->GetNativeStdin(), output);
			this->GetNativeStdout()->FireEvent(event);
			this->GetNativeStdout()->RemoveEventListener(Event::READ, onRead);
		}

		this->Exited(false);
		return output;
	}

	void Process::ExitMonitorSync()
	{
		this->exitCode = Value::NewInt(this->Wait());

		this->GetNativeStdin()->StopMonitors();
		this->GetNativeStdout()->StopMonitors();
		this->GetNativeStderr()->StopMonitors();
		if (!exitCallback.isNull())
			Host::GetInstance()->InvokeMethodOnMainThread(exitCallback, ValueList());
	}

	void Process::ExitMonitorAsync()
	{
		this->exitCode = Value::NewInt(this->Wait());

		// We want the onRead callbacks to fire before the exit
		// event, so we do a little hack here and stop the event
		// threads on the native pipes. It shouldn't matter anyhow
		// because these pipes are now dead and will be replaced on
		// next launch. Don't do this for synchronous process
		// launch becauase we are already on the main thread and that
		// will cause a deadlock.
		this->GetNativeStdin()->StopMonitors();
		this->GetNativeStdout()->StopMonitors();
		this->GetNativeStderr()->StopMonitors();

		if (!exitCallback.isNull())
			Host::GetInstance()->InvokeMethodOnMainThread(exitCallback, ValueList());
	}

	void Process::ExitCallback(const ValueList& args, KValueRef result)
	{
		this->Exited(true);
	}

	void Process::_GetPID(const ValueList& args, KValueRef result)
	{
		int pid = GetPID();
		if (pid != -1)
			result->SetInt(GetPID());
		else
			result->SetNull();
	}

	void Process::_GetExitCode(const ValueList& args, KValueRef result)
	{
		result->SetValue(exitCode);
	}
	
	void Process::_GetArguments(const ValueList& args, KValueRef result)
	{
		result->SetList(this->args);
	}
	
	void Process::_GetEnvironment(const ValueList& args, KValueRef result)
	{
		if (args.size() > 0 && args.at(0)->IsString())
		{
			KValueRef value = environment->Get(args.at(0)->ToString());
			result->SetValue(value);
		}
		else {
			result->SetObject(environment);
		}
	}
	
	void Process::_SetEnvironment(const ValueList& args, KValueRef result)
	{
		if (args.size() >= 2 && args.at(0)->IsString() && args.at(1)->IsString())
		{
			SetEnvironment(args.at(0)->ToString(), args.at(1)->ToString());
		}
	}
	
	void Process::_CloneEnvironment(const ValueList& args, KValueRef result)
	{
		result->SetObject(CloneEnvironment());
	}
	
	void Process::_Launch(const ValueList& args, KValueRef result)
	{
		LaunchAsync();
	}
	
	void Process::_Terminate(const ValueList& args, KValueRef result)
	{
		Terminate();
	}
	
	void Process::_Kill(const ValueList& args, KValueRef result)
	{
		Kill();
	}
	
	void Process::_SendSignal(const ValueList& args, KValueRef result)
	{
		if (args.size() >= 1 && args.at(0)->IsNumber())
		{
			int code = -1;
			if (args.at(0)->IsString()) {
				std::string signalName = args.at(0)->ToString();
				if (ProcessBinding::signals.find(signalName) != ProcessBinding::signals.end())
				{
					code = ProcessBinding::signals[signalName];
				}
				else
				{
					std::ostringstream str;
					str << "Error, signal name: \"" << signalName << "\" is unrecognized";
					throw ValueException::FromString(str.str());
				}
			}
			else if (args.at(0)->IsNumber())
			{
				code = args.at(0)->ToInt();
				
				bool found = false;
				for (std::map<std::string,int>::const_iterator iter = ProcessBinding::signals.begin();
					iter != ProcessBinding::signals.end();
					iter++)
				{
					if (iter->second == code)
					{
						found = true;
						break;
					}
				}
				if (!found) {
					std::ostringstream str;
					str << "Error, signal number: " << code << " is unrecognized";
					throw ValueException::FromString(str.str());
				}
			}
			
			
			SendSignal(args.at(0)->ToInt());
		}
	}

	void Process::_SetOnRead(const ValueList& args, KValueRef result)
	{
		args.VerifyException("setOnRead", "m");
		this->SetOnRead(args.GetMethod(0));
	}

	void Process::_SetOnExit(const ValueList& args, KValueRef result)
	{
		args.VerifyException("setOnExit", "m");
		this->SetOnExit(args.GetMethod(0));
	}

	void Process::_GetStdin(const ValueList& args, KValueRef result)
	{
		result->SetObject(stdinPipe);
	}
	
	void Process::_GetStdout(const ValueList& args, KValueRef result)
	{
		result->SetObject(stdoutPipe);
	}
	
	void Process::_GetStderr(const ValueList& args, KValueRef result)
	{
		result->SetObject(stderrPipe);
	}
	
	void Process::_IsRunning(const ValueList& args, KValueRef result)
	{
		result->SetBool(running);
	}

	KValueRef Process::Call(const ValueList& args)
	{
		BlobRef output = LaunchSync();
		return Value::NewObject(output);
	}

	void Process::_ToString(const ValueList& args, KValueRef result)
	{
		result->SetString(ArgumentsToString().c_str());
	}

	/*static*/
	KObjectRef Process::GetCurrentEnvironment()
	{
		KObjectRef kenv = new StaticBoundObject();
		std::map<std::string, std::string> env = EnvironmentUtils::GetEnvironment();

		std::map<std::string, std::string>::iterator i = env.begin();
		while (i != env.end())
		{
			kenv->SetString(i->first.c_str(), i->second.c_str());
			i++;
		}
		return kenv;
	}

	void Process::SetStdin(AutoPipe newStdin)
	{
		if (running)
		{
			newStdin->Attach(this->GetNativeStdin());
			this->stdinPipe->Detach(this->GetNativeStdin());
		}
		this->stdinPipe = stdinPipe;
	}

	void Process::SetStdout(AutoPipe newStdout)
	{
		if (running)
		{
			this->GetNativeStdout()->Attach(newStdout);
			this->GetNativeStdout()->Detach(this->stdoutPipe);
		}
		this->stdoutPipe = newStdout;
	}

	void Process::SetStderr(AutoPipe newStderr)
	{
		if (running)
		{
			this->GetNativeStderr()->Attach(newStderr);
			this->GetNativeStderr()->Detach(this->stderrPipe);
		}
		this->stderrPipe = newStderr;
	}
}

