/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */

#include "installer.h"
#include <cstdlib>
#include <cstring>
#include <libgen.h>
#include <unistd.h>
#include <iostream>
#include <sstream>
using std::string;

int Job::total = 0;
std::string Job::temporaryDirectory = "";
std::string Job::installDirectory = "";
char* Job::curl_error = NULL;
CURL* Job::curl = NULL;

int curl_progress_func(
	Job *fetcher,
	double t, /* dltotal */
	double d, /* dlnow */
	double ultotal,
	double ulnow);
size_t curl_write_func(void *ptr, size_t size, size_t nmemb, FILE *stream);
size_t curl_read_func(void *ptr, size_t size, size_t nmemb, FILE *stream);

void Job::InitDownloader()
{
	Job::temporaryDirectory = FileUtils::GetTempDirectory();
	curl_global_init(CURL_GLOBAL_ALL);
	Job::curl = curl_easy_init();
	if (Job::curl_error == NULL)
	{
		Job::curl_error = new char[CURL_ERROR_SIZE];
	}
}

void Job::ShutdownDownloader()
{
	if (Job::curl != NULL)
	{
		curl_easy_cleanup(Job::curl);
		curl_global_cleanup();
		Job::curl = NULL;
	}

	if (Job::curl_error != NULL)
	{
		delete [] Job::curl_error;
		Job::curl_error = NULL;
	}

	if (!Job::temporaryDirectory.empty()
			&& FileUtils::IsDirectory(Job::temporaryDirectory))
		FileUtils::DeleteDirectory(Job::temporaryDirectory); // Clean up
}

Job::Job(std::string url, int type) :
	url(url),
	type(type),
	index(++Job::total),
	progress(0.0),
	componentType(KrollUtils::UNKNOWN),
	name("unknown"),
	version("unknown"),
	download(true)
{
	if (FileUtils::IsFile(url))
	{
		this->ParseFile(url);
		this->out_filename = url;
		this->download = false;
	}
	else if (this->type == COMPONENT_JOB)
	{
		this->ParseURL(url);

	}

	// FIXME: Linux net_installer doesn't support SSL currently, because it
	// has a statically linked version of libcurl, so we just assume we can
	// fetch this update via regular HTTP instead.
	if (this->url.find("https://") == 0)
		this->url.erase(4, 1);
}

void Job::ParseFile(std::string url)
{
	char* url_cstr = strdup(url.c_str());
	std::string file = basename(url_cstr);
	free(url_cstr);

	size_t start, end;
	end = file.find("-");
	std::string partOne = file.substr(0, end);
	if (partOne == "runtime")
	{
		this->componentType = KrollUtils::RUNTIME;
		this->name = "runtime";
	}
	else if (partOne == "sdk")
	{
		this->componentType = KrollUtils::SDK;
		this->name = "sdk";
	}
	else if (partOne == "mobilesdk")
	{
		this->componentType = KrollUtils::MOBILESDK;
		this->name = "mobilesdk";
	}
	else if (partOne == "module")
	{
		this->componentType = KrollUtils::MODULE;
		start = end + 1;
		end = file.find("-", start);
		this->name = file.substr(start, end - start);
	}

	start = end + 1;
	end = file.find(".zip", start);
	this->version = file.substr(start, end - start);
}

void Job::ParseURL(std::string url)
{
	size_t start, end;
	start = url.find("name=");
	if (start != std::string::npos)
	{
		start += 5;
		end = url.find("&", start);
		if (end != std::string::npos)
			this->name = url.substr(start, end - start);
	}

	start = url.find("version=");
	if (start != std::string::npos)
	{
		start += 8;
		end = url.find("&", start);
		if (end != std::string::npos)
			this->version = url.substr(start, end - start);
	}

	if (this->name == std::string("runtime"))
	{
		this->componentType = KrollUtils::RUNTIME;
	}
	else if (this->name == std::string("sdk"))
	{
		this->componentType = KrollUtils::SDK;
	}
	else if (this->name == std::string("mobilesdk"))
	{
		this->componentType = KrollUtils::MOBILESDK;
	}
	else
	{
		this->componentType = KrollUtils::MODULE;
	}
}

void Job::Fetch()
{
	if (!this->download)
	{
		this->progress = 1.0;
		return;
	}

	FILE *out = NULL;
	this->progress = 0.0;

	try
	{
		if (Job::curl == NULL)
			throw std::string("Download failed: could not initialize cURL.");

		this->out_filename = Job::temporaryDirectory + "/";
		if (this->type == COMPONENT_JOB)
		{
			std::string filename =
				"component-" + this->name + "-" + this->version + ".zip";
			this->out_filename += filename;
		}
		else
		{
			this->out_filename += "application-update.zip";
		}

		FILE* out = fopen(out_filename.c_str(), "w");
		if (out == NULL)
			throw std::string("Download failed: could not open file for writing.");

		curl_easy_setopt(Job::curl, CURLOPT_URL, this->url.c_str());
		curl_easy_setopt(Job::curl, CURLOPT_WRITEDATA, out);
		curl_easy_setopt(Job::curl, CURLOPT_WRITEFUNCTION, curl_write_func);
		curl_easy_setopt(Job::curl, CURLOPT_READFUNCTION, curl_read_func);
		curl_easy_setopt(Job::curl, CURLOPT_NOPROGRESS, 0L);
		curl_easy_setopt(Job::curl, CURLOPT_PROGRESSFUNCTION, curl_progress_func);
		curl_easy_setopt(Job::curl, CURLOPT_PROGRESSDATA, this);
		curl_easy_setopt(Job::curl, CURLOPT_FOLLOWLOCATION, 1);
		curl_easy_setopt(Job::curl, CURLOPT_FAILONERROR, 1);
		curl_easy_setopt(Job::curl, CURLOPT_ERRORBUFFER, Job::curl_error);
		CURLcode result = curl_easy_perform(Job::curl);

		// Don't report an error if the user cancelled
		Installer::Stage s = Installer::instance->GetStage();
		if (result != CURLE_OK
			&& s != Installer::CANCELLED
			&& s != Installer::CANCEL_REQUEST)
			throw std::string("Download failed: ") + Job::curl_error;

		fflush(out);
		fclose(out);
	}
	catch (...)
	{
		// Cleanup
		if (out != NULL)
		{
			fflush(out);
			fclose(out);
		}
		throw;
	}

	this->progress = 1.0;
}

void Job::UnzipComponent()
{
	string outdir = Job::installDirectory;

	// SDK installation installs to the top-level directory.
	// Other component types neeed to be in a subdirectory.
	if (this->componentType == KrollUtils::RUNTIME)
	{
		outdir = FileUtils::Join(
			outdir.c_str(), "runtime",
			"linux", this->version.c_str(), NULL);
	}
	else if (this->componentType == KrollUtils::MODULE)
	{
		outdir = FileUtils::Join(
			outdir.c_str(), "modules", "linux",
			this->name.c_str(), this->version.c_str(), NULL);
	}

	FileUtils::CreateDirectory(outdir, true);
	FileUtils::Unzip(this->out_filename, outdir);
}

void Job::UnzipApplication()
{
	FileUtils::Unzip(this->out_filename, Installer::applicationPath);
}

void Job::Unzip()
{
	this->progress = ((double) this->index) / ((double) Job::total);
	if (this->type == COMPONENT_JOB)
	{
		this->UnzipComponent();
	}
	else
	{
		this->UnzipApplication();
	}
}

int Job::GetIndex()
{
	return this->index;
}

std::string Job::GetFilename()
{
	return this->out_filename;
}

void Job::SetProgress(double progress)
{
	this->progress = progress;
}

double Job::GetProgress()
{
	return this->progress;
}

size_t curl_write_func(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	return fwrite(ptr, size, nmemb, stream);
}

size_t curl_read_func(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	return fread(ptr, size, nmemb, stream);
}

int curl_progress_func(
	Job *job,
	double t, /* dltotal */
	double d, /* dlnow */
	double ultotal,
	double ulnow)
{
	Installer::Stage s = Installer::instance->GetStage();
	if (s == Installer::CANCELLED || s == Installer::CANCEL_REQUEST || s == Installer::ERROR)
		return 1;

	if (t == 0)
		job->SetProgress(0);
	else
		job->SetProgress(d/t);
	return 0;
}

