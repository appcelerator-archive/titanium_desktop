/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */
#ifndef _GTK_USER_WINDOW_H_
#define _GTK_USER_WINDOW_H_

namespace ti
{
	namespace GtkUserWindowNS
	{
		enum FileChooserMode
		{
			SELECT_FILE,
			SELECT_FOLDER,
			SAVE_FILE
		};

		struct FileChooserJob
		{
			Host *host;
			GtkWindow* window;
			SharedKMethod callback;
			FileChooserMode mode;
			bool multiple;
			std::string title;
			std::string path;
			std::string defaultName;
			std::vector<std::string> types;
			std::string typesDescription;
		};
	}

	class GtkUserWindow : public UserWindow
	{

		public:
		GtkUserWindow(WindowConfig*, SharedUserWindow&);
		virtual ~GtkUserWindow();
		void Destroyed();
		void SetupDecorations();
		void SetupTransparency();
		void SetupSizeLimits();
		void SetupSize();
		void SetupPosition();
		void SetupMenu();
		void SetupIcon();
		void AppMenuChanged();
		void AppIconChanged();
		void RemoveOldMenu();

		void ShowFileChooser(
			GtkUserWindowNS::FileChooserMode mode,
			SharedKMethod callback,
			bool multiple,
			std::string& title,
			std::string& path,
			std::string& defaultName,
			std::vector<std::string>& types,
			std::string& typesDescription);

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
		bool IsUsingChrome();
		void SetUsingChrome(bool chrome);
		bool IsUsingScrollbars();
		bool IsFullScreen();
		std::string GetId();
		void Open();
		void Close();
		double GetX();
		void SetX(double x);
		double GetY();
		void SetY(double y);

		double GetWidth();
		void SetWidth(double width) ;
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
		std::string GetTitle();
		void SetTitle(std::string& title);
		std::string GetURL();
		void SetURL(std::string& url);
		bool IsResizable();
		void SetResizable(bool resizable);
		bool IsMaximizable();
		void SetMaximizable(bool maximizable);
		bool IsMinimizable();
		void SetMinimizable(bool minimizable);
		bool IsCloseable();
		void SetCloseable(bool closeable);
		bool IsVisible();
		double GetTransparency();
		void SetTransparency(double transparency);
		void SetFullScreen(bool fullscreen);
		bool IsTopMost();
		void SetTopMost(bool topmost);

		void SetMenu(SharedPtr<MenuItem> menu);
		SharedPtr<MenuItem> GetMenu();

		void SetContextMenu(SharedPtr<MenuItem> menu);
		SharedPtr<MenuItem> GetContextMenu();

		void SetIcon(SharedString iconPath);
		SharedString GetIcon();

		void SetInspectorWindow(GtkWidget* inspectorWindow);
		GtkWidget *GetInspectorWindow();

		int gdkWidth;
		int gdkHeight;
		int gdkX;
		int gdkY;
		bool gdkMaximized;
		bool gdkMinimized;

	protected:
		GtkWindow* gtkWindow;
		GtkWidget* vbox;
		WebKitWebView* webView;
		bool topmost;
		gulong destroyCallbackId;

		// The window-specific menu.
		SharedPtr<GtkMenuItemImpl> menu;

		// This window's menu -- may just be a pointer to the app menu
		SharedPtr<GtkMenuItemImpl> menuInUse;

		// The widget this window uses for a menu.
		GtkWidget* menuBar;

		// The path to this window's icon
		SharedString iconPath;

		// The widget this window uses for a context menu.
		SharedPtr<GtkMenuItemImpl> context_menu;

		// This window's web inspector window
		GtkWidget *inspectorWindow;

		void _FileChooserWork(const ValueList&, SharedValue);
		static std::string openFilesDirectory;

	};

}


#endif
