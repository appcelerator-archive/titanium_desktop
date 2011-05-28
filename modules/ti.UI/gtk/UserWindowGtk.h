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

#ifndef UserWindowGtk_h
#define UserWindowGtk_h

#include "../UserWindow.h"

#include <gtk/gtk.h>
#include <webkit/webkit.h>

namespace Titanium {

class MenuGtk;

class UserWindowGtk : public UserWindow {
public:
    UserWindowGtk(AutoPtr<WindowConfig>, AutoPtr<UserWindow>&);
    inline WebKitWebView* GetWebView() { return this->webView; }

    virtual ~UserWindowGtk();
    void CreateWidgets();
    void ShowWidgets();
    void SetupDecorations();
    void SetupTransparency();
    void SetupSizeLimits();
    void SetupSize();
    void SetupPosition();
    void SetupMenu();
    void SetupIcon();
    virtual void AppMenuChanged();
    virtual void AppIconChanged();
    void RemoveOldMenu();
    void OpenFileChooserDialog(KMethodRef callback,
        bool multiple, std::string& title, std::string& path,
        std::string& defaultName, std::vector<std::string>& types,
        std::string& typesDescription);
    void OpenFolderChooserDialog(KMethodRef callback,
        bool multiple, std::string& title, std::string& path,
        std::string& defaultName);
    void OpenSaveAsDialog(KMethodRef callback,
        std::string& title, std::string& path,
        std::string& defaultName, std::vector<std::string>& types,
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
    void Open();
    bool Close();
    double GetX();
    void SetX(double x);
    double GetY();
    void SetY(double y);
    void MoveTo(double x, double y);

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
    void SetSize(double width, double height);

    Bounds GetBoundsImpl();
    void SetBoundsImpl(Bounds bounds);
    std::string GetTitle();
    void SetTitleImpl(const std::string& title);
    std::string GetURL();
    void SetURL(std::string& url);
    bool IsResizable();
    void SetResizableImpl(bool resizable);
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
    void SetMenu(AutoPtr<Menu>);
    AutoPtr<Menu> GetMenu();
    void SetContextMenu(AutoPtr<Menu>);
    AutoPtr<Menu> GetContextMenu();
    void SetIcon(std::string& iconPath);
    std::string& GetIcon();
    void SetInspectorWindow(GtkWidget* inspectorWindow);
    GtkWidget *GetInspectorWindow();
    virtual void ShowInspector(bool console);
    void SetContentsImpl(const std::string&, const std::string&);
    void SetPluginsEnabled(bool enabled);

    // These values contain the most-recently-set dimension
    // information for this UserWindow. GDK is asynchronous,
    // so if a user sets the value the and fetches it without
    // giving up control to the UI thread, returning one of them
    // will yield the correct information. When we actually
    // detect a GDK window resize, these values will also be
    // updated, so they will be an accurate representation of
    // the window size.
    int targetWidth;
    int targetHeight;
    int targetX;
    int targetY;
    bool targetMaximized;
    bool targetMinimized;

protected:
    GtkWindow* gtkWindow;
    GtkWidget* vbox;
    WebKitWebView* webView;
    bool topmost;
    gulong deleteCallbackId;
    AutoPtr<MenuGtk> menu; // The window-specific menu.
    AutoPtr<MenuGtk> activeMenu; // This window's active menu 
    AutoPtr<MenuGtk> contextMenu; // The window specific context menu 
    ::GtkMenuBar* nativeMenu; // The widget this window uses for a menu.
    std::string iconPath; // The path to this window's icon
    GtkWidget *inspectorWindow; // This window's web inspector window
};

} // namespace Titanium

#endif

