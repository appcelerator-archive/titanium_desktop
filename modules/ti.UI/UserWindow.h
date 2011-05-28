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

#ifndef UserWindow_h
#define UserWindow_h

#include <string>
#include <vector>

#include <JavaScriptCore/JSContextRef.h>
#include <kroll/kroll.h>

#ifdef OS_WIN32
#undef CreateWindow
#endif

namespace Titanium {

class WindowConfig;
class Menu;
class UI;

typedef struct {
    double x;
    double y;
    double width;
    double height;
} Bounds;

class UserWindow : public KEventObject {
public:
    // Platform-specific implementation.
    static AutoPtr<UserWindow> CreateWindow(AutoPtr<WindowConfig> config, AutoPtr<UserWindow> parent);

    virtual SharedString DisplayString(int levels=3);
    virtual ~UserWindow();
    void UpdateWindowForURL(std::string url);
    void RegisterJSContext(JSGlobalContextRef);
    void InsertAPI(KObjectRef frameGlobal);
    void PageLoaded(KObjectRef scope, std::string &url, JSGlobalContextRef context);
    KObjectRef GetDOMWindow() { return domWindow; }
    bool IsToolWindow();
    void SetToolWindow(bool toolWindow);
    bool HasTransparentBackground();
    void SetTransparentBackground(bool transparentBackground);
    std::string GetId();

    void _GetCurrentWindow(const kroll::ValueList&, kroll::KValueRef);
    void _GetDOMWindow(const kroll::ValueList&, kroll::KValueRef);
    void _InsertAPI(const kroll::ValueList&, kroll::KValueRef);
    void _Hide(const kroll::ValueList&, kroll::KValueRef);
    void _Show(const kroll::ValueList&, kroll::KValueRef);
    void _Minimize(const kroll::ValueList&, kroll::KValueRef);
    void _Maximize(const kroll::ValueList&, kroll::KValueRef);
    void _Unminimize(const kroll::ValueList&, kroll::KValueRef);
    void _Unmaximize(const kroll::ValueList&, kroll::KValueRef);
    void _IsMaximized(const kroll::ValueList&, kroll::KValueRef);
    void _IsMinimized(const kroll::ValueList&, kroll::KValueRef);
    void _Focus(const kroll::ValueList&, kroll::KValueRef);
    void _Unfocus(const kroll::ValueList&, kroll::KValueRef);
    void _IsUsingChrome(const kroll::ValueList&, kroll::KValueRef);
    void _SetUsingChrome(const kroll::ValueList&, kroll::KValueRef);
    void _IsToolWindow(const kroll::ValueList&, kroll::KValueRef);
    void _SetToolWindow(const kroll::ValueList&, kroll::KValueRef);
    void _HasTransparentBackground(const kroll::ValueList&, kroll::KValueRef);
    void _SetTransparentBackground(const kroll::ValueList&, kroll::KValueRef);
    void _IsUsingScrollbars(const kroll::ValueList&, kroll::KValueRef);
    void _IsFullscreen(const kroll::ValueList&, kroll::KValueRef);
    void _SetFullscreen(const kroll::ValueList&, kroll::KValueRef);
    void _GetId(const kroll::ValueList&, kroll::KValueRef);
    void _Open(const kroll::ValueList&, kroll::KValueRef);
    void _Close(const kroll::ValueList&, kroll::KValueRef);
    void _GetX(const kroll::ValueList&, kroll::KValueRef);
    double _GetX();
    void _SetX(const kroll::ValueList&, kroll::KValueRef);
    void _SetX(double x);
    void _GetY(const kroll::ValueList&, kroll::KValueRef);
    double _GetY();
    void _SetY(const kroll::ValueList&, kroll::KValueRef);
    void _SetY(double y);
    void _MoveTo(const kroll::ValueList&, kroll::KValueRef);
    void _SetSize(const kroll::ValueList&, kroll::KValueRef);
    void _GetWidth(const kroll::ValueList&, kroll::KValueRef);
    double _GetWidth();
    void _SetWidth(const kroll::ValueList&, kroll::KValueRef);
    void _SetWidth(double width);
    void _GetMaxWidth(const kroll::ValueList&, kroll::KValueRef);
    void _SetMaxWidth(const kroll::ValueList&, kroll::KValueRef);
    void _GetMinWidth(const kroll::ValueList&, kroll::KValueRef);
    void _SetMinWidth(const kroll::ValueList&, kroll::KValueRef);
    void _GetHeight(const kroll::ValueList&, kroll::KValueRef);
    double _GetHeight();
    void _SetHeight(const kroll::ValueList&, kroll::KValueRef);
    void _SetHeight(double height);
    void _GetMaxHeight(const kroll::ValueList&, kroll::KValueRef);
    void _SetMaxHeight(const kroll::ValueList&, kroll::KValueRef);
    void _GetMinHeight(const kroll::ValueList&, kroll::KValueRef);
    void _SetMinHeight(const kroll::ValueList&, kroll::KValueRef);
    void _GetBounds(const kroll::ValueList&, kroll::KValueRef);
    void _SetBounds(const kroll::ValueList&, kroll::KValueRef);
    void _GetTitle(const kroll::ValueList&, kroll::KValueRef);
    void _SetTitle(const kroll::ValueList&, kroll::KValueRef);
    void _GetURL(const kroll::ValueList&, kroll::KValueRef);
    void _SetURL(const kroll::ValueList&, kroll::KValueRef);
    void _IsResizable(const kroll::ValueList&, kroll::KValueRef);
    void _SetResizable(const kroll::ValueList&, kroll::KValueRef);
    void _IsMaximizable(const kroll::ValueList&, kroll::KValueRef);
    void _SetMaximizable(const kroll::ValueList&, kroll::KValueRef);
    void _IsMinimizable(const kroll::ValueList&, kroll::KValueRef);
    void _SetMinimizable(const kroll::ValueList&, kroll::KValueRef);
    void _IsCloseable(const kroll::ValueList&, kroll::KValueRef);
    void _SetCloseable(const kroll::ValueList&, kroll::KValueRef);
    void _IsVisible(const kroll::ValueList&, kroll::KValueRef);
    void _IsActive(const kroll::ValueList&, kroll::KValueRef);
    void _SetVisible(const kroll::ValueList&, kroll::KValueRef);
    void _GetTransparency(const kroll::ValueList&, kroll::KValueRef);
    void _SetTransparency(const kroll::ValueList&, kroll::KValueRef);
    void _GetMenu(const kroll::ValueList&, kroll::KValueRef);
    void _SetMenu(const kroll::ValueList&, kroll::KValueRef);
    void _GetContextMenu(const kroll::ValueList&, kroll::KValueRef);
    void _SetContextMenu(const kroll::ValueList&, kroll::KValueRef);
    void _GetIcon(const kroll::ValueList&, kroll::KValueRef);
    void _SetIcon(const kroll::ValueList&, kroll::KValueRef);
    void _GetParent(const kroll::ValueList&, kroll::KValueRef);
    void _GetChildren(const kroll::ValueList&, kroll::KValueRef);
    void _CreateWindow(const kroll::ValueList&, kroll::KValueRef);
    void _OpenFileChooserDialog(const ValueList& args, KValueRef result);
    void _OpenFolderChooserDialog(const ValueList& args, KValueRef result);
    void _OpenSaveAsDialog(const ValueList& args, KValueRef result);
    void _IsTopMost(const kroll::ValueList&, kroll::KValueRef);
    void _SetTopMost(const kroll::ValueList&, kroll::KValueRef);
    virtual void _ShowInspector(const ValueList& args, KValueRef result);
    void _SetContents(const ValueList& args, KValueRef result);
    void SetContents(const std::string& content, const std::string& baseURL);
    void _SetPluginsEnabled(const ValueList& args, KValueRef result);
    virtual void OpenFileChooserDialog(KMethodRef callback, bool multiple,
        std::string& title, std::string& path, std::string& defaultName,
        std::vector<std::string>& types, std::string& typesDescription) = 0;
    virtual void OpenFolderChooserDialog( KMethodRef callback,
        bool multiple, std::string& title, std::string& path,
        std::string& defaultName) = 0;
    virtual void OpenSaveAsDialog(KMethodRef callback, std::string& title,
        std::string& path, std::string& defaultName,
        std::vector<std::string>& types, std::string& typesDescription) = 0;
    void _SetDocumentEdited(const ValueList &args, KValueRef result);
    void _IsDocumentEdited(const ValueList &args, KValueRef result);
    void _IncludeScript(const ValueList &args, KValueRef result);

    // TODO: make these methods non-virtual
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
    virtual void Open();
    virtual bool Close();
    void Closed();
    virtual double GetX() = 0;
    virtual void SetX(double x) = 0;
    virtual double GetY() = 0;
    virtual void SetY(double y) = 0;
    virtual void MoveTo(double x, double y) = 0;

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
    virtual void SetSize(double width, double height) = 0;
    virtual Bounds GetBounds();
    virtual Bounds GetBoundsImpl() = 0;
    void SetBounds(Bounds bounds);
    virtual void SetBoundsImpl(Bounds bounds) = 0;
    virtual std::string GetTitle() = 0;
    virtual void SetTitle(const std::string& title);
    virtual void SetTitleImpl(const std::string& title) = 0;
    virtual std::string GetURL() = 0;
    virtual void SetURL(std::string &url) = 0;
    virtual bool IsResizable() = 0;
    virtual void SetResizable(bool resizable);
    virtual void SetResizableImpl(bool resizable) = 0;
    virtual bool IsMaximizable() = 0;
    virtual void SetMaximizable(bool maximizable) = 0;
    virtual bool IsMinimizable() = 0;
    virtual void SetMinimizable(bool minimizable) = 0;
    virtual bool IsCloseable() = 0;
    virtual void SetCloseable(bool closeable) = 0;
    virtual bool IsVisible() = 0;
    virtual double GetTransparency() = 0;
    virtual void SetTransparency(double transparency) = 0;
    virtual void SetFullscreen(bool fullscreen) = 0;
    virtual void SetUsingChrome(bool chrome) = 0;
    virtual void SetMenu(AutoPtr<Menu> menu) = 0;
    virtual AutoPtr<Menu> GetMenu() = 0;
    virtual void SetContextMenu(AutoPtr<Menu> menu) = 0;
    virtual AutoPtr<Menu> GetContextMenu() = 0;
    virtual void SetIcon(std::string& iconPath) = 0;
    virtual std::string& GetIcon() = 0;
    virtual bool IsTopMost() = 0;
    virtual void SetTopMost(bool topmost) = 0;
    virtual void ShowInspector(bool console=false) = 0;
    virtual void AppIconChanged() {};
    virtual void AppMenuChanged() {};
    virtual void SetContentsImpl(const std::string& content,  const std::string& baseURL) = 0;
    virtual void SetPluginsEnabled(bool enabled) = 0;
    
    // OS X only
    virtual void SetDocumentEdited(bool edited) {}
    virtual bool IsDocumentEdited() { return false; }

protected:
    Logger* logger;
    AutoPtr<UI> binding;
    KObjectRef domWindow;
    AutoPtr<WindowConfig> config;
    AutoPtr<UserWindow> parent;
    std::vector<AutoPtr<UserWindow> > children;
    bool active;
    bool initialized;
    std::string iconURL;

    UserWindow(AutoPtr<WindowConfig> config, AutoPtr<UserWindow> parent);
    virtual AutoPtr<UserWindow> GetParent();
    virtual void AddChild(AutoPtr<UserWindow>);
    virtual void RemoveChild(AutoPtr<UserWindow>);
    void ReadChooserDialogObject(KObjectRef o, bool& multiple,
        std::string& title, std::string& path, std::string& defaultName,
        std::vector<std::string>& types, std::string& typesDescription);
    static void LoadUIJavaScript(JSGlobalContextRef context);

private:
    DISALLOW_EVIL_CONSTRUCTORS(UserWindow);
};

} // namespace Titanium

#endif
