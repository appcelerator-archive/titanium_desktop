/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */
#ifndef __TI_WIN32_USER_WINDOW_H
#define __TI_WIN32_USER_WINDOW_H
namespace ti
{
	class Win32UserWindow : public UserWindow
	{
		public:
		Win32UserWindow(WindowConfig* config, AutoUserWindow& parent);
		virtual ~Win32UserWindow();
	
		void OpenFileChooserDialog(
			SharedKMethod callback,
			bool multiple,
			std::string& title,
			std::string& path,
			std::string& defaultName,
			std::vector<std::string>& types,
			std::string& typesDescription);
		void OpenFolderChooserDialog(
			SharedKMethod callback,
			bool multiple,
			std::string& title,
			std::string& path,
			std::string& defaultName);
		void OpenSaveAsDialog(
			SharedKMethod callback,
			std::string& title,
			std::string& path,
			std::string& defaultName,
			std::vector<std::string>& types,
			std::string& typesDescription);
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
		Bounds GetBounds();
		void SetBounds(Bounds bounds);
		void SetTitleImpl(std::string& title);
		void SetURL(std::string& url);
		void SetResizable(bool resizable);
		void SetMaximizable(bool maximizable);
		void SetMinimizable(bool minimizable);
		void SetCloseable(bool closeable);
		bool IsVisible();
		void SetTransparency(double transparency);
		std::string GetTransparencyColor();
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
		static void RegisterWindowClass(HINSTANCE hInstance);
		static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		static Win32UserWindow* FromWindow(HWND hWnd);
		void RedrawMenu();
		static void RedrawAllMenus();

		virtual void AppIconChanged();
		virtual void AppMenuChanged();
		IWebView* GetWebView() { return webView; };
	
		protected:
		kroll::Win32Host *win32Host;
		Win32WebKitFrameLoadDelegate *frameLoadDelegate;
		Win32WebKitUIDelegate *uiDelegate;
		Win32WebKitPolicyDelegate *policyDelegate;
		
		Bounds restoreBounds;
		long restoreStyles;
		int chromeWidth, chromeHeight;
	
		HWND windowHandle, viewWindowHandle;
		IWebView* webView;
		IWebFrame *webFrame;
		IWebInspector *webInspector;
		bool requiresDisplay;
	
		AutoPtr<Win32Menu> menu; // The window-specific menu
		AutoPtr<Win32Menu> activeMenu; // This window's active menu
		HMENU nativeMenu; // This window's active native menu
		AutoPtr<Win32Menu> contextMenu; // This window-specific context menu
		std::string iconPath; // The path to this window's icon
		HICON defaultIcon;
	
		void RemoveOldMenu();
		void ReloadTiWindowConfig();
		void InitWindow();
		void InitWebKit();
		void SetupBounds();
		void SetupPosition();
		void SetupSize();
		void SetupDecorations(bool showHide = true);
		void SetupMenu();
		void SetupIcon();
		SharedKList SelectFile(
			bool saveDialog, bool multiple, std::string& title,
			std::string& path, std::string& defaultName,
			std::vector<std::string>& types, std::string& typesDescription);
		SharedKList SelectDirectory(
			bool multiple, std::string& title, std::string& path, std::string& defaultName);
		static void ParseStringNullSeparated(const wchar_t *s, std::vector<std::string> &tokens);
		Logger* logger;

		public:
		// Implementations that depend on instance variables
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
		std::string GetId() { return config->GetID(); }
	};
}
#endif
