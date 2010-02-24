/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009-2010 Appcelerator, Inc. All Rights Reserved.
 */
#include <kroll/utils/utils.h>
using namespace KrollUtils;
using KrollUtils::Application;
using KrollUtils::SharedApplication;
using KrollUtils::KComponentType;
using std::wstring;
using std::string;

#include <msi.h>
#include <msiquery.h>
#include <Wininet.h>
#include <cmath>
#include <sstream>
#include "common.h"

#pragma comment(linker, "/EXPORT:NetInstall=_NetInstall@4")
#pragma comment(linker, "/EXPORT:Clean=_Clean@4")

wstring MsiProperty(MSIHANDLE hInstall, const wchar_t* property)
{
	wchar_t buffer[4096];
	DWORD bufferLength = 4096;
	MsiGetProperty(hInstall, property, buffer, &bufferLength);
	return wstring(buffer, bufferLength);
}

vector<wstring>& Split(const wstring& s, wchar_t delim, vector<wstring>& elems)
{
	std::wstringstream ss(s);
	wstring item;
	while (getline(ss, item, delim))
	{
		elems.push_back(item);
	}
	return elems;
}

HWND GetInstallerHWND()
{
	HWND hwnd = FindWindowW(L"MsiDialogCloseClass", NULL);
	if (!hwnd)
		hwnd = GetActiveWindow();

	return hwnd;
}


SharedApplication CreateApplication(MSIHANDLE hInstall)
{
	wstring params = MsiProperty(hInstall, L"CustomActionData");
	vector<wstring> tokens;
	Split(params, L';', tokens);

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

vector<SharedDependency> FindUnresolvedDependencies(MSIHANDLE hInstall)
{
	// Tell the installer to check the installation state and execute
	// the code needed during the rollback, acquisition, or
	// execution phases of the installation.
	vector<SharedDependency> unresolved;
	vector<SharedComponent> components;
	vector<SharedComponent>& installedComponents =
		BootUtils::GetInstalledComponents(true);
	for (size_t i = 0; i < installedComponents.size(); i++)
	{
		components.push_back(installedComponents.at(i));
	}

	wstring dependencies, bundledModules, bundledRuntime, installDir;
	vector<wstring> tokens;
	// deferred / async mode, get from the hacked "CustomActionData" property
	if (MsiGetMode(hInstall, MSIRUNMODE_SCHEDULED) == TRUE)
	{
		wstring params = MsiProperty(hInstall, L"CustomActionData");
		Split(params, L';', tokens);

		dependencies.assign(tokens[0]);
		if (tokens.size() > 1) bundledModules.assign(tokens[1]);
		if (tokens.size() > 2) bundledRuntime.assign(tokens[2]);
		
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


	// We cannot resolve dependencies in the normal way, since bundled modules
	// are still packed into the MSI and won't be found. Instead, go through
	// the unbundled modules line-by-line and try to resolve them like that.
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
		if (BootUtils::ResolveDependency(dependency, components).isNull())
		{
			unresolved.push_back(dependency);
		}
	}

	return unresolved;
}


// A helper function that sends a progress message to the installer, returns
// false if the user has cancelled the action.
static int currentProgress = 0;
static MSIHANDLE installHandle;
static bool isDownloading = false;
bool Progress(SharedDependency dependency, int percentage)
{
	// The download step represents the first 50% of the download+extract
	// action, so divide the progress in half here.
	percentage = percentage / 2;
	if (!isDownloading)
		percentage += 50;

	// If the progress hasn't increased, just bail here.
	if (percentage <= currentProgress)
		return true;

	PMSIHANDLE actionRecord = MsiCreateRecord(3);
	MsiRecordSetInteger(actionRecord, 1, percentage); // Current percentage.
	MsiRecordSetInteger(actionRecord, 2, 0); // Unused
	MsiRecordSetInteger(actionRecord, 3, 0); // Unused
	UINT result = MsiProcessMessage(installHandle, INSTALLMESSAGE_ACTIONDATA, actionRecord);
	if (result == IDCANCEL)
		return false;

	MsiRecordSetInteger(actionRecord, 1, 2); // Type of message: Increment the bar
	MsiRecordSetInteger(actionRecord, 2, (percentage - currentProgress)); // Number of ticks the bar has moved.
	MsiRecordSetInteger(actionRecord, 3, 0); // Unused
	result = MsiProcessMessage(installHandle, INSTALLMESSAGE_PROGRESS, actionRecord);
	if (result == IDCANCEL)
		return false;

	currentProgress = percentage;
	return true;
}

bool ProcessDependency(MSIHANDLE hInstall, SharedApplication app, SharedDependency dependency)
{

	// Set up the custom action description and template.
	wstring description(L"Downloading and extracting the ");
	if (dependency->type == MODULE)
		description.append(UTF8ToWide(dependency->name) + L" module");
	else if (dependency->type == SDK)
		description.append(L"SDK");
	else if (dependency->type == MOBILESDK)
		description.append(L"Mobile SDK");
	else if (dependency->type == APP_UPDATE)
		description.append(L"application update");
	else
		description.append(L"runtime");
	description += L" (" + UTF8ToWide(dependency->version) + L")";

	// TODO: The WIX GUI seems to ignore this line.
	wstring plate(L"About [1]% complete...");

	// See the long comment below about modifying this code.
	// Update the install message display. We've basically set up a template
	// above that looks like '[1]% complete". When we sent the INSTALLMESSAGE_ACTIONDATA
	// message, the '[1]' will turn into the value in the first message field.
	// We aren't using the other fields right now, but we may in the future.
	PMSIHANDLE record = MsiCreateRecord(3);
	MsiRecordSetString(record, 1, L"NetInstall"); // Custom action name
	MsiRecordSetString(record, 2, description.c_str()); // Description.
	MsiRecordSetString(record, 3, plate.c_str()); // Template for action data messages.
	UINT result = MsiProcessMessage(hInstall, INSTALLMESSAGE_ACTIONSTART, record);
	if (result == IDCANCEL)
		return ERROR_INSTALL_USEREXIT;

	// Excplicitly set the progress bar back to zero.
	MsiRecordSetInteger(record, 1, 0); // Type of message: Reset progress bar.
	MsiRecordSetInteger(record, 2, 100); // Total number of ticks to use.
	MsiRecordSetInteger(record, 3, 0); // Left-to-right progress bar.
	result = MsiProcessMessage(hInstall, INSTALLMESSAGE_PROGRESS, record);
	if (result == IDCANCEL)
		return ERROR_INSTALL_USEREXIT;

	currentProgress = 0;
	installHandle = hInstall;
	isDownloading = true;
	if (!DownloadDependency(app, dependency))
		return false;

	isDownloading = false;
	if (!InstallDependency(app, dependency))
		return false;

	return true;
}

extern "C" UINT __stdcall NetInstall(MSIHANDLE hInstall)
{
	// Wow, this is convoluted and easy to get wrong. It feels like a
	// scene out of Fear and Loathing in Las Vegas. Essentially we've been
	// consuming mind-altering substances for the better part of a day
	// (coffee, of course) and now we're in bat country.
	
	// All the information on these magic numbers can be found here:
	// http://msdn.microsoft.com/en-us/library/aa370354(VS.85).aspx
	// If that page disappears, we're basically all screwed, but you might
	// have luck Binging or whatever for: INSTALLMESSAGE_PROGRESS.
	
	// I pity you, I really do, if you need to change this code. Rest assured
	// though, if you add code here without documenting what every single
	// magic number does, I will build a time machine, travel into the future/past
	// as appropriate and engage in the most heinous psychological torture that
	// our busted legal system affords. Yes, that is a threat.
	
	// Tell the installer to use explicit progress messages and reset the progress bar.
	// Tell the installer to increase the value of the final total
	// length of the progress bar by the total number of ticks in
	// the custom action.
	if (MsiGetMode(hInstall, MSIRUNMODE_SCHEDULED) == TRUE)
	{
		PMSIHANDLE record = MsiCreateRecord(3);
		MsiRecordSetInteger(record, 1, 1); // Type of message: Information about progress.
		MsiRecordSetInteger(record, 2, 1); // Number of ticks to move per message.
		MsiRecordSetInteger(record, 3, 0); // We will send explicit progress message.
		UINT result = MsiProcessMessage(hInstall, INSTALLMESSAGE_PROGRESS, record);
		if (result == IDCANCEL)
			return ERROR_INSTALL_USEREXIT;
	}
	else
	{
		PMSIHANDLE hProgressRec = MsiCreateRecord(2);
		MsiRecordSetInteger(hProgressRec, 1, 3); // Type of messaeg: Enable this action to add ticks to the progress bar
		MsiRecordSetInteger(hProgressRec, 2, 100); // Number of ticks to add.
		UINT result = MsiProcessMessage(hInstall, INSTALLMESSAGE_PROGRESS, hProgressRec);
		if (result == IDCANCEL)
			return ERROR_INSTALL_USEREXIT;
		return ERROR_SUCCESS;
	}

	// If the SDK is listed, we need to ignore other non-SDK components below.
	SharedApplication app(CreateApplication(hInstall));
	vector<SharedDependency> unresolved = FindUnresolvedDependencies(hInstall);
	bool sdkInstalled = false;
	for (size_t i = 0; i < unresolved.size(); i++)
	{
		SharedDependency dep = unresolved.at(i);
		if (dep->type == SDK || dep->type == MOBILESDK)
		{
			if (!ProcessDependency(hInstall, app, dep))
			{
				ShutdownNetConnection();
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
				if (!ProcessDependency(hInstall, app, dep))
				{
					ShutdownNetConnection();
					return ERROR_INSTALL_USEREXIT;
				}
			}
		}
	}

	ShutdownNetConnection();
	return ERROR_SUCCESS;
}

extern "C" UINT __stdcall Clean(MSIHANDLE hInstall)
{
	wchar_t dir[MAX_PATH];
	DWORD dirSize = MAX_PATH;
	MsiGetTargetPath(hInstall, L"INSTALLDIR", dir, &dirSize);
	wstring installDir(dir, dirSize);

	string installDirUtf8(WideToUTF8(installDir));
	FileUtils::DeleteDirectory(installDirUtf8);

	return ERROR_SUCCESS;
}

bool WcaProgressMessage(MSIHANDLE installHandle, __in UINT uiCost, __in BOOL fExtendProgressBar)
{
    static BOOL fExplicitProgressMessages = FALSE;
    MSIHANDLE hRec = ::MsiCreateRecord(3);
    if (!fExtendProgressBar && !fExplicitProgressMessages)
    {
        ::MsiRecordSetInteger(hRec, 1, 1);
        ::MsiRecordSetInteger(hRec, 2, 1);
        ::MsiRecordSetInteger(hRec, 3, 0);

		UINT er = ::MsiProcessMessage(installHandle, INSTALLMESSAGE_PROGRESS, hRec);
        if (IDABORT == er || IDCANCEL == er)
			return false;

        fExplicitProgressMessages = TRUE;
    }

    // send the progress message
    ::MsiRecordSetInteger(hRec, 1, (fExtendProgressBar) ? 3 : 2);
    ::MsiRecordSetInteger(hRec, 2, uiCost);
    ::MsiRecordSetInteger(hRec, 3, 0);

	UINT er = ::MsiProcessMessage(installHandle, INSTALLMESSAGE_PROGRESS, hRec);
    if (IDABORT == er || IDCANCEL == er)
        return false;

    return true;
}


