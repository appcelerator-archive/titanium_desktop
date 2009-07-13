/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#include <kroll/kroll.h>
#include <Poco/Environment.h>
#include <Poco/Process.h>
#include "process_binding.h"
#include "process.h"
#include <signal.h>

namespace ti
{

	std::map<std::string,int> ProcessBinding::signals;
	std::vector<AutoProcess> ProcessBinding::processes;
	
	ProcessBinding::ProcessBinding() : AccessorBoundObject("Process")
	{
		Logger::Get("Process")->Debug("Initializing Titanium.Process");
		
		//TODO doc me
		SetMethod("createProcess", &ProcessBinding::CreateProcess);
		SetMethod("createInputPipe", &ProcessBinding::CreateInputPipe);
		SetMethod("createOutputPipe", &ProcessBinding::CreateOutputPipe);
		SetMethod("getCurrentProcess", &ProcessBinding::GetCurrentProcess);
		
#if defined(OS_OSX) || (OS_LINUX)
		signals["SIGHUP"] = SIGHUP;
		signals["SIGINT"] = SIGINT;
		signals["SIGQUIT"] = SIGQUIT;
		signals["SIGILL"] = SIGILL;
		signals["SIGTRAP"] = SIGTRAP;
		signals["SIGABRT"] = SIGABRT;
		signals["SIGFPE"] = SIGFPE;
		signals["SIGKILL"] = SIGKILL;
		signals["SIGBUS"] = SIGBUS;
		signals["SIGSEGV"] = SIGSEGV;
		signals["SIGSYS"] = SIGSYS;
		signals["SIGPIPE"] = SIGPIPE;
		signals["SIGALRM"] = SIGALRM;
		signals["SIGTERM"] = SIGTERM;
		signals["SIGURG"] = SIGURG;
		signals["SIGSTOP"] = SIGSTOP;
		signals["SIGTSTP"] = SIGTSTP;
		signals["SIGCHLD"] = SIGCHLD;
		signals["SIGTTIN"] = SIGTTIN;
		signals["SIGTTOU"] = SIGTTOU;
		signals["SIGIO"] = SIGIO;
		signals["SIGXCPU"] = SIGXCPU;
		signals["SIGXFSZ"] = SIGXFSZ;
		signals["SIGVTALRM"] = SIGVTALRM;
		signals["SIGPROF"] = SIGPROF;
		signals["SIGWINCH"] = SIGWINCH;
		signals["SIGUSR1"] = SIGUSR1;
		signals["SIGUSR2"] = SIGUSR2;
#elif defined(OS_WIN32)
		signals["SIGABRT"] = SIGABRT;
		signals["SIGFPE"] = SIGFPE;
		signals["SIGILL"] = SIGILL;
		signals["SIGINT"] = SIGINT;
		signals["SIGSEGV"] = SIGSEGV;
		signals["SIGTERM"] = SIGTERM;
#endif
#if defined(OS_OSX)
		signals["SIGEMT"] = SIGEMT;
		signals["SIGINFO"] = SIGINFO;
#endif
		std::map<std::string,int>::iterator iter;
		for (iter = signals.begin(); iter != signals.end(); iter++)
		{
			Set(iter->first.c_str(), Value::NewInt(iter->second));
		}
		
		Logger::Get("Process")->Debug("returning");
	}
	
	ProcessBinding::~ProcessBinding()
	{
	}
	
	void ProcessBinding::CreateProcess(const ValueList& args, SharedValue result)
	{
		SharedKList argList = 0;
		SharedKObject environment = 0;
		AutoOutputPipe stdinPipe = 0;
		AutoInputPipe stdoutPipe, stderrPipe = 0;
		
		if (args.size() > 0 && args.at(0)->IsObject())
		{
			SharedKObject options = args.at(0)->ToObject();
			if (options->Get("args")->IsUndefined())
			{
				throw ValueException::FromString("Titanium.Process option argument 'args' was undefined");
			}
			if (!options->Get("args")->IsList())
			{
				throw ValueException::FromString("Titanium.Process option 'args' must be an array");
			}
			
			argList = options->Get("args")->ToList();
			if (!options->Get("env")->IsUndefined() && options->Get("env")->IsList())
			{
				environment = options->Get("env")->ToList();
			}
			if (!options->Get("stdin")->IsUndefined() && options->Get("stdin")->IsObject())
			{
				stdinPipe = options->Get("stdin")->ToObject().cast<OutputPipe>();
			}
			if (!options->Get("stdout")->IsUndefined() && options->Get("stdout")->IsObject())
			{
				stdoutPipe = options->Get("stdin")->ToObject().cast<InputPipe>();
			}
			if (!options->Get("stderr")->IsUndefined() && options->Get("stderr")->IsObject())
			{
				stderrPipe = options->Get("stderr")->ToObject().cast<InputPipe>();
			}
		}
		else if (args.size() > 0 && args.at(0)->IsList())
		{
			argList = args.at(0)->ToList();
			if (args.size() > 1 && args.at(1)->IsObject())
			{
				environment = args.at(1)->ToObject();
			}
			if (args.size() > 2 && args.at(2)->IsObject())
			{
				stdinPipe = args.at(2)->ToObject().cast<OutputPipe>();
			}
			if (args.size() > 3 && args.at(3)->IsObject())
			{
				stdoutPipe = args.at(3)->ToObject().cast<InputPipe>();
			}
			if (args.size() > 4 && args.at(4)->IsObject())
			{
				stderrPipe = args.at(4)->ToObject().cast<InputPipe>();
			}
		}
		
		if (argList.isNull())
		{
			throw ValueException::FromString("Titanium.Process option argument 'args' was undefined");
		}
		if (argList->Size() == 0)
		{
			throw ValueException::FromString("Titanium.Process option argument 'args' must have at least 1 element");
		}
		// Clone args
		SharedKList argsClone = new StaticBoundList();
		for (size_t i = 0; i < argList->Size(); i++)
		{
			SharedValue arg = Value::Undefined;
			if (!argList->At(i)->IsString())
			{
				if (argList->At(i)->IsObject() && argList->At(i)->ToObject()->Get("toString") != Value::Undefined)
				{
					arg = argList->At(i)->ToObject()->CallNS("toString");
				}
			}
			else {
				arg = argList->At(i);
			}
			if (arg->IsUndefined()) {
				throw ValueException::FromString("Titanium.Process argument was not a String or Object with toString");
			}
			
			argsClone->Append(arg);
		}
		
		AutoProcess process = Process::CreateProcess(argsClone, environment, stdinPipe, stdoutPipe, stderrPipe);
		processes.push_back(process);
		result->SetObject(process);
	}
	
	void ProcessBinding::ProcessTerminated(AutoProcess process)
	{
		std::vector<AutoProcess>::iterator found =
			std::find(processes.begin(), processes.end(), process);
		
		if (found != processes.end())
		{
			processes.erase(found);
		}
	}
	
	void ProcessBinding::CreateInputPipe(const ValueList& args, SharedValue result)
	{
		result->SetObject(InputPipe::CreateInputPipe());
	}
	
	void ProcessBinding::CreateOutputPipe(const ValueList& args, SharedValue result)
	{
		result->SetObject(OutputPipe::CreateOutputPipe());
	}
	
	void ProcessBinding::GetCurrentProcess(const ValueList& args, SharedValue result)
	{
		result->SetObject(Process::GetCurrentProcess());
	}
}
