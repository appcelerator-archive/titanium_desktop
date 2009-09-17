/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _USER_WINDOW_H_
#define _USER_WINDOW_H_

#include <string>
#include <vector>
#include <map>
#include <kroll/kroll.h>

#ifdef OS_WIN32
#undef CreateWindow
#endif

#include "../ti.App/app_config.h"
#include "../ti.App/window_config.h"

namespace ti
{
	typedef struct {
		double x;
		double y;
		double width;
		double height;
	} Bounds;

	class UserWindow : public KEventObject {
		public:
			UserWindow(WindowConfig *config, AutoUserWindow& parent);
			virtual ~UserWindow();
			void UpdateWindowForURL(std::string url);
			Host* GetHost();

			inline SharedKObject GetDOMWindow() { return this->domWindow; }
			void _GetCurrentWindow(const kroll::ValueList&, kroll::SharedValue);
			void _GetDOMWindow(const kroll::ValueList&, kroll::SharedValue);
			void _InsertAPI(const kroll::ValueList&, kroll::SharedValue);
			void _Hide(const kroll::ValueList&, kroll::SharedValue);
			void _Show(const kroll::ValueList&, kroll::SharedValue);
			void _Minimize(const kroll::ValueList&, kroll::SharedValue);
			void _Maximize(const kroll::ValueList&, kroll::SharedValue);
			void _Unminimize(const kroll::ValueList&, kroll::SharedValue);
			void _Unmaximize(const kroll::ValueList&, kroll::SharedValue);
			void _IsMaximized(const kroll::ValueList&, kroll::SharedValue);
			void _IsMinimized(const kroll::ValueList&, kroll::SharedValue);
			void _Focus(const kroll::ValueList&, kroll::SharedValue);
			void _Unfocus(const kroll::ValueList&, kroll::SharedValue);
			void _IsUsingChrome(const kroll::ValueList&, kroll::SharedValue);
			void _SetUsingChrome(const kroll::ValueList&, kroll::SharedValue);
			void _IsUsingScrollbars(const kroll::ValueList&, kroll::SharedValue);
			void _IsFullscreen(const kroll::ValueList&, kroll::SharedValue);
			void _SetFullscreen(const kroll::ValueList&, kroll::SharedValue);
			void _GetId(const kroll::ValueList&, kroll::SharedValue);
			void _Open(const kroll::ValueList&, kroll::SharedValue);
			void _Close(const kroll::ValueList&, kroll::SharedValue);
			void _GetX(const kroll::ValueList&, kroll::SharedValue);
			double _GetX();
			void _SetX(const kroll::ValueList&, kroll::SharedValue);
			void _SetX(double x);
			void _GetY(const kroll::ValueList&, kroll::SharedValue);
			double _GetY();
			void _SetY(const kroll::ValueList&, kroll::SharedValue);
			void _SetY(double y);
			void _GetWidth(const kroll::ValueList&, kroll::SharedValue);
			double _GetWidth();
			void _SetWidth(const kroll::ValueList&, kroll::SharedValue);
			void _SetWidth(double width);
			void _GetMaxWidth(const kroll::ValueList&, kroll::SharedValue);
			void _SetMaxWidth(const kroll::ValueList&, kroll::SharedValue);
			void _GetMinWidth(const kroll::ValueList&, kroll::SharedValue);
			void _SetMinWidth(const kroll::ValueList&, kroll::SharedValue);
			void _GetHeight(const kroll::ValueList&, kroll::SharedValue);
			double _GetHeight();
			void _SetHeight(const kroll::ValueList&, kroll::SharedValue);
			void _SetHeight(double height);
			void _GetMaxHeight(const kroll::ValueList&, kroll::SharedValue);
			void _SetMaxHeight(const kroll::ValueList&, kroll::SharedValue);
			void _GetMinHeight(const kroll::ValueList&, kroll::SharedValue);
			void _SetMinHeight(const kroll::ValueList&, kroll::SharedValue);
			void _GetBounds(const kroll::ValueList&, kroll::SharedValue);
			void _SetBounds(const kroll::ValueList&, kroll::SharedValue);
			void _GetTitle(const kroll::ValueList&, kroll::SharedValue);
			void _SetTitle(const kroll::ValueList&, kroll::SharedValue);
			void _GetURL(const kroll::ValueList&, kroll::SharedValue);
			void _SetURL(const kroll::ValueList&, kroll::SharedValue);
			void _IsResizable(const kroll::ValueList&, kroll::SharedValue);
			void _SetResizable(const kroll::ValueList&, kroll::SharedValue);
			void _IsMaximizable(const kroll::ValueList&, kroll::SharedValue);
			void _SetMaximizable(const kroll::ValueList&, kroll::SharedValue);
			void _IsMinimizable(const kroll::ValueList&, kroll::SharedValue);
			void _SetMinimizable(const kroll::ValueList&, kroll::SharedValue);
			void _IsCloseable(const kroll::ValueList&, kroll::SharedValue);
			void _SetCloseable(const kroll::ValueList&, kroll::SharedValue);
			void _IsVisible(const kroll::ValueList&, kroll::SharedValue);
			void _IsActive(const kroll::ValueList&, kroll::SharedValue);
			void _SetVisible(const kroll::ValueList&, kroll::SharedValue);
			void _GetTransparency(const kroll::ValueList&, kroll::SharedValue);
			void _SetTransparency(const kroll::ValueList&, kroll::SharedValue);
			void _GetTransparencyColor(const kroll::ValueList&, kroll::SharedValue);
			void _GetMenu(const kroll::ValueList&, kroll::SharedValue);
			void _SetMenu(const kroll::ValueList&, kroll::SharedValue);
			void _GetContextMenu(const kroll::ValueList&, kroll::SharedValue);
			void _SetContextMenu(const kroll::ValueList&, kroll::SharedValue);
			void _GetIcon(const kroll::ValueList&, kroll::SharedValue);
			void _SetIcon(const kroll::ValueList&, kroll::SharedValue);
			void _GetParent(const kroll::ValueList&, kroll::SharedValue);
			void _GetChildren(const kroll::ValueList&, kroll::SharedValue);
			void _CreateWindow(const kroll::ValueList&, kroll::SharedValue);
			void _OpenFileChooserDialog(const ValueList& args, SharedValue result);
			void _OpenFolderChooserDialog(const ValueList& args, SharedValue result);
			void _OpenSaveAsDialog(const ValueList& args, SharedValue result);
			void _IsTopMost(const kroll::ValueList&, kroll::SharedValue);
			void _SetTopMost(const kroll::ValueList&, kroll::SharedValue);
			virtual void _ShowInspector(const ValueList& args, SharedValue result);

			virtual void OpenFileChooserDialog(
				SharedKMethod callback,
				bool multiple,
				std::string& title,
				std::string& path,
				std::string& defaultName,
				std::vector<std::string>& types,
				std::string& typesDescription) = 0;
	
			virtual void OpenFolderChooserDialog(
				SharedKMethod callback,
				bool multiple,
				std::string& title,
				std::string& path,
				std::string& defaultName) = 0;
	
			virtual void OpenSaveAsDialog(
				SharedKMethod callback,
				std::string& title,
				std::string& path,
				std::string& defaultName,
				std::vector<std::string>& types,
				std::string& typesDescription) = 0;
	
			virtual void Hide() = 0;
			virtual void Show() = 0;
			virtual void Minimize() = 0;
			virtual void Maximize() = 0;
			virtual void Unminimize() = 0;
			virtual void Unmaximize() = 0;
			virtual bool IsMaximized() = 0;
			virtual bool IsMinimized() = 0;
			virtual void Focus() = 0;
			virtual void Unfocus() = 0;
			virtual bool IsUsingChrome() = 0;
			virtual bool IsUsingScrollbars() = 0;
			virtual bool IsFullscreen() = 0;
			virtual std::string GetId() = 0;
			virtual void Open();
			virtual bool Close();
			void Closed();
	
			virtual double GetX() = 0;
			virtual void SetX(double x) = 0;
			virtual double GetY() = 0;
			virtual void SetY(double y) = 0;
	
			virtual double GetWidth() = 0;
			virtual void SetWidth(double width) = 0;
			virtual double GetMaxWidth() = 0;
			virtual void SetMaxWidth(double width) = 0;
			virtual double GetMinWidth() = 0;
			virtual void SetMinWidth(double width) = 0;
	
			virtual double GetHeight() = 0;
			virtual void SetHeight(double height) = 0;
			virtual double GetMaxHeight() = 0;
			virtual void SetMaxHeight(double height) = 0;
			virtual double GetMinHeight() = 0;
			virtual void SetMinHeight(double height) = 0;
	
			virtual Bounds GetBounds() = 0;
			virtual void SetBounds(Bounds bounds) = 0;
			virtual std::string GetTitle() = 0;
			virtual void SetTitle(std::string& title);
			virtual void SetTitleImpl(std::string& title) = 0;
			virtual std::string GetURL() = 0;
			virtual void SetURL(std::string &url) = 0;
			virtual bool IsResizable() = 0;
			virtual void SetResizable(bool resizable) = 0;
			virtual bool IsMaximizable() = 0;
			virtual void SetMaximizable(bool maximizable) = 0;
			virtual bool IsMinimizable() = 0;
			virtual void SetMinimizable(bool minimizable) = 0;
			virtual bool IsCloseable() = 0;
			virtual void SetCloseable(bool closeable) = 0;
			virtual bool IsVisible() = 0;
			virtual double GetTransparency() = 0;
			virtual void SetTransparency(double transparency) = 0;
			virtual std::string GetTransparencyColor() { return ""; }
			virtual void SetFullscreen(bool fullscreen) = 0;
			virtual void SetUsingChrome(bool chrome) = 0;
			virtual void SetMenu(AutoMenu menu) = 0;
			virtual AutoMenu GetMenu() = 0;
			virtual void SetContextMenu(AutoMenu menu) = 0;
			virtual AutoMenu GetContextMenu() = 0;
			virtual void SetIcon(std::string& iconPath) = 0;
			virtual std::string& GetIcon() = 0;
			virtual bool IsTopMost() = 0;
			virtual void SetTopMost(bool topmost) = 0;
			virtual void ShowInspector(bool console=false) = 0;
			virtual void RegisterJSContext(JSGlobalContextRef);
			virtual void InsertAPI(SharedKObject frameGlobal);
			virtual void PageLoaded(
				SharedKObject scope, std::string &url, JSGlobalContextRef context);
			virtual void AppIconChanged() {};
			virtual void AppMenuChanged() {};
			AutoUserWindow CreateWindow(WindowConfig* config);
			AutoUserWindow CreateWindow(std::string& url);
			AutoUserWindow CreateWindow(SharedKObject properties);
			AutoUserWindow GetAutoPtr();
			static bool ShouldHaveTitaniumObject(JSGlobalContextRef, JSObjectRef);
			static bool IsMainFrame(JSGlobalContextRef, JSObjectRef);

		protected:
			Logger* logger;
			AutoUIBinding binding;
			SharedKObject domWindow;
			Host* host;
			WindowConfig *config;
			AutoUserWindow parent;
			std::vector<AutoUserWindow> children;
			long next_listener_id;
			bool active;
			bool initialized;
			virtual AutoUserWindow GetParent();
			virtual void AddChild(AutoUserWindow);
			virtual void RemoveChild(AutoUserWindow);
			void ReadChooserDialogObject(
				SharedKObject o,
				bool& multiple,
				std::string& title,
				std::string& path,
				std::string& defaultName,
				std::vector<std::string>& types,
				std::string& typesDescription);
			static double Constrain(double, double, double);
			static void LoadUIJavaScript(JSGlobalContextRef context);

		private:
			DISALLOW_EVIL_CONSTRUCTORS(UserWindow);
	};
}
#endif
