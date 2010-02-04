/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008-2010 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef __TI_WIN32_USER_WINDOW_H
#define __TI_WIN32_USER_WINDOW_H

namespace ti
{
	class Win32UserWindow : public UserWindow
	{
	public:
		Win32UserWindow(AutoPtr<WindowConfig> config, AutoUserWindow& parent);
		~Win32UserWindow();

		void OpenFileChooserDialog(KMethodRef callback, bool multiple,
			std::string& title, std::string& path, std::string& defaultName,
			std::vector<std::string>& types, std::string& typesDescription);
		void OpenFolderChooserDialog( KMethodRef callback, bool multiple,
			std::string& title, std::string& path, std::string& defaultName);
		void OpenSaveAsDialog( KMethodRef callback, std::string& title,
			std::string& path, std::string& defaultName,
			std::vector<std::string>& types, std::string& typesDescription);
		void ResizeSubViews();
		HWND GetWindowHandle();
		void Hide();
		void Show();
		void Minimize();
		void Maximize();
		void Unminimize();
		void Unmaximize();
		bool IsMinimized();
		bool IsMaximized();
		void Focus();
		void Unfocus();
		void Open();
		bool Close();
		double GetX();
		void SetX(double x);
		double GetY();
		void SetY(double y);
		double GetWidth();
		void SetWidth(double width);
		double GetMaxWidth();
		void SetMaxWidth(double width);
		double GetMinWidth();
		void SetMinWidth(double width);
		double GetHeight();
		void SetHeight(double height);
		double GetMaxHeight();
		void SetMaxHeight(double height);
		double GetMinHeight();
		void SetMinHeight(double height);
		Bounds GetBoundsImpl();
		void SetBoundsImpl(Bounds bounds);
		void SetTitleImpl(std::string& title);
		void SetURL(std::string& url);
		void SetResizableImpl(bool resizable);
		void SetMaximizable(bool maximizable);
		void SetMinimizable(bool minimizable);
		void SetCloseable(bool closeable);
		bool IsVisible();
		void SetTransparency(double transparency);
		void SetFullscreen(bool fullscreen);
		void SetUsingChrome(bool chrome);
		void SetMenu(AutoMenu menu);
		AutoMenu GetMenu();
		void SetContextMenu(AutoMenu menu);
		AutoMenu GetContextMenu();
		void SetIcon(std::string&);
		std::string& GetIcon();
		bool IsTopMost();
		void SetTopMost(bool topmost);
		void FrameLoaded();
		void ShowInspector(bool console);
		static Win32UserWindow* FromWindow(HWND hWnd);
		static AutoPtr<Win32UserWindow> FromWebView(IWebView *webView);

		void RedrawMenu();
		static void RedrawAllMenus();
		virtual void AppIconChanged();
		virtual void AppMenuChanged();

		IWebView* GetWebView() { return webView; };
		std::string GetTitle() { return config->GetTitle(); }
		std::string GetURL() { return config->GetURL(); }
		bool IsResizable() { return config->IsResizable(); }
		bool IsMaximizable() { return config->IsMaximizable(); }
		bool IsMinimizable() { return config->IsMinimizable(); }
		bool IsCloseable() { return config->IsCloseable(); }
		double GetTransparency() { return config->GetTransparency(); }
		bool IsUsingChrome() { return config->IsUsingChrome(); }
		bool IsUsingScrollbars() { return config->IsUsingScrollbars(); }
		bool IsFullscreen() { return config->IsFullscreen(); }
		void SetBitmap(HBITMAP bitmap) { this->webkitBitmap = bitmap; }
		UINT_PTR GetTimer() { return this->timer; }
		void UpdateBitmap();
		void GetMinMaxInfo(MINMAXINFO* minMaxInfo);

	private:
		kroll::Win32Host* win32Host;
		Win32WebKitFrameLoadDelegate* frameLoadDelegate;
		Win32WebKitUIDelegate* uiDelegate;
		Win32WebKitPolicyDelegate* policyDelegate;
		Win32WebKitResourceLoadDelegate* resourceLoadDelegate;
		Bounds restoreBounds;
		long restoreStyles;
		HWND windowHandle;
		HWND viewWindowHandle;
		HBITMAP webkitBitmap;
		UINT_PTR timer;
		IWebView* webView;
		IWebFrame* mainFrame;
		IWebInspector* webInspector;
		Bounds chromeSize;

		// Set this flag to indicate that when the frame is loaded we want to
		// show the window - we do this to prevent white screen while the first
		// URL loads in the WebView.
		bool requiresDisplay;
		AutoPtr<Win32Menu> menu; // The window-specific menu
		AutoPtr<Win32Menu> activeMenu; // This window's active menu
		HMENU nativeMenu; // This window's active native menu
		AutoPtr<Win32Menu> contextMenu; // This window-specific context menu
		std::string iconPath; // The path to this window's icon

		void RemoveOldMenu();
		DWORD GetStyleFromConfig();
		void InitWindow();
		void InitWebKit();
		void SetupFrame();
		void SetupDecorations();
		void SetupState();
		void SetupMenu();
		void SetupIcon();

		KListRef SelectFile(
			bool saveDialog, bool multiple, std::string& title,
			std::string& path, std::string& defaultName,
			std::vector<std::string>& types, std::string& typesDescription);
		KListRef SelectDirectory(bool multiple, std::string& title,
			std::string& path, std::string& defaultName);
		static void ParseSelectedFiles(const wchar_t *s,
			std::vector<std::string> &selectedFiles);
		Logger* logger;
	};
}
#endif
