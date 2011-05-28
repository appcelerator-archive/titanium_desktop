/*
 * Copyright (c) 2008-2010 Appcelerator, Inc. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef UserWindowWin_h
#define UserWindowWin_h

#include "../UserWindow.h"
#include "../../ti.App/WindowConfig.h"

#include <WebKit/WebKit.h>
#include <WebKit/WebKitCOMAPI.h>

namespace Titanium {

class MenuWin;
class WebKitFrameLoadDelegate;
class WebKitPolicyDelegate;
class WebKitResourceLoadDelegate;
class WebKitUIDelegate;

class UserWindowWin : public UserWindow {
public:
    UserWindowWin(AutoPtr<WindowConfig> config, AutoPtr<UserWindow>& parent);
    ~UserWindowWin();

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
    void MoveTo(double x, double y);
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
    void SetSize(double width, double height);
    Bounds GetBoundsImpl();
    void SetBoundsImpl(Bounds bounds);
    void SetTitleImpl(const std::string& title);
    void SetURL(std::string& url);
    void SetResizableImpl(bool resizable);
    void SetMaximizable(bool maximizable);
    void SetMinimizable(bool minimizable);
    void SetCloseable(bool closeable);
    bool IsVisible();
    void SetTransparency(double transparency);
    void SetFullscreen(bool fullscreen);
    void SetUsingChrome(bool chrome);
    void SetMenu(AutoPtr<Menu> menu);
    AutoPtr<Menu> GetMenu();
    void SetContextMenu(AutoPtr<Menu> menu);
    AutoPtr<Menu> GetContextMenu();
    void SetIcon(std::string&);
    std::string& GetIcon();
    bool IsTopMost();
    void SetTopMost(bool topmost);
    void FrameLoaded();
    void ShowInspector(bool console);
    static UserWindowWin* FromWindow(HWND hWnd);
    static AutoPtr<UserWindowWin> FromWebView(IWebView *webView);

    void RedrawMenu();
    static void RedrawAllMenus();
    virtual void AppIconChanged();
    virtual void AppMenuChanged();
    void SetContentsImpl(const std::string&, const std::string&);
    void SetPluginsEnabled(bool enabled);

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
    WebKitFrameLoadDelegate* frameLoadDelegate;
    WebKitUIDelegate* uiDelegate;
    WebKitPolicyDelegate* policyDelegate;
    WebKitResourceLoadDelegate* resourceLoadDelegate;
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
    AutoPtr<MenuWin> menu; // The window-specific menu
    AutoPtr<MenuWin> activeMenu; // This window's active menu
    HMENU nativeMenu; // This window's active native menu
    AutoPtr<MenuWin> contextMenu; // This window-specific context menu
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
    Logger* logger;
};

} // namespace Titanium

#endif
