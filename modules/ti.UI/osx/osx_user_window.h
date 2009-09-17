/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */
#ifndef OSX_USER_WINDOW_H
#define OSX_USER_WINDOW_H
namespace ti
{
	class OSXUIBinding;
	class OSXMenuItem;

	class OSXUserWindow : public UserWindow
	{
		public:
			OSXUserWindow(WindowConfig* config, AutoUserWindow& parent);
			~OSXUserWindow();
		public:

			void OpenChooserDialog(
				bool files,
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
			bool IsFullscreen();
			std::string GetId();
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
			void ReconfigureWindowConstraints();
			double GetMinHeight();
			void SetMinHeight(double height);
			Bounds GetBounds();
			void SetBounds(Bounds bounds);
			std::string GetTitle();
			void SetTitleImpl(std::string& title);
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
			void SetFullscreen(bool fullscreen);
			bool IsTopMost();
			void SetTopMost(bool topmost);

			void SetMenu(AutoMenu menu);
			AutoMenu GetMenu();
			void SetContextMenu(AutoMenu menu);
			AutoMenu GetContextMenu();
			void SetIcon(std::string& iconPath);
			std::string& GetIcon();

			NativeWindow* GetNative() { return nativeWindow; }
			void Focused();
			void Unfocused();
			
			virtual void ShowInspector(bool console=false);

		private:
			NativeWindow *nativeWindow;
			bool focused;
			AutoPtr<OSXMenu> menu;
			AutoPtr<OSXMenu> contextMenu;
			AutoPtr<OSXUIBinding> osxBinding;
			static bool initial;
			std::string iconPath;

			NSScreen* GetWindowScreen();
			NSRect CalculateWindowFrame(double, double, double, double);
			DISALLOW_EVIL_CONSTRUCTORS(OSXUserWindow);
	};
}

#endif
