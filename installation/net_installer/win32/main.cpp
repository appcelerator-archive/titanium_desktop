/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#include <windows.h>
#include <new.h>
#include <objbase.h>
#include <Wininet.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <fcntl.h>
#include <io.h>
#include <stdlib.h>
#include <stdio.h>
#include <utils.h>
#include "progress_dialog.h"
#include "Resource.h"
#include "intro_dialog.h"
#include "update_dialog.h"
#include "api/utils/utils.h"
#undef CreateDirectory

using std::string;
using std::wstring;
using std::vector;
using KrollUtils::Application;
using KrollUtils::SharedApplication;
using KrollUtils::KComponentType;
using namespace KrollUtils;

HINSTANCE mainInstance;
HICON mainIcon;

SharedApplication app;
string exePath;
string updateFile;
string appPath;
string runtimeHome;
string appInstallPath;
string componentInstallPath;
string temporaryPath;
bool doInstall = false;
bool installStartMenuIcon = false;
bool forceInstall = false;
ProgressDialog* progressDialog;

enum IType
{
	CRUNTIME,
	CMODULE,
	CUPDATE,
	CSDK,
	CMOBILESDK,
	CUNKNOWN
};

class Job
{
public:
	std::string name, version, url;
};

void ShowError(string msg)
{
	wstring wmsg = KrollUtils::UTF8ToWide(msg);
	MessageBoxW(
		GetDesktopWindow(),
		wmsg.c_str(),
		L"Installation Failed",
		MB_OK | MB_SYSTEMMODAL | MB_ICONEXCLAMATION);
}

std::wstring ParseQueryParam(string uri8, string key8)
{
	std::wstring uri = KrollUtils::UTF8ToWide(uri8);
	std::wstring key = KrollUtils::UTF8ToWide(key8);
	key+=L"=";
	size_t pos = uri.find(key);
	
	if (pos!=std::wstring::npos)
	{
		std::wstring p = uri.substr(pos + key.length());
		pos = p.find(L"&");
		if (pos!=std::wstring::npos)
		{
			p = p.substr(0,pos);
		}

		// decode
		WCHAR szOut[INTERNET_MAX_URL_LENGTH];
		DWORD cchDecodedUrl = INTERNET_MAX_URL_LENGTH;
		CoInternetParseUrl(p.c_str(), PARSE_UNESCAPE, 0, szOut, INTERNET_MAX_URL_LENGTH, &cchDecodedUrl, 0);
		p.assign(szOut);

		return p;
	}
	return L"";
}

std::wstring SizeString(DWORD size)
{
	char str[512];

#define KB 1024
#define MB KB * 1024

	if (size < KB) {
		sprintf(str, "%0.2f bytes", size);
	}

	else if (size < MB) {
		double sizeKB = size/1024.0;
		sprintf(str, "%0.2f KB", sizeKB);
	}
	
	else {
		// hopefully we shouldn't ever need to count more than 1023 in a single file!
		double sizeMB = size/1024.0/1024.0;
		sprintf(str, "%0.2f MB", sizeMB);
	}
	
	std::string string(str);
	std::wstring wstr(string.begin(),string.end());
	return wstr;
}

bool DownloadURL(HINTERNET hINet, std::string urlA, std::string outFilenameA, std::string introA)
{
	std::wstring url = KrollUtils::UTF8ToWide(urlA);
	std::wstring outFilename = KrollUtils::UTF8ToWide(outFilenameA);
	std::wstring intro = KrollUtils::UTF8ToWide(introA);

	WCHAR szDecodedUrl[INTERNET_MAX_URL_LENGTH];
	DWORD cchDecodedUrl = INTERNET_MAX_URL_LENGTH;
	WCHAR szDomainName[INTERNET_MAX_URL_LENGTH];

	// parse the URL
	HRESULT hr = CoInternetParseUrl(url.c_str(), PARSE_DECODE, 
		URL_ENCODING_NONE, szDecodedUrl, INTERNET_MAX_URL_LENGTH, 
		&cchDecodedUrl, 0);
	if (hr != S_OK)
	{
		std::string error = Win32Utils::QuickFormatMessage(GetLastError());
		error = string("Could not decode URL: ") + error;
		ShowError(error);
		return false;
	}

	// figure out the domain/hostname
	hr = CoInternetParseUrl(szDecodedUrl, PARSE_DOMAIN, 
		0, szDomainName, INTERNET_MAX_URL_LENGTH, &cchDecodedUrl, 0);
	if (hr != S_OK)
	{
		std::string error = Win32Utils::QuickFormatMessage(GetLastError());
		error = string("Could not parse domain: ") + error;
		ShowError(error);
		return false;
	}
	
	// start the HTTP fetch
	HINTERNET hConnection = InternetConnectW(hINet, szDomainName, 
		80, L" ", L" ", INTERNET_SERVICE_HTTP, 0, 0 );
	if (!hConnection)
	{
		std::string error = Win32Utils::QuickFormatMessage(GetLastError());
		error = string("Could not start connection: ") + error;
		ShowError(error);
		return false;
	}
	
	std::wstring wurl(szDecodedUrl);
	std::wstring path = wurl.substr(wurl.find(szDomainName)+wcslen(szDomainName));
	//std::wstring queryString = url.substr(url.rfind("?")+1);
	//astd::wstring object = path + "?" + queryString;
	HINTERNET hRequest = HttpOpenRequestW(hConnection, L"GET", path.c_str(), 
		NULL, NULL, NULL, 
		INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE | 
		INTERNET_FLAG_NO_COOKIES | INTERNET_FLAG_NO_UI | 
		INTERNET_FLAG_IGNORE_CERT_CN_INVALID | 
		INTERNET_FLAG_IGNORE_CERT_DATE_INVALID, 0);

	if (!hRequest)
	{
		InternetCloseHandle(hConnection);

		std::string error = Win32Utils::QuickFormatMessage(GetLastError());
		error = string("Could not open request: ") + error;
		ShowError(error);

		return false;
	}

	// now stream the resulting HTTP into a file
	std::ofstream ostr;
	ostr.open(outFilename.c_str(), std::ios_base::binary | std::ios_base::trunc);

	bool failed = false;
	CHAR buffer[2048];
	DWORD bytesRead;
	DWORD total = 0;
	wchar_t msg[255];
	
	HttpSendRequest(hRequest, NULL, 0, NULL, 0);

	DWORD contentLength = 0;
	DWORD statusCode = 0;
	DWORD size = sizeof(contentLength);
	
	BOOL success = HttpQueryInfo(hRequest, 
		HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER,
		(LPDWORD) &statusCode, (LPDWORD) &size, NULL);
	if (!success || statusCode != 200)
	{
		std::string error = Win32Utils::QuickFormatMessage(GetLastError());
		if (success)
		{
			std::ostringstream str;
			str << "Invalid HTTP Status Code (" << statusCode << ")";
			error = str.str();
		}
		error = string("Could not query info: ") + error;
		ShowError(error);
		return false;
	}
	
	success = HttpQueryInfo(hRequest, HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER,
		(LPDWORD)&contentLength, (LPDWORD)&size, NULL);
	if (!success)
	{
		std::string error = Win32Utils::QuickFormatMessage(GetLastError());
		error = string("Could not determine content length: ") + error;
		ShowError(error);
		return false;
	}
	
	std::wstring contentLengthStr = SizeString(contentLength);
	
	// Use do/while since the last call to InternetReadFile might actually read bytes
	do
	{
		if (!InternetReadFile(hRequest, buffer, 2047, &bytesRead))
		{
			ostr.close();
		}
		
		if (progressDialog->IsCancelled())
		{
			failed = true;
			break;
		}
		
		if (bytesRead == 0)
		{
			break;
		}
		else
		{
			buffer[bytesRead] = '\0';
			total += bytesRead;
			ostr.write(buffer, bytesRead);
			progressDialog->SetLineText(2,intro + L": " + SizeString(total) + L" of " + contentLengthStr,true);
			progressDialog->Update(total, contentLength);
		}
	} while(true);
	
	InternetCloseHandle(hConnection);
	InternetCloseHandle(hRequest);
	return !failed;
}

void UnzipProgress(char *message, int current, int total, void *data)
{
	progressDialog->SetLineText(2, message, true);
	progressDialog->Update(current, total);
}

void Install(IType type, string name, string version, string path)
{
	string destination;
	if (type == CMODULE)
	{
		destination = FileUtils::Join(
			componentInstallPath.c_str(), "modules", OS_NAME, name.c_str(), version.c_str(), NULL);
	}
	else if (type == CRUNTIME)
	{
		destination = FileUtils::Join(
			componentInstallPath.c_str(), "runtime", OS_NAME, version.c_str(), NULL);
	}
	else if (type == CSDK || type == CMOBILESDK)
	{
		destination = componentInstallPath;
	}
	else if (type == CUPDATE)
	{
		destination = app->path;
	}
	else
	{
		return;
	}

	// Recursively create directories
	FileUtils::CreateDirectory(destination, true);
	FileUtils::Unzip(path, destination, &UnzipProgress, 0);
}

void ProcessUpdate(HINTERNET hINet)
{
	string version = app->version;
	string name = app->name;
	string url = app->GetUpdateURL();

	string path = "update-update.zip";
	path = FileUtils::Join(temporaryPath.c_str(), path.c_str(), NULL);

	// Figure out the path and destination
	string intro = string("Downloading application update");
	bool downloaded = DownloadURL(hINet, url, path, intro);
	if (downloaded)
	{
		progressDialog->SetLineText(2, string("Installing ") + name + "-" + version + "...", true);
		Install(CUPDATE, name, version, path);
	}
}

void ProcessURL(string url, HINTERNET hINet)
{
	std::wstring wuuid = ParseQueryParam(url, "uuid");
	std::wstring wname = ParseQueryParam(url, "name");
	std::wstring wversion = ParseQueryParam(url, "version");
	
	string uuid = KrollUtils::WideToUTF8(wuuid);
	string name = KrollUtils::WideToUTF8(wname);
	string version = KrollUtils::WideToUTF8(wversion);
	
	IType type = CUNKNOWN;

	string path = "";
	if (string(RUNTIME_UUID) == uuid)
	{
		type = CRUNTIME;
		path = "runtime-";
	}
	else if (string(MODULE_UUID) == uuid)
	{
		type = CMODULE;
		path = "module-";
	}
	else if (string(SDK_UUID) == uuid)
	{
		type = CSDK;
		path = "sdk-";
	}
	else if (string(MOBILESDK_UUID) == uuid)
	{
		type = CMOBILESDK;
		path = "mobilesdk-";
	}
	else
	{
		return;
	}

	path.append(name + "-");
	path.append(version + ".zip");
	path = FileUtils::Join(temporaryPath.c_str(), path.c_str(), NULL);

	// Figure out the path and destination
	string intro = string("Downloading ") + name + " " + version;
	bool downloaded = DownloadURL(hINet, url, path, intro);

	if (downloaded)
	{
		std::string lineText = "Installing ";
		lineText.append(name);
		lineText.append("-");
		lineText.append(version);
		lineText.append("...");
		progressDialog->SetLineText(2, lineText, true);
		Install(type, name, version, path);
	}
}

void ProcessFile(string fullPath)
{
	IType type = CUNKNOWN;
	string name = "";
	string version = "";

	string path = FileUtils::Basename(fullPath);

	size_t start, end;
	end = path.find("-");
	std::string partOne = path.substr(0, end);
	if (partOne == "runtime")
	{
		type = CRUNTIME;
		name = "runtime";
	}
	else if (partOne == "sdk")
	{
		type = CSDK;
		name = "sdk";
	}
	else if (partOne == "mobilesdk")
	{
		type = CMOBILESDK;
		name = "mobilesdk";
	}
	else if (partOne == "module")
	{
		type = CMODULE;
		start = end + 1;
		end = path.find("-", start);
		name = path.substr(start, end - start);
	}

	start = end + 1;
	end = path.find(".zip", start);
	version = path.substr(start, end - start);

	progressDialog->SetLineText(2, string("Installing ") + name + "-" + version + "...", true);
	Install(type, name, version, fullPath);
}

bool InstallApplication()
{
	if (forceInstall || !app->IsInstalled())
	{
		progressDialog->SetLineText(2, string("Installing to ") + appInstallPath, true);
		FileUtils::CreateDirectory(appInstallPath);
		FileUtils::CopyRecursive(app->path, appInstallPath, "dist");
	}
	return true;
}

bool HandleAllJobs(vector<ti::InstallJob*> jobs)
{
	if (jobs.size() == 0)
	{
		return true;	
	}

	temporaryPath = FileUtils::GetTempDirectory();
	
	FileUtils::CreateDirectory(temporaryPath);

	int count = jobs.size();
	bool success = true;
	
	// Create our progress indicator class
	progressDialog->SetTitle(L"Titanium Installer");
	progressDialog->SetCancelMessage(L"Cancelling, one moment...");

	wchar_t buf[255];
	wsprintfW(buf,L"Preparing to download %d file%s", count, (count > 1 ? L"s" : L""));
	progressDialog->SetLineText(2,std::wstring(buf),true);
	progressDialog->Update(0, count);
	
	// Initialize the Interent DLL
	HINTERNET hINet = InternetOpenW(
		L"Mozilla/5.0 (compatible; Titanium_Downloader/0.1; Win32)",
		INTERNET_OPEN_TYPE_PRECONFIG,
		NULL, NULL, 0 );

	// For each URL, fetch the URL and then unzip it
	DWORD x = 0;
	
	for (int i = 0; i < jobs.size(); i++)
	{
		progressDialog->Update(x++, count);
		ti::InstallJob *job = jobs[i];
		progressDialog->SetLineText(3, "Downloading: " + job->url, true);
		if (job->isUpdate)
		{
			ProcessUpdate(hINet);
		}

		if (FileUtils::IsFile(job->url))
		{
			ProcessFile(job->url);
		}
		else
		{
			ProcessURL(job->url, hINet);
		}

		if (progressDialog->IsCancelled())
		{
			return false;
		}
	}

	// done with iNet - so close it
	InternetCloseHandle(hINet);

	if (progressDialog->IsCancelled())
		success = false;

	if (!temporaryPath.empty()  && FileUtils::IsDirectory(temporaryPath))
		FileUtils::DeleteDirectory(temporaryPath);
	return success;
}

bool CreateLink(std::string& path, std::string& linkPath, std::string& description) 
{ 
	HRESULT hres; 
	IShellLink* psl; 
 
	// Get a pointer to the IShellLink interface. 
	hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&psl); 
	if (SUCCEEDED(hres)) 
	{ 
		IPersistFile* ppf; 
		wstring pathW = KrollUtils::UTF8ToWide(path);
		wstring linkPathW = KrollUtils::UTF8ToWide(linkPath);
		wstring descriptionW = KrollUtils::UTF8ToWide(description);
 
		// Set the path to the shortcut target and add the description. 
		psl->SetPath(pathW.c_str()); 
		psl->SetDescription(descriptionW.c_str()); 
 
		// Query IShellLink for the IPersistFile interface for saving the 
		// shortcut in persistent storage. 
		hres = psl->QueryInterface(IID_IPersistFile, (LPVOID*) &ppf); 
 
		if (SUCCEEDED(hres)) 
		{ 
			// Save the link by calling IPersistFile::Save. 
			hres = ppf->Save(linkPathW.c_str(), TRUE); 
			ppf->Release(); 
			return true;
		} 
		psl->Release(); 
	} 
	return false;
}

bool FinishInstallation()
{
	if (forceInstall || !app->IsInstalled())
	{
		string installedFile = FileUtils::Join(app->GetDataPath().c_str(), ".installed", NULL);
		FILE* file = fopen(installedFile.c_str(), "w");
		fprintf(file, "%s\n", appInstallPath.c_str());
		fclose(file);

		// Inform the boot where the application installed to
		installedFile = FileUtils::Join(app->GetDataPath().c_str(), ".installedto", NULL);
		file = fopen(installedFile.c_str(), "w");
		fprintf(file, "%s\n", appInstallPath.c_str());
		fclose(file);
	}

	if(!updateFile.empty() && FileUtils::IsFile(updateFile))
	{
		wstring updateFileW = KrollUtils::UTF8ToWide(updateFile);
		DeleteFile(updateFileW.c_str());
	}

	if (installStartMenuIcon && (forceInstall || !app->IsInstalled()))
	{
		string newExe = FileUtils::Basename(exePath);
		newExe = FileUtils::Join(appInstallPath.c_str(), newExe.c_str(), NULL);

		wchar_t pathW[MAX_PATH];
		if (SHGetSpecialFolderPath(NULL, pathW, CSIDL_PROGRAMS, TRUE))
		{
			std::wstring pathWStr = pathW;
			string path = KrollUtils::WideToUTF8(pathWStr);
			string linkPath = app->name + ".lnk";
			linkPath = FileUtils::Join(path.c_str(), linkPath.c_str(), NULL);
			CreateLink(newExe, linkPath, string(""));
		}
	}

	return true;
}

string GetDefaultInstallationDirectory()
{
	wchar_t pathW[MAX_PATH];
	if (SHGetSpecialFolderPath(NULL, pathW, CSIDL_PROGRAM_FILES, FALSE))
	{
		std::wstring pathWStr = pathW;
		std::string path = KrollUtils::WideToUTF8(pathWStr);
		return FileUtils::Join(path.c_str(), app->name.c_str(), NULL);
	}
	else // That would be really weird, but handle it
	{
		return FileUtils::Join("C:", app->name.c_str(), NULL);
	}
}

void RedirectIOToConsole();

int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	mainInstance = ::GetModuleHandle(NULL);
	mainIcon = LoadIcon(mainInstance, MAKEINTRESOURCE(IDR_MAINFRAME));

	int argc = __argc;
	char** argv = __argv;
	vector<ti::InstallJob*> jobs;
	string jobsFile;
	bool quiet = false;
	
	for (int i = 1; i < argc; i++)
	{
		string arg = argv[i];
		
		if (arg == "-appPath" && argc > i+1)
		{
			i++;
			appPath = argv[i];
		}
		else if (arg == "-exePath" && argc > i+1)
		{
			i++;
			exePath = argv[i];
		}
		else if (arg == "-updateFile" && argc > i+1)
		{
			i++;
			updateFile = argv[i];
		}
		else if (arg == "-quiet")
		{
			quiet = true;
		}
		else if (arg == "-forceInstall")
		{
			forceInstall = true;
		}
		else if (arg == "-debug")
		{
			RedirectIOToConsole();
		}
		else
		{
			jobsFile = arg;
		}
	}
	
	if (appPath.empty() || exePath.empty())
	{
		ShowError("The installer was not given enough information to continue.");
		return __LINE__;
	}

	bool updateDialog = false;
	if (updateFile.empty())
	{
		app = Application::NewApplication(appPath);
	}
	else
	{
		updateDialog = true;
		app = Application::NewApplication(updateFile, appPath);
	}

	//printf("exePath=%s,basename=%s,appPath=%s",exePath.c_str(),FileUtils::Basename(exePath).c_str(),appPath.c_str());
	
	//if (!exePath.empty() && FileUtils::Dirname(exePath) == appPath) {
	//	updateDialog = true;
	//}
	
	if (app.isNull())
	{
		ShowError("The installer could not read the application manifest.");
		return __LINE__;
	}

	if (!updateFile.empty())
	{
		appInstallPath = app->path;
	}
	else
	{
		appInstallPath = GetDefaultInstallationDirectory();
	}
	
	
	componentInstallPath = FileUtils::GetSystemRuntimeHomeDirectory();
	
	// Command line arguments from kboot.exe are ANSI encoded, not 100% sure why yet
	// .. could be ShellExecute
	std::wstring wideJobsFile = UTILS_NS::MBToWide(jobsFile, jobsFile.length(), CP_ACP);
	jobs = ti::InstallJob::ReadJobs(wideJobsFile);
	if (!updateFile.empty())
	{
		ti::InstallJob* updateJob = new ti::InstallJob(true);
		updateJob->name = app->name;
		updateJob->version = app->version;
		jobs.push_back(updateJob);
	}
	
	if (jobs.size() == 0)
	{
		// Warn if there are no jobs, we still want to copy the app into
		// it's system location though, right?
		fprintf(stderr, "WARNING: No jobs were defined for the installer.\n");
	}
	
	// Major WTF here, Redmond.
	LoadLibrary(TEXT("Riched20.dll"));
	CoInitialize(NULL);

	if (!quiet)
	{
		ti::Dialog *dialog = NULL;
		if (updateDialog) {
			dialog = new ti::UpdateDialog(jobs);
		}
		else {
		
			dialog = new ti::IntroDialog();
		}
		if (!dialog->GetWindow())
		{
			int i = GetLastError();
			ShowError("The installer failed to open a dialog.");
			return __LINE__;
		}

		MSG msg;
		int status;
		while ((status = GetMessage(&msg, 0, 0, 0)) != 0)
		{
			if (status == -1)
			{
				char buf[2000];
				sprintf(buf, "Error: %i", GetLastError());
				ShowError(buf);
				return -1;
			}
			if (!IsDialogMessage(dialog->GetWindow(), &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}
	else
	{
		doInstall = true;
	}
	
	if (doInstall)
	{
		progressDialog = new ProgressDialog;
		progressDialog->SetLineText(1, app->name, false);
		progressDialog->Show();
		bool success = 
			InstallApplication() &&
			HandleAllJobs(jobs) &&
			FinishInstallation();
		
		CoUninitialize();
		return success ? 0 : 1;
	}
	else
	{
		CoUninitialize();
		return 1;
	}
}

static const WORD MAX_CONSOLE_LINES = 500;

void RedirectIOToConsole()
{
	int hConHandle;
	long lStdHandle;
	CONSOLE_SCREEN_BUFFER_INFO coninfo;
	FILE *fp;

	// allocate a console for this app
	AllocConsole();

	// set the screen buffer to be big enough to let us scroll text
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);
	coninfo.dwSize.Y = MAX_CONSOLE_LINES;

	SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize);

	// redirect unbuffered STDOUT to the console
	lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
	fp = _fdopen( hConHandle, "w" );

	*stdout = *fp;
	setvbuf( stdout, NULL, _IONBF, 0 );

	// redirect unbuffered STDIN to the console
	lStdHandle = (long)GetStdHandle(STD_INPUT_HANDLE);
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);

	fp = _fdopen( hConHandle, "r" );
	*stdin = *fp;
	setvbuf( stdin, NULL, _IONBF, 0 );

	// redirect unbuffered STDERR to the console
	lStdHandle = (long)GetStdHandle(STD_ERROR_HANDLE);
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
	fp = _fdopen( hConHandle, "w" );
	*stderr = *fp;
	setvbuf( stderr, NULL, _IONBF, 0 );

	// make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog
	// point to console as well
	std::ios::sync_with_stdio();
}

