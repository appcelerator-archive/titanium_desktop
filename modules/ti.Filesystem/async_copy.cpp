/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#include "async_copy.h"
#include "filesystem_binding.h"
#include <kroll/thread_manager.h>
#include <iostream>
#include <sstream>

#ifndef OS_WIN32
#include <unistd.h>
#include <string.h>
#include <errno.h>
#endif

namespace ti
{
	AsyncCopy::AsyncCopy(FilesystemBinding* parent, Host *host,
		std::vector<std::string> files, std::string destination, KMethodRef callback) :
			StaticBoundObject("Filesystem.AsyncCopy"),
			parent(parent),
			host(host),
			files(files),
			destination(destination),
			callback(callback),
			stopped(false)
	{
		this->Set("running",Value::NewBool(true));
		this->thread = new Poco::Thread();
		this->thread->start(&AsyncCopy::Run,this);
	}

	AsyncCopy::~AsyncCopy()
	{
		if (this->thread!=NULL)
		{
			this->thread->tryJoin(10); // precaution, should already be stopped
			delete this->thread;
			this->thread = NULL;
		}
	}

	void AsyncCopy::Copy(Poco::Path &src, Poco::Path &dest)
	{
		Logger* logger = Logger::Get("Filesystem.AsyncCopy");
		std::string srcString = src.toString();
		std::string destString = dest.toString();
		Poco::File from(srcString);
		bool isLink = from.isLink();

		logger->Debug("file=%s dest=%s link=%i", srcString.c_str(), destString.c_str(), isLink);
#ifndef OS_WIN32
		if (isLink)
		{
			char linkPath[PATH_MAX];
			ssize_t length = readlink(from.path().c_str(), linkPath, PATH_MAX);
			linkPath[length] = '\0';

			std::string newPath (dest.toString());
			const char *destPath = newPath.c_str();
			unlink(destPath); // unlink it first, fails in some OS if already there
			int result = symlink(linkPath, destPath);

			if (result == -1)
			{
				std::string err = "Copy failed: Could not make symlink (";
				err.append(destPath);
				err.append(") from ");
				err.append(linkPath);
				err.append(" : ");
				err.append(strerror(errno));
				throw kroll::ValueException::FromString(err);
			}
		}
#endif
		if (!isLink && from.isDirectory())
		{
			Poco::File d(dest.toString());
			if (!d.exists())
			{
				d.createDirectories();
			}
			std::vector<std::string> files;
			from.list(files);
			std::vector<std::string>::iterator i = files.begin();
			while(i!=files.end())
			{
				std::string fn = (*i++);
				Poco::Path sp(kroll::FileUtils::Join(src.toString().c_str(),fn.c_str(),NULL));
				Poco::Path dp(kroll::FileUtils::Join(dest.toString().c_str(),fn.c_str(),NULL));
				this->Copy(sp,dp);
			}
		}
		else if (!isLink)
		{
			// in this case it's a regular file
			Poco::File s(src.toString());
			s.copyTo(dest.toString().c_str());
		}
	}

	void AsyncCopy::Run(void* data)
	{
		START_KROLL_THREAD;

		Logger* logger = Logger::Get("Filesystem.AsyncCopy");

		AsyncCopy* ac = static_cast<AsyncCopy*>(data);
		std::vector<std::string>::iterator iter = ac->files.begin();
		Poco::Path to(ac->destination);
		Poco::File tof(to.toString());

		logger->Debug("Job started: dest=%s, count=%i", ac->destination.c_str(), ac->files.size());
		if (!tof.exists())
		{
			tof.createDirectory();
		}
		int c = 0;
		while (!ac->stopped && iter!=ac->files.end())
		{
			std::string file = (*iter++);
			c++;

			logger->Debug("File: path=%s, count=%i\n", file.c_str(), c);
			try
			{
				Poco::Path from(file);
				Poco::File f(file);
				if (f.isDirectory())
				{
					ac->Copy(from,to);
				}
				else
				{
					Poco::Path dest(to,from.getFileName());
					ac->Copy(from,dest);
				}
				logger->Debug("File copied");

				KValueRef value = Value::NewString(file);
				ValueList args;
				args.push_back(value);
				args.push_back(Value::NewInt(c));
				args.push_back(Value::NewInt(ac->files.size()));
				RunOnMainThread(ac->callback, args, false);

				logger->Debug("Callback executed");
			}
			catch (ValueException &ex)
			{
				SharedString ss = ex.DisplayString();
				logger->Error(std::string("Error: ") + *ss + " for file: " + file);
			}
			catch (Poco::Exception &ex)
			{
				logger->Error(std::string("Error: ") + ex.displayText() + " for file: " + file);
			}
			catch (std::exception &ex)
			{
				logger->Error(std::string("Error: ") + ex.what() + " for file: " + file);
			}
			catch (...)
			{
				logger->Error(std::string("Unknown error during copy: ") + file);
			}
		}
		ac->Set("running",Value::NewBool(false));
		ac->stopped = true;

		logger->Debug(std::string("Job finished"));

		END_KROLL_THREAD;
	}

	void AsyncCopy::ToString(const ValueList& args, KValueRef result)
	{
		result->SetString("[Async Copy]");
	}

	void AsyncCopy::Cancel(const ValueList& args, KValueRef result)
	{
		KR_DUMP_LOCATION
		if (thread!=NULL && thread->isRunning())
		{
			this->stopped = true;
			this->Set("running",Value::NewBool(false));
			result->SetBool(true);
		}
		else
		{
			result->SetBool(false);
		}
	}
}
