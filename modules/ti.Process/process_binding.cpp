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
		
	ProcessBinding::ProcessBinding()
	{
		Logger::Get("Process")->Debug("Initializing Titanium.Process");
		
		//TODO doc me
		SetMethod("createProcess", &ProcessBinding::CreateProcess);
		SetMethod("createInputPipe", &ProcessBinding::CreateInputPipe);
		SetMethod("createOutputPipe", &ProcessBinding::CreateOutputPipe);
		SetMethod("getCurrentProcess", &ProcessBinding::GetCurrentProcess);
		
#if defined(OS_OSX) || (OS_LINUX)
		Logger::Get("Process")->Debug("Setting up signals..");
		signals["SIGHUP"] = SIGHUP;
		signals["SIGINT"] = SIGINT;
		signals["SIGQUIT"] = SIGQUIT;
		signals["SIGILL"] = SIGILL;
		signals["SIGTRAP"] = SIGTRAP;
		signals["SIGABRT"] = SIGABRT;
		signals["SIGEMT"] = SIGEMT;
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
		signals["SIGINFO"] = SIGINFO;
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
		Logger::Get("Process")->Debug("Binding signals..");
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
		SharedKList argList = NULL;
		SharedKObject environment = NULL;
		SharedOutputPipe stdin = NULL;
		SharedInputPipe stdout, stderr = NULL;
		
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
				stdin = options->Get("stdin")->ToObject().cast<OutputPipe>();
			}
			if (!options->Get("stdout")->IsUndefined() && options->Get("stdout")->IsObject())
			{
				stdout = options->Get("stdin")->ToObject().cast<InputPipe>();
			}
			if (!options->Get("stderr")->IsUndefined() && options->Get("stderr")->IsObject())
			{
				stderr = options->Get("stderr")->ToObject().cast<InputPipe>();
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
				stdin = args.at(2)->ToObject().cast<OutputPipe>();
			}
			if (args.size() > 3 && args.at(3)->IsObject())
			{
				stdout = args.at(3)->ToObject().cast<InputPipe>();
			}
			if (args.size() > 4 && args.at(4)->IsObject())
			{
				stderr = args.at(4)->ToObject().cast<InputPipe>();
			}
		}
		
		if (argList.isNull())
		{
			throw ValueException::FromString("Titanium.Process option argument 'args' was undefined");
		}
		if (argList->Size() == 0)
		{
			throw ValueException::FromString("TItanium.Process option argument 'args' must have at least 1 element");
		}
		
		// Clone args
		SharedKList argsClone = new StaticBoundList();
		for (int i = 0; i < argList->Size(); i++)
		{
			argsClone->Append(Value::NewString(argList->At(i)->ToString()));
		}
		
		result->SetObject(Process::CreateProcess(argsClone, environment, stdin, stdout, stderr));
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
