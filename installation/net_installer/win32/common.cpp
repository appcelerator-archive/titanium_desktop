/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009-2010 Appcelerator, Inc. All Rights Reserved.
 */
#include <kroll/utils/utils.h>
#include <windows.h>
#include <Wininet.h>
#include <cmath>
#include <sstream>
#include <string>

using namespace KrollUtils;
using KrollUtils::Application;
using KrollUtils::SharedApplication;
using KrollUtils::KComponentType;
using std::wstring;
using std::string;

// These functions must be defined in the installer implementation.
extern HWND GetInstallerHWND();
extern bool Progress(SharedDependency dependency, int percent);

void ShowError(const wstring& wmsg)
{
	MessageBoxW(GetDesktopWindow(), wmsg.c_str(), L"Installation Failed", 
		MB_OK | MB_SYSTEMMODAL | MB_ICONEXCLAMATION);
}

void ShowError(const string& msg)
{
	wstring wmsg(UTF8ToWide(msg));
	ShowError(wmsg);
}

static wstring GetTempFilePathForDependency(SharedDependency dependency)
{
	string filename;
	switch (dependency->type)
	{
		case MODULE:
			filename = "module-";
			break;
		case RUNTIME:
			filename = "runtime-";
			break;
		case MOBILESDK:
			filename = "mobilesdk-";
			break;
		case APP_UPDATE:
			filename = "appupdate-";
			break;
		case SDK:
			filename = "sdk-";
			break;
	}
	filename.append(dependency->name);
	filename.append("-");
	filename.append(dependency->version);
	filename.append(".zip");
	static string tempdir;
	if (tempdir.empty())
	{
		tempdir.assign(FileUtils::GetTempDirectory());
		FileUtils::CreateDirectory(tempdir);
	}

	return UTF8ToWide(FileUtils::Join(tempdir.c_str(), filename.c_str(), 0));
}

static HINTERNET netHandle = 0;
static HINTERNET GetNetConnection()
{
	if (netHandle)
		return netHandle;

	netHandle = InternetOpenW(
		L"Mozilla/5.0 (compatible; Titanium_Downloader/0.1; Win32)",
		INTERNET_OPEN_TYPE_PRECONFIG, 0, 0, 0);

	if (!netHandle)
	{
		string error(Win32Utils::QuickFormatMessage(GetLastError()));
		error = string("Could not open Internet connection: ") + error;
		ShowError(error);
	}
	
	return netHandle;
}

void ShutdownNetConnection()
{
	if (!netHandle)
		return;

	InternetCloseHandle(netHandle);
}

static void ShowLastDownloadEror()
{
	DWORD bufferSize = 1024, error;
	wchar_t staticErrorBuffer[1024];
	wchar_t* errorBuffer = staticErrorBuffer;
	BOOL success = InternetGetLastResponseInfo(&error, errorBuffer, &bufferSize);

	if (!success && GetLastError() == ERROR_INSUFFICIENT_BUFFER)
	{
		errorBuffer = new wchar_t[bufferSize];
		success = InternetGetLastResponseInfo(&error, errorBuffer, &bufferSize);
	}

	std::wstring errorString(L"Download failed: Unknown error");
	if (success)
		errorString = std::wstring(L"Download failed:") + errorBuffer;

	ShowError(::WideToSystem(errorString));

	if (errorBuffer != staticErrorBuffer)
	   delete [] errorBuffer;
}

bool DownloadDependency(SharedApplication app, SharedDependency dependency)
{
	HINTERNET hINet = GetNetConnection();
	if (!hINet)
	{
		ShowError("Could not establish an Internet connection.");
		return false;
	}

	// If ths URL is a file on the local file system, then this dependency
	// is packaged with the application ala the SDK Installer. In that case
	// just pretend that we downloaded successfully. InstallDependency will
	// take care of unpacking it properly.
	wstring url(UTF8ToWide(app->GetURLForDependency(dependency)));
	if (FileUtils::IsFile(url))
	{
		return true;
	}

	wstring outFilename(GetTempFilePathForDependency(dependency));
	WCHAR szDecodedUrl[INTERNET_MAX_URL_LENGTH];
	DWORD cchDecodedUrl = INTERNET_MAX_URL_LENGTH;
	WCHAR szDomainName[INTERNET_MAX_URL_LENGTH];

	// parse the URL
	HRESULT hr = CoInternetParseUrl(url.c_str(), PARSE_DECODE,
		URL_ENCODING_NONE, szDecodedUrl, INTERNET_MAX_URL_LENGTH,
		&cchDecodedUrl, 0);
	if (hr != S_OK)
	{
		string error = Win32Utils::QuickFormatMessage(GetLastError());
		error = string("Could not decode URL: ") + error;
		ShowError(error);
		return false;
	}

	// figure out the domain/hostname
	hr = CoInternetParseUrl(szDecodedUrl, PARSE_DOMAIN,
		0, szDomainName, INTERNET_MAX_URL_LENGTH, &cchDecodedUrl, 0);
	if (hr != S_OK)
	{
		string error = Win32Utils::QuickFormatMessage(GetLastError());
		error = string("Could not parse domain: ") + error;
		ShowError(error);
		return false;
	}

	// start the HTTP fetch
	HINTERNET hConnection = InternetConnectW(hINet, szDomainName,
		80, L" ", L" ", INTERNET_SERVICE_HTTP, 0, 0 );
	if (!hConnection)
	{
		string error = Win32Utils::QuickFormatMessage(GetLastError());
		error = string("Could not start connection: ") + error;
		ShowError(error);
		return false;
	}

	wstring wurl(szDecodedUrl);
	wstring path = wurl.substr(wurl.find(szDomainName)+wcslen(szDomainName));
	HINTERNET hRequest = HttpOpenRequestW(hConnection, L"GET", path.c_str(),
		0, 0, 0,
		INTERNET_FLAG_IGNORE_CERT_CN_INVALID | // Disregard TLS certificate errors.
		INTERNET_FLAG_IGNORE_CERT_DATE_INVALID |
		INTERNET_FLAG_KEEP_CONNECTION | // Needed for NTLM authentication.
		INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_RELOAD | // Always get the latest.
		INTERNET_FLAG_NO_COOKIES, 0);

	resend:
	HttpSendRequest(hRequest, 0, 0, 0, 0);

	DWORD dwErrorCode = hRequest ? ERROR_SUCCESS : GetLastError();
	if (InternetErrorDlg(GetInstallerHWND(), hRequest, dwErrorCode,
		FLAGS_ERROR_UI_FILTER_FOR_ERRORS |
		FLAGS_ERROR_UI_FLAGS_CHANGE_OPTIONS |
		FLAGS_ERROR_UI_FLAGS_GENERATE_DATA,
		0) == ERROR_INTERNET_FORCE_RETRY)
		goto resend;

	CHAR buffer[2048];
	DWORD bytesRead;
	DWORD contentLength = 0;
	DWORD statusCode = 0;
	DWORD size = sizeof(contentLength);
	BOOL success = HttpQueryInfo(hRequest,
		HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER,
		(LPDWORD) &statusCode, (LPDWORD) &size, 0);
	if (!success || statusCode != 200)
	{
		string error = Win32Utils::QuickFormatMessage(GetLastError());
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

	success = HttpQueryInfo(hRequest,
		HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER,
		(LPDWORD)&contentLength, (LPDWORD)&size, 0);
	if (!success)
	{
		string error = Win32Utils::QuickFormatMessage(GetLastError());
		error = string("Could not determine content length: ") + error;
		ShowError(error);
		return false;
	}

	// now stream the resulting HTTP into a file
	HANDLE file = CreateFileW(outFilename.c_str(), GENERIC_WRITE,
		0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (file == INVALID_HANDLE_VALUE)
	{
		string error = Win32Utils::QuickFormatMessage(GetLastError());
		error = string("Could not open output file (") + WideToUTF8(outFilename) +
			string("): ") + error;
		ShowError(error);
		return false;
	}

	// Keep reading from InternetReadFile as long as it's successful and the number
	// of bytes read is greater than zero.
	bool showError = true;
	DWORD total = 0;
	while ((success = InternetReadFile(hRequest, buffer, 2047, &bytesRead)) && bytesRead > 0)
	{
		// Be sure to Write the entire buffer into to the file.
		DWORD bytesWritten = 0;
		while (bytesWritten < bytesRead)
		{
			if (!WriteFile(file, buffer + bytesWritten,
				bytesRead - bytesWritten, &bytesWritten, 0))
			{
				showError = success = false;
				string error = Win32Utils::QuickFormatMessage(GetLastError());
				error = string("Could write data to output file (") + WideToUTF8(outFilename) +
					string("): ") + error;
				ShowError(error);
				break;
			}
		}

		total += bytesRead;
		printf("total: %i contentLength: %i\n", total, contentLength);
		if (!Progress(dependency, (double)total/(double)contentLength*100))
		{
			showError = success = false;
			break;
		}
	}

	if (!success)
	{
		if (showError)
			ShowLastDownloadEror();

		CancelIo(file);
		CloseHandle(file);
		DeleteFileW(outFilename.c_str());
	}
	else
	{
		CloseHandle(file);
	}

	InternetCloseHandle(hRequest);
	return success;
}

static bool UnzipProgressCallback(char* message, int current, int total, void* data)
{
	SharedDependency* dep = (SharedDependency*) data;
	int percent = total == 0 ? 0 : floor(((double)current/(double)total)*100);
	return Progress(*dep, percent);
}

bool InstallDependency(SharedApplication app, SharedDependency dependency)
{
	string destination(FileUtils::GetSystemRuntimeHomeDirectory());
	if (dependency->type == MODULE)
	{
		destination = FileUtils::Join(destination.c_str(), 
			"modules", OS_NAME, dependency->name.c_str(), 
			dependency->version.c_str(), 0);
	}
	else if (dependency->type == RUNTIME)
	{
		destination = FileUtils::Join(destination.c_str(), 
			"runtime", OS_NAME, dependency->version.c_str(), 0);
	}
	else if (dependency->type == SDK || dependency->type == MOBILESDK)
	{
		// The SDKs unzip directly into the component installation path.
	}
	else if (dependency->type == APP_UPDATE)
	{
		// Application updates need to be unzipped into the application
		// installation directory.
		destination = app->path;
	}
	else
	{
		return false;
	}

	// Recursively create directories and then unzip the temporary
	// download into that directory, calling the progress callback
	// the entire time.
	FileUtils::CreateDirectory(destination, true);

	// First check if this dependency is packaged in the 'dist' folder
	// ala the SDK installer. In that case, we didn't actual
	string zipFile(app->GetURLForDependency(dependency));
	if (!FileUtils::IsFile(zipFile))
		zipFile = WideToUTF8(GetTempFilePathForDependency(dependency));

	return FileUtils::Unzip(zipFile, destination, &UnzipProgressCallback, &dependency);
}



