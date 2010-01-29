/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#include <kroll/utils/utils.h>
#include <msi.h>
#include <msiquery.h>
#include <sstream>
#include <Wininet.h>
#include <math.h>

#pragma comment(linker, "/EXPORT:NetInstallSetup=_NetInstallSetup@4")
#pragma comment(linker, "/EXPORT:NetInstall=_NetInstall@4")
#pragma comment(linker, "/EXPORT:Clean=_Clean@4")

using namespace KrollUtils;
using namespace std;
wstring MsiProperty(MSIHANDLE hInstall, const wchar_t* property)
{
	wchar_t buffer[4096];
	DWORD bufferLength = 4096;
	MsiGetProperty(hInstall, property, buffer, &bufferLength);

	return wstring(buffer, bufferLength);
}

vector<wstring>& Split(const wstring& s, wchar_t delim, vector<wstring>& elems)
{
	wstringstream ss(s);
	wstring item;
	while(getline(ss, item, delim))
	{
		elems.push_back(item);
	}
	return elems;
}

void ShowError(string msg)
{
	wstring wmsg = KrollUtils::UTF8ToWide(msg);
	MessageBoxW(
		GetDesktopWindow(),
		wmsg.c_str(),
		L"Installation Failed",
		MB_OK | MB_SYSTEMMODAL | MB_ICONEXCLAMATION);
}

void ShowError(wstring wmsg)
{
	MessageBoxW(
		GetDesktopWindow(),
		wmsg.c_str(),
		L"Installation Failed",
		MB_OK | MB_SYSTEMMODAL | MB_ICONEXCLAMATION);
}

SharedApplication CreateApplication(MSIHANDLE hInstall)
{
	wstring params = MsiProperty(hInstall, L"CustomActionData");
	vector<wstring> tokens;
	Split(params, L';', tokens);

	if (tokens[0] == L"app_update")
	{
		wstring updateManifest = tokens[1];
		wstring appPath = tokens[2];

		return Application::NewApplication(
			WideToUTF8(updateManifest), WideToUTF8(appPath));
	}
	else
	{
		wstring dependencies(tokens[0]);
		vector<pair<string, string> > manifest;
		Split(dependencies, L'&', tokens);
		for (size_t i = 0; i < tokens.size(); i++)
		{
			wstring token = tokens[i];
			wstring key = token.substr(0, token.find(L"="));
			wstring value = token.substr(token.find(L"=")+1);

			manifest.push_back(pair<string,string>(
				WideToUTF8(key), WideToUTF8(value)));
		}

		return Application::NewApplication(manifest);
	}
}

vector<SharedDependency>
FindUnresolvedDependencies(MSIHANDLE hInstall)
{
	vector<SharedDependency> unresolved;
	vector<SharedComponent> components;
	vector<SharedComponent>& installedComponents =
		BootUtils::GetInstalledComponents(true);
	for (size_t i = 0; i < installedComponents.size(); i++)
	{
		components.push_back(installedComponents.at(i));
	}

	wstring dependencies, bundledModules, bundledRuntime;
	wstring updateManifest, installDir;
	vector<wstring> tokens;
	// deferred / async mode, get from the hacked "CustomActionData" property
	if (MsiGetMode(hInstall, MSIRUNMODE_SCHEDULED) == TRUE)
	{
		wstring params = MsiProperty(hInstall, L"CustomActionData");
		Split(params, L';', tokens);

		if (tokens[0] == L"app_update")
		{
			updateManifest.assign(tokens[1]);
			installDir.assign(tokens[2]);
		}
		else
		{
			dependencies.assign(tokens[0]);
			if (tokens.size() > 1) bundledModules.assign(tokens[1]);
			if (tokens.size() > 2) bundledRuntime.assign(tokens[2]);
		}
	}
	else // immediate mode, get from actual properties.. god this sucks
	{
		dependencies.assign(MsiProperty(hInstall, L"AppDependencies"));
		bundledModules.assign(MsiProperty(hInstall, L"AppBundledModules"));
		bundledRuntime.assign(MsiProperty(hInstall, L"AppBundledRuntime"));
	}

	Split(bundledModules, L',', tokens);
	for (size_t i = 0; i < tokens.size(); i++)
	{
		components.push_back(KComponent::NewComponent(
			MODULE, WideToUTF8(tokens[i]), "", "", true));
	}

	if (bundledRuntime.size() > 0)
	{
		components.push_back(KComponent::NewComponent(
			RUNTIME, "runtime", "", "", true));
	}

	if (updateManifest.size() > 0)
	{
		SharedApplication app = Application::NewApplication(
			WideToUTF8(updateManifest), WideToUTF8(installDir));


		unresolved = app->ResolveDependencies();
		if (FileUtils::Basename(WideToUTF8(updateManifest)) == ".update")
		{
			unresolved.push_back(Dependency::NewDependencyFromValues(
				APP_UPDATE, "app_update", app->version));
		}
	}

	tokens.clear();
	Split(dependencies, L'&', tokens);
	for (size_t i = 0; i < tokens.size(); i++)
	{
		wstring token = tokens[i];
		wstring key = token.substr(0, token.find(L"="));
		wstring value = token.substr(token.find(L"=")+1);
		if (key.at(0) == L'#')
		{
			continue;
		}

		SharedDependency dependency = Dependency::NewDependencyFromManifestLine(
			WideToUTF8(key), WideToUTF8(value));
		SharedComponent c = BootUtils::ResolveDependency(dependency, components);
		if (c.isNull())
		{
			unresolved.push_back(dependency);
		}
	}

	return unresolved;
}

// a helper function that sends a progress message to the installer
UINT Progress(MSIHANDLE hInstall, SharedDependency dependency,
	const wchar_t *intro, int percent)
{
	static int oldPercent = -1;
	if (oldPercent == percent) // prevent updating too often / flickering
		return IDOK;

	oldPercent = percent;

	wstring message(intro);
	if (dependency->type == MODULE)
	{
		message += L"Module \"";
		message += UTF8ToWide(dependency->name);
		message += L"\" ";
	}
	else if (dependency->type == SDK)
	{
		message += L"SDK ";
	}
	else if (dependency->type == MOBILESDK)
	{
		message += L"Mobile SDK ";
	}
	else if (dependency->type == APP_UPDATE)
	{
		message += L"Application Update ";
	}
	else
	{
		message += L"Runtime ";
	}
	message += UTF8ToWide(dependency->version);
	message += L" (";
	wchar_t buffer[8];
	_itow(percent, buffer, 10);
	message += buffer;
	message += L"%)";

	PMSIHANDLE actionRecord = MsiCreateRecord(3);

	MsiRecordSetString(actionRecord, 1, L"NetInstall");
	MsiRecordSetString(actionRecord, 2, message.c_str());
	MsiRecordSetString(actionRecord, 3, L"Downloading..");

	return MsiProcessMessage(hInstall, INSTALLMESSAGE_ACTIONSTART, actionRecord);
}

extern "C" UINT __stdcall
NetInstallSetup(MSIHANDLE hInstall)
{
	// Installer is generating the installation script of the
	// custom action.

	// Tell the installer to increase the value of the final total
	// length of the progress bar by the total number of ticks in
	// the custom action.
	vector<SharedDependency> unresolved = FindUnresolvedDependencies(hInstall);
	PMSIHANDLE hProgressRec = MsiCreateRecord(2);

	MsiRecordSetInteger(hProgressRec, 1, 3);
	MsiRecordSetInteger(hProgressRec, 2, unresolved.size());

	UINT iResult = MsiProcessMessage(
		hInstall, INSTALLMESSAGE_PROGRESS, hProgressRec);
	if ((iResult == IDCANCEL))
		return ERROR_INSTALL_USEREXIT;
	return ERROR_SUCCESS;
}

typedef struct {
	MSIHANDLE hInstall;
	SharedDependency dependency;
} UnzipProgressData;

bool UnzipProgress(char *message, int current, int total, void *data)
{
	UnzipProgressData* progressData = (UnzipProgressData*) data;

	int percent = total == 0 ? 0 : floor(((double)current/(double)total)*100);
	UINT result = Progress(progressData->hInstall,
		progressData->dependency, L"Extracting ", percent);
	if (result == IDCANCEL)
		return false;
	return true;
}

wstring GetFilePath(SharedDependency dependency)
{
	wstring outFilename;
	string filename;
	switch (dependency->type)
	{
		case MODULE: filename = "module-"; break;
		case RUNTIME: filename = "runtime-"; break;
		case MOBILESDK:filename = "mobilesdk-"; break;
		case APP_UPDATE: filename = "appupdate-"; break;
		case SDK: filename = "sdk-"; break;
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

bool Install(MSIHANDLE hInstall, SharedDependency dependency)
{
	string componentInstallPath = FileUtils::GetSystemRuntimeHomeDirectory();
	string destination, updateFile;
	if (dependency->type == MODULE)
	{
		destination = FileUtils::Join(
			componentInstallPath.c_str(), "modules", OS_NAME,
			dependency->name.c_str(), dependency->version.c_str(), 0);
	}
	else if (dependency->type == RUNTIME)
	{
		destination = FileUtils::Join(
			componentInstallPath.c_str(), "runtime", OS_NAME,
			dependency->version.c_str(), 0);
	}
	else if (dependency->type == SDK || dependency->type == MOBILESDK)
	{
		destination = componentInstallPath;
	}
	else if (dependency->type == APP_UPDATE)
	{
		wstring params = MsiProperty(hInstall, L"CustomActionData");
		vector<wstring> tokens;
		Split(params, L';', tokens);

		updateFile = WideToUTF8(tokens[1]);
		destination = WideToUTF8(tokens[2]);
	}
	else
	{
		return false;
	}

	// Recursively create directories
	UnzipProgressData *data = new UnzipProgressData();
	data->hInstall = hInstall;
	data->dependency = dependency;
	FileUtils::CreateDirectory(destination, true);

	string utf8Path = WideToUTF8(GetFilePath(dependency));
	bool success = FileUtils::Unzip(
		utf8Path, destination, &UnzipProgress, (void*)data);

	if (success && dependency->type == APP_UPDATE)
	{
		FileUtils::DeleteFile(updateFile);
	}

	//delete data;
	return success;
}

static HWND GetInstallerHWND()
{
	HWND hwnd = FindWindowW(L"MsiDialogCloseClass", NULL);
	if (!hwnd)
		hwnd = GetActiveWindow();

	return hwnd;
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

bool DownloadDependency(MSIHANDLE hInstall, HINTERNET hINet, SharedDependency dependency)
{
	SharedApplication app = CreateApplication(hInstall);
	wstring url(UTF8ToWide(app->GetURLForDependency(dependency)));
	wstring outFilename(GetFilePath(dependency));
	
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
			ostringstream str;
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
		UINT result = Progress(hInstall, dependency, L"Downloading ",
			floor(((double)total/(double)contentLength)*100));
		if (result == IDCANCEL)
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

bool ProcessDependency(MSIHANDLE hInstall, PMSIHANDLE hProgressRec,
	HINTERNET hINet, SharedDependency dependency)
{
	UINT result = MsiProcessMessage(hInstall, INSTALLMESSAGE_PROGRESS, hProgressRec);
	if ((result == IDCANCEL))
		return false;
	if (!DownloadDependency(hInstall, hINet, dependency))
		return false;
	if (!Install(hInstall, dependency))
		return false;

	return true;
}

// inspired by http://msdn.microsoft.com/en-us/library/aa367525(VS.85).aspx
// also see http://msdn.microsoft.com/en-us/library/aa370354(VS.85).aspx for INSTALLMESSAGE_PROGRESS message values
extern "C" UINT __stdcall
NetInstall(MSIHANDLE hInstall)
{
	// Tell the installer to check the installation state and execute
	// the code needed during the rollback, acquisition, or
	// execution phases of the installation.
	vector<SharedDependency> unresolved = FindUnresolvedDependencies(hInstall);

	PMSIHANDLE hActionRec = MsiCreateRecord(3);
	PMSIHANDLE hProgressRec = MsiCreateRecord(3);

	// Installer is executing the installation script. Set up a
	// record specifying appropriate templates and text for
	// messages that will inform the user about what the custom
	// action is doing. Tell the installer to use this template and
	// text in progress messages.

	MsiRecordSetString(hActionRec, 1, TEXT("NetInstall"));
	MsiRecordSetString(hActionRec, 2, TEXT("Downloading dependencies..."));
	MsiRecordSetString(hActionRec, 3, TEXT("Downloading [4] [5] ([1] of [2]...)"));
	UINT result = MsiProcessMessage(hInstall, INSTALLMESSAGE_ACTIONSTART, hActionRec);
	if ((result == IDCANCEL))
		return ERROR_INSTALL_USEREXIT;

	// Tell the installer to use explicit progress messages.
	MsiRecordSetInteger(hProgressRec, 1, 1);
	MsiRecordSetInteger(hProgressRec, 2, 1);
	MsiRecordSetInteger(hProgressRec, 3, 0);
	result = MsiProcessMessage(hInstall, INSTALLMESSAGE_PROGRESS, hProgressRec);
	if ((result == IDCANCEL))
		return ERROR_INSTALL_USEREXIT;

	//Specify that an update of the progress bar's position in
	//this case means to move it forward by one increment.
	MsiRecordSetInteger(hProgressRec, 1, 2);
	MsiRecordSetInteger(hProgressRec, 2, 1);
	MsiRecordSetInteger(hProgressRec, 3, 0);

	// The following loop sets up the record needed by the action
	// messages and tells the installer to send a message to update
	// the progress bar.

	MsiRecordSetInteger(hActionRec, 2, unresolved.size());

	// Initialize the Interent DLL
	HINTERNET hINet = InternetOpenW(
		L"Mozilla/5.0 (compatible; Titanium_Downloader/0.1; Win32)",
		INTERNET_OPEN_TYPE_PRECONFIG, 0, 0, 0);
	if (!hINet)
	{
		string error(Win32Utils::QuickFormatMessage(GetLastError()));
		error = string("Could not open Internet connection: ") + error;
		ShowError(error);
		return ERROR_INSTALL_FAILURE;
	}

	// Install app updates and SDKs first.
	// If the (non-mobile) SDK is listed, we need to ignore runtime+modules below
	bool sdkInstalled = false;
	for (size_t i = 0; i < unresolved.size(); i++)
	{
		SharedDependency dep = unresolved.at(i);
		if (dep->type == SDK || dep->type == MOBILESDK || dep->type == APP_UPDATE)
		{
			if (!ProcessDependency(hInstall, hProgressRec, hINet, dep))
			{
				InternetCloseHandle(hINet);
				return ERROR_INSTALL_USEREXIT;
			}

			if (dep->type == SDK)
				sdkInstalled = true;
		}
	}

	if (!sdkInstalled)
	{
		for (size_t i = 0; i < unresolved.size(); i++)
		{
			SharedDependency dep = unresolved.at(i);
			if (dep->type != SDK && dep->type != MOBILESDK &&
				dep->type != APP_UPDATE && !sdkInstalled)
			{
				if (!ProcessDependency(hInstall, hProgressRec, hINet, dep))
				{
					InternetCloseHandle(hINet);
					return ERROR_INSTALL_USEREXIT;
				}
			}
		}
	}

	InternetCloseHandle(hINet);
	return ERROR_SUCCESS;
}

extern "C" UINT __stdcall
Clean(MSIHANDLE hInstall)
{
	wchar_t dir[MAX_PATH];
	DWORD dirSize = MAX_PATH;
	MsiGetTargetPath(hInstall, L"INSTALLDIR", dir, &dirSize);
	wstring installDir(dir, dirSize);

	string installDirUtf8(WideToUTF8(installDir));
	FileUtils::DeleteDirectory(installDirUtf8);

	return ERROR_SUCCESS;
}
