/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */
#ifndef OSX_USER_WINDOW_H
#define OSX_USER_WINDOW_H

#import "preinclude.h"
#import <WebKit/WebKit.h>
#import "../menu_item.h"
#import "../user_window.h"
#import "native_window.h"

namespace ti
{
	class OSXUIBinding;
	class OSXMenuItem;

	class OSXUserWindow : public UserWindow
	{
		public:
			OSXUserWindow(WindowConfig* config, SharedUserWindow& parent);
			~OSXUserWindow();
		public:

			void OSXUserWindow::OpenChooserDialog(
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
			bool IsFullScreen();
			std::string GetId();
			void Open();
			void Close();
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
			void SetIcon(SharedString icon_path);
			SharedString GetIcon();

			NativeWindow* GetNative() { return window; }
			void Focused();
			void Unfocused();

		private:
			NativeWindow *window;
			bool opened;
			bool closed;
			bool topmost;
			bool focused;
			SharedPtr<MenuItem> menu;
			SharedPtr<MenuItem> context_menu;
			SharedPtr<OSXUIBinding> osx_binding;
			static bool initial;

			NSScreen* GetWindowScreen();
			NSRect CalculateWindowFrame(double, double, double, double);
			double real_x;
			double real_y;
			double real_w;
			double real_h;

			void InstallMenu(OSXMenuItem *menu);

			DISALLOW_EVIL_CONSTRUCTORS(OSXUserWindow);
	};
}

#endif
