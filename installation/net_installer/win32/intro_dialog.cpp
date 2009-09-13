#include <windows.h>
#include <new.h>
#include <objbase.h>
#include <vector>
#include <string>
#include <io.h>
#include <stdlib.h>
#include <stdio.h>
#include <utils.h>
#include "progress_dialog.h"
#include "Resource.h"
#include "intro_dialog.h"
#include "api/utils/utils.h"

using std::string;
using std::wstring;
using KrollUtils::SharedApplication;
using KrollUtils::Application;
using namespace KrollUtils;

extern HICON mainIcon;

extern SharedApplication app;
extern string updateFile;
extern string appPath;
extern string runtimeHome;
extern string appInstallPath;
extern bool doInstall;
extern bool installStartMenuIcon;
extern bool forceInstall;

namespace ti
{
	IntroDialog::IntroDialog() : Dialog()
	{
		Create(IDD_INTRODIALOG);
	}
	
	void IntroDialog::Initialize(HWND hwnd)
	{
		nameLabel = GetDlgItem(hwnd, IDC_NAME_LABEL);
		versionLabel = GetDlgItem(hwnd, IDC_VERSION_LABEL);
		publisherLabel = GetDlgItem(hwnd, IDC_PUBLISHER_LABEL);
		urlLabel = GetDlgItem(hwnd, IDC_URL_LABEL);
		licenseTextBox = GetDlgItem(hwnd, IDC_LICENSETEXT);
		licenseBlurb = GetDlgItem(hwnd, IDC_LICENSE_BLURB);
		installBox = GetDlgItem(hwnd, IDC_INSTALL_BOX);
		installLocationText = GetDlgItem(hwnd, IDC_INSTALL_LOCATION_EDIT);
		installLocationButton = GetDlgItem(hwnd, IDC_INSTALL_LOCATION_BUTTON);
		startMenuCheck = GetDlgItem(hwnd, IDC_START_MENU_CHECK);
		securityBlurb = GetDlgItem(hwnd, IDC_SECURITY_BLURB);
		runButton = GetDlgItem(hwnd, IDC_RUN);
		cancelButton = GetDlgItem(hwnd, IDC_CANCEL);

		// Set the name label's font to be a bit bigger
		LOGFONT newFontStruct;
		HFONT currentFont = (HFONT) SendMessage(nameLabel, WM_GETFONT, (WPARAM)0, (LPARAM)0);
		GetObject(currentFont, sizeof newFontStruct, &newFontStruct);
		newFontStruct.lfWeight = FW_BOLD;
		newFontStruct.lfHeight = 30;
		HFONT newFont = CreateFontIndirect(&newFontStruct);
		SendMessage(nameLabel, WM_SETFONT, (WPARAM)newFont, LPARAM(0));

		std::wstring wideAppName = KrollUtils::UTF8ToWide(app->name);
		SendMessage(nameLabel, WM_SETTEXT, 0, (LPARAM) wideAppName.c_str());

		string version = "Unknown";
		if (!app->version.empty())
			version = app->version;
		if (!updateFile.empty())
			version.append(" (Update)");

		std::wstring wideVersion = KrollUtils::UTF8ToWide(app->version);
		SendMessage(versionLabel, WM_SETTEXT, 0, (LPARAM) wideVersion.c_str());

		if (!app->publisher.empty())
		{
			std::wstring widePublisher = KrollUtils::UTF8ToWide(app->publisher);
			SendMessage(publisherLabel, WM_SETTEXT, 0, (LPARAM) widePublisher.c_str());
		}
		if (!app->url.empty())
		{
			std::wstring wideURL = KrollUtils::UTF8ToWide(app->url);
			SendMessage(urlLabel, WM_SETTEXT, 0, (LPARAM) wideURL.c_str());
		}

		// Set license text
		string licenseText = app->GetLicenseText();
		if (licenseText.empty() || app->IsInstalled())
		{
			::ShowWindow(licenseTextBox , SW_HIDE);
			::ShowWindow(licenseBlurb , SW_HIDE);

			// *wince* -- I can't believe I'm about to do this
			int width = 530;
			int height = 270;
			SetWindowPos(hwnd, NULL, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);

			int installBoxX = 10;
			int installBoxY = 110;
			SetWindowPos(installBox, NULL, installBoxX, installBoxY, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
			SetWindowPos(installLocationText, NULL, installBoxX + 8, installBoxY + 17, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
			SetWindowPos(installLocationButton, NULL, width - 73, installBoxY + 16, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
			SetWindowPos(startMenuCheck, NULL, installBoxX + 12, installBoxY + 45, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
			SetWindowPos(securityBlurb, NULL, installBoxX + 25, installBoxY + 70, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
			
			SetWindowPos(runButton, NULL, width - 90, installBoxY + 95, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
			SetWindowPos(cancelButton, NULL, width - 175, installBoxY + 95, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
		}
		else
		{
			std::wstring wideLicense = KrollUtils::UTF8ToWide(licenseText);
			SendMessage(licenseTextBox, WM_SETTEXT, 0, (LPARAM) wideLicense.c_str());
		}

		// Hide installation location controls when this isn't a full app installation
		if (!forceInstall && app->IsInstalled())
		{
			::ShowWindow(installLocationButton , SW_HIDE);
			::ShowWindow(startMenuCheck , SW_HIDE);
		}

		std::wstring wideAppInstallPath = KrollUtils::UTF8ToWide(appInstallPath);
		SendMessage(installLocationText, WM_SETTEXT, 0, (LPARAM) wideAppInstallPath.c_str());

		// Set intro dialog icon
		SendMessage(hwnd, WM_SETICON, (WPARAM)true, (LPARAM)mainIcon);

		Dialog::Initialize(hwnd);
	}

	void IntroDialog::ButtonClicked(int id)
	{
		switch (id) {
			case IDC_CANCEL:
				this->Destroy();
				break;
			case IDC_RUN: {
				doInstall = true;
				LRESULT checked = SendMessage(startMenuCheck, BM_GETCHECK, 0, 0);
				if (checked == BST_CHECKED)
					installStartMenuIcon = true;
				this->Destroy();
			} break;
			case IDC_INSTALL_LOCATION_BUTTON:
				this->InstallLocationClicked();
				break;
		}
	}
	
	void IntroDialog::InstallLocationClicked()
	{
		BROWSEINFO bi = { 0 };
		bi.lpszTitle =  (LPCWSTR) L"Pick installation directory";
		LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
		if (pidl != 0)
		{
			TCHAR pathW[MAX_PATH];
			if (SHGetPathFromIDList(pidl, pathW))
			{
				std::string path = KrollUtils::WideToUTF8(pathW);
				appInstallPath = FileUtils::Join(path.c_str(), app->name.c_str(), NULL);
				std::wstring wideAppInstallPath = KrollUtils::UTF8ToWide(appInstallPath);
				SendMessage(installLocationText, WM_SETTEXT, 0, (LPARAM) wideAppInstallPath.c_str());
			}

			IMalloc * imalloc = 0;
			if (SUCCEEDED(SHGetMalloc(&imalloc)))
			{
				imalloc->Free(pidl);
				imalloc->Release();
			}

		}
	}
}
