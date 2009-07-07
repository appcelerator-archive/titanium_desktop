/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#define _WINSOCKAPI_
#include <kroll/base.h>

#include <winsock2.h> 
#include "../ui_module.h"
#include "win32_menu_item_impl.h"

#include <windows.h>
#include <commdlg.h>
#include <shellapi.h>
#include <shlobj.h>
#include <string>
#include <stdlib.h>
#include "win32_tray_item.h"

#include "../url/url_curl.h"

namespace ti
{
	HMENU Win32UIBinding::contextMenuInUseHandle = NULL;


	Win32UIBinding::Win32UIBinding(Module *uiModule, Host *host) :
		UIBinding(host),
		evaluator(host),
		menu(0),
		contextMenu(0),
		iconPath("")
	{
	
		if (!Win32UIBinding::IsWindowsXP())
		{
			// Use Activation Context API by pointing it at the WebKit
			// manifest. This should allos us to load our COM object.
			ACTCTX actctx; 
			ZeroMemory(&actctx, sizeof(actctx)); 
			actctx.cbSize = sizeof(actctx); 

			std::string source = host->GetRuntimePath();
			source = FileUtils::Join(source.c_str(), "WebKit.manifest", NULL);
			actctx.lpSource = source.c_str(); // Path to the Side-by-Side Manifest File 
			this->pActCtx = CreateActCtx(&actctx); 
			ActivateActCtx(pActCtx, &this->lpCookie);
		}
		
		INITCOMMONCONTROLSEX InitCtrlEx;

		InitCtrlEx.dwSize = sizeof(INITCOMMONCONTROLSEX);
		InitCtrlEx.dwICC = 0x00004000; //ICC_STANDARD_CLASSES;
		InitCommonControlsEx(&InitCtrlEx);
		
		InitCurl(uiModule);
		addScriptEvaluator(&evaluator);
	}
	
	void Win32UIBinding::InitCurl(Module* uiModule)
	{
		std::string pemPath = FileUtils::Join(uiModule->GetPath().c_str(), "cacert.pem", NULL);
		
		// use _putenv since webkit uses stdlib's getenv which is incompatible with GetEnvironmentVariable
		std::string var = "CURL_CA_BUNDLE_PATH=" + pemPath;
		_putenv(var.c_str());

		curl_register_local_handler(&CurlTiURLHandler);
		curl_register_local_handler(&CurlAppURLHandler);
	}

	Win32UIBinding::~Win32UIBinding()
	{
		if (!Win32UIBinding::IsWindowsXP())
		{
   			DeactivateActCtx(0, this->lpCookie); 
			ReleaseActCtx(this->pActCtx);
		}
	}

	bool Win32UIBinding::IsWindowsXP()
	{
		OSVERSIONINFO osVersion;
		osVersion.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		::GetVersionEx(&osVersion);
		return osVersion.dwMajorVersion == 5;
	}

	SharedUserWindow Win32UIBinding::CreateWindow(
		WindowConfig* config,
		SharedUserWindow& parent)
	{
		UserWindow* w = new Win32UserWindow(config, parent);
		return w->GetSharedPtr();
	}

	void Win32UIBinding::ErrorDialog(std::string msg)
	{
		MessageBox(NULL,msg.c_str(),"Application Error",MB_OK|MB_ICONERROR|MB_SYSTEMMODAL);
		UIBinding::ErrorDialog(msg);
	}

	SharedMenuItem Win32UIBinding::CreateMenuItem(
		std::string label, SharedKMethod callback, std::string iconURL)
	{
		return new Win32MenuItem(MenuItem::NORMAL, label, callback, iconURL);
	}

	SharedMenuItem Win32UIBinding::CreateSeparatorMenuItem()
	{
		return new Win32MenuItem(MenuItem::SEPARATOR, std::string(), NULL, std::string());
	}

	SharedMenuItem Win32UIBinding::CreateCheckMenuItem(
		std::string label, SharedKMethod callback)
	{
		return new Win32MenuItem(MenuItem::CHECK, label, callback, std::string());
	}

	void Win32UIBinding::SetMenu(SharedPtr<MenuItem>)
	{
		this->menu = newMenu.cast<Win32Menu>();

		// Notify all windows that the app menu has changed.
		std::vector<SharedUserWindow>& windows = this->GetOpenWindows();
		std::vector<SharedUserWindow>::iterator i = windows.begin();
		while (i != windows.end())
		{
			SharedPtr<Win32UserWindow> wuw = (*i).cast<Win32UserWindow>();
			if (!wuw.isNull())
				wuw->AppMenuChanged();

			i++;
		}
	}

	void Win32UIBinding::SetContextMenu(SharedMenu menu)
	{
		this->contextMenu = newMenu.cast<Win32Menu>();
	}

	void Win32UIBinding::SetIcon(std::string& iconPath)
	{
		this->iconPath = iconPath;

		// Notify all windows that the app icon has changed
		// TODO this kind of notification should really be placed in UIBinding..
		std::vector<SharedUserWindow>& windows = this->GetOpenWindows();
		std::vector<SharedUserWindow>::iterator i = windows.begin();
		while (i != windows.end())
		{
			SharedPtr<Win32UserWindow> wuw = (*i).cast<Win32UserWindow>();
			if (!wuw.isNull())
				wuw->AppIconChanged();

			i++;
		}
	}

	SharedPtr<TrayItem> Win32UIBinding::AddTray(
		SharedString icon_path,
		SharedKMethod cb)
	{
		SharedPtr<TrayItem> trayItem = new Win32TrayItem(icon_path, cb);
		return trayItem;
	}

	long Win32UIBinding::GetIdleTime()
	{
		LASTINPUTINFO lii;
		memset(&lii, 0, sizeof(lii));

		lii.cbSize = sizeof(lii);
		::GetLastInputInfo(&lii);

		DWORD currentTickCount = GetTickCount();
		long idleTicks = currentTickCount - lii.dwTime;

		return (int)idleTicks;
	}

	SharedMenu GtkUIBinding::GetMenu()
	{
		return this->menu;
	}

	SharedMenu GtkUIBinding::GetContextMenu()
	{
		return this->contextMenu;
	}

	std::string& GtkUIBinding::GetIcon()
	{
		return this->iconPath;
	}
}
