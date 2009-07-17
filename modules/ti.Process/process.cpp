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
# include "win32/win32_process.h"
#else
# include "posix/posix_process.h"
#endif

namespace ti
{
	/*static*/
	AutoProcess Process::GetCurrentProcess()
	{
#if defined(OS_WIN32)
		return Win32Process::GetCurrentProcess();
#else
		return PosixProcess::GetCurrentProcess();
#endif
	}
	
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
		AccessorBoundMethod(NULL, "Process.Process"),
		stdoutPipe(new Pipe()),
		stderrPipe(new Pipe()),
		stdinPipe(new Pipe()),
		environment(GetCurrentProcess()->CloneEnvironment()),
		exitCode(-1),
		onExit(0)
	{
		SetMethod("getPID", &Process::_GetPID);
		SetMethod("getExitCode", &Process::_GetExitCode);
		SetMethod("getArguments", &Process::_GetArguments);
		SetMethod("getEnvironment", &Process::_GetEnvironment);
		SetMethod("setEnvironment", &Process::_SetEnvironment);
		SetMethod("cloneEnvironment", &Process::_CloneEnvironment);
		SetMethod("launch", &Process::_Launch);
		SetMethod("terminate", &Process::_Terminate);
		SetMethod("kill", &Process::_Kill);
		SetMethod("sendSignal", &Process::_SendSignal);
		SetMethod("restart", &Process::_Restart);
		SetMethod("setOnRead", &Process::_SetOnRead);
		SetMethod("setOnExit", &Process::_SetOnExit);
		SetMethod("getStdin", &Process::_GetStdin);
		SetMethod("getStdout", &Process::_GetStdout);
		SetMethod("getStderr", &Process::_GetStderr);
		SetMethod("isRunning", &Process::_IsRunning);
		SetMethod("toString", &Process::_ToString);
	}

	Process::~Process()
	{
		delete exitMonitorAdapter;
	}

	void Process::Exited()
	{
		if (onExit != NULL && !onExit->isNull())
		{
			ValueList args(Value::NewInt(this->exitCode));
			try
			{
				Host::GetInstance()->InvokeMethodOnMainThread(*this->onExit, args, false);
			}
			catch (ValueException &e)
			{
				Logger::Get("Process")->Error(e.DisplayString()->c_str());
			}
		}
		
		this->duplicate();
		AutoPtr<Process> autoThis = this;
		ProcessBinding::ProcessTerminated(autoThis);
	}
	
	// convenience for joining stdout + stderr, and attaching to stdout
	void Process::SetOnRead(SharedKMethod method)
	{
		if (method.isNull())
		{
			//stdoutPipe->SetOnRead(NULL);
			//stderrPipe->SetOnRead(NULL);
			return;
		}
		
		//stdoutPipe->SetOnRead(method);
		//stderrPipe->SetOnRead(method);
	}
	
	SharedKObject Process::CloneEnvironment()
	{
		SharedStringList properties = environment->GetPropertyNames();
		SharedKObject clonedEnvironment = new StaticBoundObject();
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
	
	void Process::LaunchAsync()
	{
		ForkAndExec();
		MonitorAsync();
		
		this->exitCallback = StaticBoundMethod::FromMethod<Process>(
			this, &Process::ExitCallback);
		this->exitMonitorThread.start(*exitMonitorAdapter);
	}

	std::string Process::LaunchSync()
	{
		ForkAndExec();
		std::string output = MonitorSync();
		return output;
	}
	
	void Process::ExitMonitor()
	{
		this->exitCode = this->Wait();
		if (!exitCallback.isNull())
			Host::GetInstance()->InvokeMethodOnMainThread(exitCallback, ValueList());
	}
	
	void Process::ExitCallback(const ValueList& args, SharedValue result)
	{
		this->Exited();
		delete this;
	}
	
	void Process::Restart()
	{
		Restart(NULL, NULL, NULL, NULL);
	}
	
	void Process::Restart(SharedKObject environment, AutoPipe stdinPipe, AutoPipe stdoutPipe, AutoPipe stderrPipe)
	{
		this->environment = environment.isNull() ? CloneEnvironment() : environment;

		this->stdinPipe = stdinPipe;
		this->stdoutPipe = stdoutPipe;
		this->stderrPipe = stderrPipe;
		if (stdinPipe.isNull())
			this->stdinPipe = new Pipe();
		if (this->stdoutPipe.isNull())
			this->stdoutPipe = new Pipe();
		if (this->stderrPipe.isNull())
			this->stderrPipe = new Pipe();
	
		if (IsRunning())
		{
			Terminate();
		}

		this->duplicate();
		AutoPtr<Process> autoThis = this;
		ProcessBinding::AddProcess(autoThis);
	
		Logger::Get("Process.Process")->Debug("restarting...");
		LaunchAsync();
	}
	
	void Process::_GetPID(const ValueList& args, SharedValue result)
	{
		result->SetInt(GetPID());
	}
	
	void Process::_GetExitCode(const ValueList& args, SharedValue result)
	{
		result->SetInt(exitCode);
	}
	
	void Process::_GetArguments(const ValueList& args, SharedValue result)
	{
		result->SetList(this->args);
	}
	
	void Process::_GetEnvironment(const ValueList& args, SharedValue result)
	{
		if (args.size() > 0 && args.at(0)->IsString())
		{
			SharedValue value = environment->Get(args.at(0)->ToString());
			result->SetValue(value);
		}
		else {
			result->SetObject(environment);
		}
	}
	
	void Process::_SetEnvironment(const ValueList& args, SharedValue result)
	{
		if (args.size() >= 2 && args.at(0)->IsString() && args.at(1)->IsString())
		{
			SetEnvironment(args.at(0)->ToString(), args.at(1)->ToString());
		}
	}
	
	void Process::_CloneEnvironment(const ValueList& args, SharedValue result)
	{
		result->SetObject(CloneEnvironment());
	}
	
	void Process::_Launch(const ValueList& args, SharedValue result)
	{
		LaunchAsync();
	}
	
	void Process::_Terminate(const ValueList& args, SharedValue result)
	{
		Terminate();
	}
	
	void Process::_Kill(const ValueList& args, SharedValue result)
	{
		Kill();
	}
	
	void Process::_SendSignal(const ValueList& args, SharedValue result)
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
	
	void Process::_Restart(const ValueList& args, SharedValue result)
	{
		if (args.size() == 0)
		{
			Restart();
		}
		else
		{
			if (args.at(0)->IsObject())
			{
				SharedKObject object = args.at(0)->ToObject();
				SharedKObject env;
				AutoPipe stdinPipe;
				AutoPipe stdoutPipe, stderrPipe;
				
				env = object->GetObject("env");
				stdinPipe = object->GetObject("stdin").cast<Pipe>();
				stdoutPipe = object->GetObject("stdout").cast<Pipe>();
				stderrPipe = object->GetObject("stderr").cast<Pipe>();
				Restart(env, stdinPipe, stdoutPipe, stderrPipe);
			}
		}
	}
	
	void Process::_SetOnRead(const ValueList& args, SharedValue result)
	{
		if (args.size() > 0 && args.at(0)->IsMethod())
		{
			//SetOnRead(args.at(0)->ToMethod());
		}
	}
	
	void Process::_SetOnExit(const ValueList& args, SharedValue result)
	{
		if (args.size() > 0 && args.at(0)->IsMethod())
		{
			this->onExit = new SharedKMethod(args.at(0)->ToMethod());
		}
	}
	
	void Process::_GetStdin(const ValueList& args, SharedValue result)
	{
		result->SetObject(stdinPipe);
	}
	
	void Process::_GetStdout(const ValueList& args, SharedValue result)
	{
		result->SetObject(stdoutPipe);
	}
	
	void Process::_GetStderr(const ValueList& args, SharedValue result)
	{
		result->SetObject(stderrPipe);
	}
	
	void Process::_IsRunning(const ValueList& args, SharedValue result)
	{
		result->SetBool(IsRunning());
	}

	void Process::Call(const ValueList& args, SharedValue result)
	{
		// Should this return an AutoBlob instead?
		std::string output = LaunchSync();
		result->SetString(output);
	}
	
	void Process::_ToString(const ValueList& args, SharedValue result)
	{
		result->SetString(ArgumentsToString().c_str());
	}
}

