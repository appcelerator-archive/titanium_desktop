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
#if defined(OS_OSX)
# include "osx/osx_process.h"
#elif defined(OS_WIN32)
# include "win32/win32_process.h"
#elif defined(OS_LINUX)
# include "linux/linux_process.h"
#endif

namespace ti
{
	/*static*/
	SharedProcess Process::GetCurrentProcess()
	{
#if defined(OS_OSX)
		return OSXProcess::GetCurrentProcess();
#elif defined(OS_WIN32)
		return Win32Process::GetCurrentProcess();
#elif defined(OS_LINUX)
		return LinuxProcess::GetCurrentProcess();
#endif
	}
	
	/*static*/
	SharedProcess Process::CreateProcess(
		SharedKList args, SharedKObject environment,
		SharedOutputPipe stdin, SharedInputPipe stdout, SharedInputPipe stderr)
	{
#if defined(OS_OSX)
		return new OSXProcess(args, environment, stdin, stdout, stderr);
#elif defined(OS_WIN32)
		return new Win32Process(args, environment, stdin, stdout, stderr);
#elif defined(OS_LINUX)
		return new LinuxProcess(args, environment, stdin, stdout, stderr);
#endif		
	}
	
	
	Process::Process() : StaticBoundObject("Process")
	{
		InitBindings();
	}
	
	Process::Process(SharedKList args, SharedKObject environment,
		SharedOutputPipe stdin, SharedInputPipe stdout, SharedInputPipe stderr) :
			StaticBoundObject("Process"),
			stdin(stdin), stdout(stdout), stderr(stderr),
			args(args), environment(environment), exitCode(-1),
			onExit(NULL)
	{
		if (environment.isNull())
		{
			this->environment = GetCurrentProcess()->CloneEnvironment();
		}
		
		if (stdin.isNull())
		{
			this->stdin = OutputPipe::CreateOutputPipe();
		}
		if (stdout.isNull())
		{
			this->stdout = InputPipe::CreateInputPipe();
		}
		if (stderr.isNull())
		{
			this->stderr = InputPipe::CreateInputPipe();
		}
		InitBindings();
	}
	
	void Process::InitBindings()
	{
		//TODO doc me
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
	}

	Process::~Process()
	{
	}

	void Process::Exited()
	{
		if (onExit != NULL && !onExit->isNull())
		{
			ValueList args(Value::NewInt(this->exitCode));
			Host::GetInstance()->InvokeMethodOnMainThread(*this->onExit, args, false);
		}
	}
	
	// convenience for joining stdout + stderr, and attaching to stdout
	void Process::SetOnRead(SharedKMethod method)
	{
		if (method.isNull())
		{
			stdout->SetOnRead(NULL);
			return;
		}
		
		if (!stderr->IsJoined())
		{
			stdout->Join(stderr);
		}

		stdout->SetOnRead(method);
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
		result->SetObject(this->args);
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
		Launch();
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
				SharedOutputPipe stdin;
				SharedInputPipe stdout, stderr;
				
				env = object->GetObject("env");
				stdin = object->GetObject("stdin").cast<OutputPipe>();
				stdout = object->GetObject("stdout").cast<InputPipe>();
				stderr = object->GetObject("stderr").cast<InputPipe>();
				Restart(env, stdin, stdout, stderr);
			}
		}
	}
	
	void Process::_SetOnRead(const ValueList& args, SharedValue result)
	{
		if (args.size() > 0 && args.at(0)->IsMethod())
		{
			SetOnRead(args.at(0)->ToMethod());
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
		result->SetObject(stdin);
	}
	
	void Process::_GetStdout(const ValueList& args, SharedValue result)
	{
		result->SetObject(stdout);
	}
	
	void Process::_GetStderr(const ValueList& args, SharedValue result)
	{
		result->SetObject(stderr);
	}
	
	SharedValue Process::Call(const ValueList& args)
	{
		Launch(false);
		// TODO join stdout/stderr, buffer contents, return
		return Value::Undefined;
	}
}

