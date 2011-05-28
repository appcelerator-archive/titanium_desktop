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

#include "UserWindow.h"

#include <JavaScriptCore/JSStringRef.h>
#include <kroll/javascript/javascript_module.h>
#include <kroll/javascript/javascript_module_instance.h>
#include <kroll/ScriptController.h>

#include "Menu.h"
#include "UI.h"
#include "UIModule.h"
#include "../ti.App/ApplicationConfig.h"
#include "../ti.App/WindowConfig.h"

namespace Titanium {

UserWindow::UserWindow(AutoPtr<WindowConfig> config, AutoPtr<UserWindow> parent)
    : KEventObject("UI.UserWindow")
    , logger(Logger::Get("UI.UserWindow"))
    , binding(UIModule::GetInstance()->GetUIBinding())
    , domWindow(0)
    , config(config)
    , parent(parent)
    , active(false)
    , initialized(false)
{
    this->SetMethod("insertAPI", &UserWindow::_InsertAPI);
    this->SetMethod("hide", &UserWindow::_Hide);
    this->SetMethod("show", &UserWindow::_Show);
    this->SetMethod("minimize", &UserWindow::_Minimize);
    this->SetMethod("unminimize", &UserWindow::_Unminimize);
    this->SetMethod("maximize", &UserWindow::_Maximize);
    this->SetMethod("unmaximize", &UserWindow::_Unmaximize);
    this->SetMethod("focus", &UserWindow::_Focus);
    this->SetMethod("unfocus", &UserWindow::_Unfocus);
    this->SetMethod("isUsingChrome", &UserWindow::_IsUsingChrome);
    this->SetMethod("setUsingChrome", &UserWindow::_SetUsingChrome);
    this->SetMethod("isToolWindow", &UserWindow::_IsToolWindow);
    this->SetMethod("setToolWindow", &UserWindow::_SetToolWindow);
    this->SetMethod("hasTransparentBackground", &UserWindow::_HasTransparentBackground);
    this->SetMethod("setTransparentBackground", &UserWindow::_SetTransparentBackground);
    this->SetMethod("isFullscreen", &UserWindow::_IsFullscreen);
    this->SetMethod("isFullScreen", &UserWindow::_IsFullscreen);
    this->SetMethod("setFullscreen", &UserWindow::_SetFullscreen);
    this->SetMethod("setFullScreen", &UserWindow::_SetFullscreen);
    this->SetMethod("getID", &UserWindow::_GetId);
    this->SetMethod("open", &UserWindow::_Open);
    this->SetMethod("close", &UserWindow::_Close);
    this->SetMethod("getX", &UserWindow::_GetX);
    this->SetMethod("setX", &UserWindow::_SetX);
    this->SetMethod("getY", &UserWindow::_GetY);
    this->SetMethod("setY", &UserWindow::_SetY);
    this->SetMethod("moveTo", &UserWindow::_MoveTo);
    this->SetMethod("setSize", &UserWindow::_SetSize);
    this->SetMethod("getWidth", &UserWindow::_GetWidth);
    this->SetMethod("setWidth", &UserWindow::_SetWidth);
    this->SetMethod("getMaxWidth", &UserWindow::_GetMaxWidth);
    this->SetMethod("setMaxWidth", &UserWindow::_SetMaxWidth);
    this->SetMethod("getMinWidth", &UserWindow::_GetMinWidth);
    this->SetMethod("setMinWidth", &UserWindow::_SetMinWidth);
    this->SetMethod("getHeight", &UserWindow::_GetHeight);
    this->SetMethod("setHeight", &UserWindow::_SetHeight);
    this->SetMethod("getMaxHeight", &UserWindow::_GetMaxHeight);
    this->SetMethod("setMaxHeight", &UserWindow::_SetMaxHeight);
    this->SetMethod("getMinHeight", &UserWindow::_GetMinHeight);
    this->SetMethod("setMinHeight", &UserWindow::_SetMinHeight);
    this->SetMethod("getBounds", &UserWindow::_GetBounds);
    this->SetMethod("setBounds", &UserWindow::_SetBounds);
    this->SetMethod("getTitle", &UserWindow::_GetTitle);
    this->SetMethod("setTitle", &UserWindow::_SetTitle);
    this->SetMethod("getURL", &UserWindow::_GetURL);
    this->SetMethod("setURL", &UserWindow::_SetURL);
    this->SetMethod("isResizable", &UserWindow::_IsResizable);
    this->SetMethod("setResizable", &UserWindow::_SetResizable);
    this->SetMethod("isMaximized", &UserWindow::_IsMaximized);
    this->SetMethod("isMinimized", &UserWindow::_IsMinimized);
    this->SetMethod("isMaximizable", &UserWindow::_IsMaximizable);
    this->SetMethod("setMaximizable", &UserWindow::_SetMaximizable);
    this->SetMethod("isMinimizable", &UserWindow::_IsMinimizable);
    this->SetMethod("setMinimizable", &UserWindow::_SetMinimizable);
    this->SetMethod("isCloseable", &UserWindow::_IsCloseable);
    this->SetMethod("setCloseable", &UserWindow::_SetCloseable);
    this->SetMethod("isVisible", &UserWindow::_IsVisible);
    this->SetMethod("isActive", &UserWindow::_IsActive);
    this->SetMethod("setVisible", &UserWindow::_SetVisible);
    this->SetMethod("getTransparency", &UserWindow::_GetTransparency);
    this->SetMethod("setTransparency", &UserWindow::_SetTransparency);
    this->SetMethod("setMenu", &UserWindow::_SetMenu);
    this->SetMethod("getMenu", &UserWindow::_GetMenu);
    this->SetMethod("setContextMenu", &UserWindow::_SetContextMenu);
    this->SetMethod("getContextMenu", &UserWindow::_GetContextMenu);
    this->SetMethod("setIcon", &UserWindow::_SetIcon);
    this->SetMethod("getIcon", &UserWindow::_GetIcon);
    this->SetMethod("setTopMost", &UserWindow::_SetTopMost);
    this->SetMethod("isTopMost", &UserWindow::_IsTopMost);
    this->SetMethod("createWindow", &UserWindow::_CreateWindow);
    this->SetMethod("openFileChooserDialog", &UserWindow::_OpenFileChooserDialog);
    this->SetMethod("openFolderChooserDialog", &UserWindow::_OpenFolderChooserDialog);
    this->SetMethod("openSaveAsDialog", &UserWindow::_OpenSaveAsDialog);
    this->SetMethod("getParent", &UserWindow::_GetParent);
    this->SetMethod("getChildren", &UserWindow::_GetChildren);
    this->SetMethod("getDOMWindow", &UserWindow::_GetDOMWindow);
    this->SetMethod("getWindow", &UserWindow::_GetDOMWindow);
    this->SetMethod("showInspector", &UserWindow::_ShowInspector);
    this->SetMethod("setContents", &UserWindow::_SetContents);
    this->SetMethod("setPluginsEnabled", &UserWindow::_SetPluginsEnabled);
    this->SetMethod("setDocumentEdited", &UserWindow::_SetDocumentEdited);
    this->SetMethod("isDocumentEdited", &UserWindow::_IsDocumentEdited);

    // Hidden
    this->SetMethod("getCurrentWindow", &UserWindow::_GetCurrentWindow);
    this->SetMethod("include", &UserWindow::_IncludeScript);

    this->FireEvent(Event::CREATED);
}

UserWindow::~UserWindow()
{
    if (this->active)
    {
        this->Close();
    }
}

SharedString UserWindow::DisplayString(int level)
{
    std::string* displayString = new std::string();
    displayString->append("URL=");
    displayString->append(this->GetURL());
    displayString->append(" window id=");
    displayString->append(this->GetId());
    return displayString;
}

static double Constrain(double value, double min, double max)
{
    if (min >= 0.0 && value < min)
    {
        value = min;
    }
    if (max >= 0.0 && value > max)
    {
        value = max;
    }
    return value;
}

void UserWindow::Open()
{
    this->FireEvent(Event::OPEN);

    // We are now in the UI binding's open window list
    this->binding->AddToOpenWindows(AutoPtr<UserWindow>(this, true));

    // Tell the parent window that we are open for business
    if (!parent.isNull())
    {
        parent->AddChild(AutoPtr<UserWindow>(this, true));
    }

    this->initialized = true;
    this->active = true;

    if (!this->config->GetContents().empty())
        this->SetContents(this->config->GetContents(),
             this->config->GetBaseURL());
    else
        this->SetURL(this->config->GetURL());
}

bool UserWindow::Close()
{
    // If FireEvent returns true, stopPropagation or preventDefault
    // was not called on the event -- and we should continue closing
    // the window. Otherwise, we want to cancel the close.
    bool shouldProcess = this->FireEvent(Event::CLOSE);
    if (shouldProcess)
    {
        this->active = false; // Prevent further modification.
    }

    return !this->active;
}

void UserWindow::Closed()
{
    this->config->SetVisible(false);
    this->FireEvent(Event::CLOSED);

    // Close all children and cleanup
    std::vector<AutoPtr<UserWindow> >::iterator iter = this->children.begin();
    while (iter != this->children.end())
    {
        // Save a pointer to the child here, because it may
        // be freed by the SharedPtr otherwise and that will
        // make this iterator seriously, seriously unhappy.
        AutoPtr<UserWindow> child = (*iter);
        iter = children.erase(iter);
        child->Close();
    }

    // Tell our parent that we are now closed
    if (!this->parent.isNull())
    {
        this->parent->RemoveChild(AutoPtr<UserWindow>(this, true));
        if (this->parent->IsVisible())
        {
            this->parent->Focus(); // Focus the parent (if visible)
        }
    }

    // Tell the UIBinding that we are closed
    this->binding->RemoveFromOpenWindows(AutoPtr<UserWindow>(this, true));

    // When we have no more open windows, we exit...
    std::vector<AutoPtr<UserWindow> > windows = this->binding->GetOpenWindows();
    if (windows.size() == 0) 
    {
        Host::GetInstance()->Exit(0);
    } 
    else 
    {
        // attempt to focus the next visible window on the stack
        for (size_t c = 0; c < windows.size(); c++)
        {
            AutoPtr<UserWindow> window = windows.at(c);
            if (window->IsVisible())
            {
                window->Focus();
                break;
            }
        }
    }
}

void UserWindow::_GetCurrentWindow(const kroll::ValueList& args, kroll::KValueRef result)
{
    result->SetObject(AutoPtr<UserWindow>(this, true));
}

void UserWindow::_GetDOMWindow(const kroll::ValueList& args, kroll::KValueRef result)
{
    result->SetObject(this->domWindow);
}

void UserWindow::_InsertAPI(const kroll::ValueList& args, kroll::KValueRef result)
{
    if (!this->active)
        return;

    if (args.size() > 0 && args.at(0)->IsObject())
    {
        this->InsertAPI(args.GetObject(0));
    }
}

void UserWindow::_Hide(const kroll::ValueList& args, kroll::KValueRef result)
{
    this->config->SetVisible(false);
    if (this->active)
    {
        this->Hide();
    }
}

void UserWindow::_Show(const kroll::ValueList& args, kroll::KValueRef result)
{
    this->config->SetVisible(true);
    if (this->active)
    {
        this->Show();
    }
}

void UserWindow::_Minimize(const kroll::ValueList& args, kroll::KValueRef result)
{
    this->config->SetMinimized(true);
    if (this->active)
    {
        this->Minimize();
    }
}

void UserWindow::_Unminimize(const kroll::ValueList& args, kroll::KValueRef result)
{
    this->config->SetMinimized(false);
    if (this->active)
    {
        this->Unminimize();
    }
}

void UserWindow::_IsMinimized(const kroll::ValueList& args, kroll::KValueRef result)
{
    if (this->active)
    {
        result->SetBool(this->IsMinimized());
    }
    else
    {
        return result->SetBool(this->config->IsMinimized());
    }
}

void UserWindow::_Maximize(const kroll::ValueList& args, kroll::KValueRef result)
{
    this->config->SetMaximized(true);
    if (this->active)
    {
        this->Maximize();
    }
}

void UserWindow::_IsMaximized(const kroll::ValueList& args, kroll::KValueRef result)
{
    if (this->active)
    {
        result->SetBool(this->IsMaximized());
    }
    else
    {
        result->SetBool(this->config->IsMaximized());
    }
}

void UserWindow::_Unmaximize(const kroll::ValueList& args, kroll::KValueRef result)
{
    this->config->SetMaximized(false);
    if (this->active)
    {
        this->Unmaximize();
    }
}

void UserWindow::_Focus(const kroll::ValueList& args, kroll::KValueRef result)
{
    if (this->active)
    {
        this->Focus();
    }
}

void UserWindow::_Unfocus(const kroll::ValueList& args, kroll::KValueRef result)
{
    if (this->active)
    {
        this->Unfocus();
    }
}

void UserWindow::_IsUsingChrome(const kroll::ValueList& args, kroll::KValueRef result)
{
    if (this->active)
    {
        result->SetBool(this->IsUsingChrome());
    }
    else
    {
        result->SetBool(this->config->IsUsingChrome());
    }
}

void UserWindow::_SetUsingChrome(const kroll::ValueList& args, kroll::KValueRef result)
{
    args.VerifyException("setUsingChrome", "b");
    bool b = args.at(0)->ToBool();
    this->config->SetUsingChrome(b);
    if (this->active)
    {
        this->SetUsingChrome(b);
    }
}


void UserWindow::_IsToolWindow(const kroll::ValueList& args, kroll::KValueRef result)
{
    result->SetBool(this->IsToolWindow());
}

void UserWindow::_SetToolWindow(const kroll::ValueList& args, kroll::KValueRef result)
{
    args.VerifyException("setToolWindow", "b");
    config->SetToolWindow(args.GetBool(0));
}

void UserWindow::_HasTransparentBackground(const kroll::ValueList& args, kroll::KValueRef result)
{
    result->SetBool(this->HasTransparentBackground());
}

void UserWindow::_SetTransparentBackground(const kroll::ValueList& args, kroll::KValueRef result)
{
    args.VerifyException("setTransparentBackground", "b");
    config->SetTransparentBackground(args.GetBool(0));
}

void UserWindow::_SetTopMost(const kroll::ValueList& args, kroll::KValueRef result)
{
    args.VerifyException("setTopMost", "b");
    bool b = args.at(0)->ToBool();
    this->config->SetTopMost(b);
    if (this->active)
    {
        this->SetTopMost(b);
    }
}

void UserWindow::_IsTopMost(const kroll::ValueList& args, kroll::KValueRef result)
{
    if (this->active)
    {
        result->SetBool(this->IsTopMost());
    }
    else
    {
        result->SetBool(this->config->IsTopMost());
    }
}


void UserWindow::_IsUsingScrollbars(const kroll::ValueList& args, kroll::KValueRef result)
{
    if (this->active)
    {
        result->SetBool(this->IsUsingChrome());
    }
    else
    {
        result->SetBool(this->config->IsUsingScrollbars());
    }
}

void UserWindow::_IsFullscreen(const kroll::ValueList& args, kroll::KValueRef result)
{
    if (this->active)
    {
        result->SetBool(this->IsFullscreen());
    }
    else
    {
        result->SetBool(this->config->IsFullscreen());
    }
}

void UserWindow::_SetFullscreen(const kroll::ValueList& args, kroll::KValueRef result)
{
    args.VerifyException("setFullscreen", "b");
    bool b = args.at(0)->ToBool();
    this->config->SetFullscreen(b);
    if (this->active)
    {
        this->SetFullscreen(b);
    }
}

void UserWindow::_GetId(const kroll::ValueList& args, kroll::KValueRef result)
{
    result->SetString(this->config->GetID());
}

void UserWindow::_Open(const kroll::ValueList& args, kroll::KValueRef result)
{
    // Don't allow a window to be opened twice
    if (this->active || this->initialized)
    {
        throw ValueException::FromString("Cannot open a window more than once");
    }
    else
    {
        this->Open();
    }
}

void UserWindow::_Close(const kroll::ValueList& args, kroll::KValueRef result)
{
    // Don't allow a non-active window to be closed
    if (this->active)
    {
        this->Close();
    }
}

void UserWindow::_GetX(const kroll::ValueList& args, kroll::KValueRef result)
{
    result->SetDouble(this->_GetX());
}

double UserWindow::_GetX()
{
    if (this->active)
    {
        return this->GetX();
    }
    else
    {
        return this->config->GetX();
    }
}

void UserWindow::_SetX(const kroll::ValueList& args, kroll::KValueRef result)
{
    // Manual argument type-checking for speed considerations
    if (args.size() > 0 && args.at(0)->IsNumber())
    {
        double x = args.at(0)->ToNumber();
        this->_SetX(x);
    }
}

void UserWindow::_SetX(double x)
{
    this->config->SetX((int)x);
    if (this->active)
    {
        this->SetX(x);
    }
}

void UserWindow::_GetY(const kroll::ValueList& args, kroll::KValueRef result)
{
    result->SetDouble(this->_GetY());
}

double UserWindow::_GetY()
{
    if (this->active)
    {
        return this->GetY();
    }
    else
    {
        return this->config->GetY();
    }
}

void UserWindow::_SetY(const kroll::ValueList& args, kroll::KValueRef result)
{
    // Manual argument type-checking for speed considerations
    if (args.size() > 0 && args.at(0)->IsNumber())
    {
        double y = args.at(0)->ToNumber();
        this->_SetY(y);
    }
}

void UserWindow::_SetY(double y)
{
    this->config->SetY((int)y);
    if (this->active)
    {
        this->SetY(y);
    }

}

void UserWindow::_MoveTo(const kroll::ValueList& args, kroll::KValueRef result)
{
    args.VerifyException("moveTo", "nn");

    double x = args.GetDouble(0);
    double y = args.GetDouble(1);

    this->config->SetX((int)x);
    this->config->SetY((int)y);

    if (this->active)
    {
        this->MoveTo(x, y);
    }
}

void UserWindow::_SetSize(const kroll::ValueList& args, kroll::KValueRef result)
{
    args.VerifyException("setSize", "nn");

    double width = Constrain(args.GetDouble(0), config->GetMinWidth(), config->GetMaxWidth());
    double height = Constrain(args.GetDouble(1), config->GetMinHeight(), config->GetMaxHeight());

    config->SetWidth(width);
    config->SetHeight(height);
    SetSize(width, height);
}

void UserWindow::_GetWidth(const kroll::ValueList& args, kroll::KValueRef result)
{
    result->SetDouble(this->_GetWidth());
}

double UserWindow::_GetWidth()
{
    if (this->active)
    {
        return this->GetWidth();
    }
    else
    {
        return this->config->GetWidth();
    }
}

void UserWindow::_SetWidth(const kroll::ValueList& args, kroll::KValueRef result)
{
    // Manual argument type-checking for speed considerations
    if (args.size() > 0 && args.at(0)->IsNumber())
    {
        double w = args.at(0)->ToNumber();
        this->_SetWidth(w);
    }
}

void UserWindow::_SetWidth(double w)
{
    if (w > 0)
    {
        w = Constrain(w, config->GetMinWidth(), config->GetMaxWidth());
        this->config->SetWidth((int)w);
        if (this->active)
        {
            this->SetWidth(w);
        }
    }
}

void UserWindow::_GetMinWidth(const kroll::ValueList& args, kroll::KValueRef result)
{
    if (this->active)
    {
        result->SetDouble(this->GetMinWidth());
    }
    else
    {
        result->SetDouble(this->config->GetMinWidth());
    }
}

void UserWindow::_SetMinWidth(const kroll::ValueList& args, kroll::KValueRef result)
{
    args.VerifyException("setMinWidth", "n");
    double mw = args.at(0)->ToNumber();

    if (mw <= 0)
    {
        mw = -1;
    }

    this->config->SetMinWidth((int)mw);
    if (mw != -1 && this->config->GetWidth() < mw)
    {
        this->config->SetWidth((int)mw);
    }

    if (this->active)
    {
        this->SetMinWidth(mw);
        if (mw != -1 && this->GetWidth() < mw)
        {
            this->SetWidth(mw);
        }
    }
}

void UserWindow::_GetMaxWidth(const kroll::ValueList& args, kroll::KValueRef result)
{
    if (this->active)
    {
        result->SetDouble(this->GetMaxWidth());
    }
    else
    {
        result->SetDouble(this->config->GetMaxWidth());
    }
}

void UserWindow::_SetMaxWidth(const kroll::ValueList& args, kroll::KValueRef result)
{
    args.VerifyException("setMaxWidth", "n");
    double mw = args.at(0)->ToNumber();

    if (mw <= 0)
    {
        mw = -1;
    }

    this->config->SetMaxWidth((int)mw);
    if (mw != -1 && this->config->GetWidth() > mw)
    {
        this->config->SetWidth((int)mw);
    }

    if (this->active)
    {
        this->SetMaxWidth(mw);
        if (mw != -1 && this->GetWidth() > mw)
        {
            this->SetWidth(mw);
        }
    }
}

void UserWindow::_GetHeight(const kroll::ValueList& args, kroll::KValueRef result)
{
    result->SetDouble(this->_GetHeight());
}

double UserWindow::_GetHeight()
{
    if (this->active)
    {
        return this->GetHeight();
    }
    else
    {
        return this->config->GetHeight();
    }
}

void UserWindow::_SetHeight(const kroll::ValueList& args, kroll::KValueRef result)
{
    // Manual argument type-checking for speed considerations
    if (args.size() > 0 && args.at(0)->IsNumber())
    {
        double h = args.at(0)->ToNumber();
        this->_SetHeight(h);
    }
}

void UserWindow::_SetHeight(double h)
{
    if (h > 0)
    {
        h = Constrain(h, config->GetMinHeight(), config->GetMaxHeight());
        this->config->SetHeight((int)h);
        if (this->active)
        {
            this->SetHeight(h);
        }
    }
}

void UserWindow::_GetMinHeight(const kroll::ValueList& args, kroll::KValueRef result)
{
    if (this->active)
    {
        result->SetDouble(this->GetMinHeight());
    }
    else
    {
        result->SetDouble(this->config->GetMinHeight());
    }
}

void UserWindow::_SetMinHeight(const kroll::ValueList& args, kroll::KValueRef result)
{
    args.VerifyException("setMinHeight", "n");
    double mh = args.at(0)->ToNumber();
    if (mh <= 0)
    {
        mh = -1;
    }

    this->config->SetMinHeight((int)mh);
    if (mh != -1 && this->config->GetHeight() < mh)
    {
        this->config->SetHeight((int)mh);
    }

    if (this->active)
    {
        this->SetMinHeight(mh);
        if (mh != -1 && this->GetHeight() < mh)
        {
            this->SetHeight(mh);
        }
    }
}

void UserWindow::_GetMaxHeight(const kroll::ValueList& args, kroll::KValueRef result)
{
    if (this->active)
    {
        result->SetDouble(this->GetMaxHeight());
    }
    else
    {
        result->SetDouble(this->config->GetMaxHeight());
    }
}

void UserWindow::_SetMaxHeight(const kroll::ValueList& args, kroll::KValueRef result)
{
    args.VerifyException("setMaxHeight", "n");
    double mh = args.at(0)->ToNumber();
    if (mh <= 0)
    {
        mh = -1;
    }

    this->config->SetMaxHeight((int)mh);
    if (mh != -1 && this->config->GetHeight() > mh)
    {
        this->config->SetHeight((int)mh);
    }

    if (this->active)
    {
        this->SetMaxHeight(mh);
        if (mh != -1 && this->GetHeight() > mh)
        {
            this->SetHeight(mh);
        }
    }
}

void UserWindow::_GetBounds(const kroll::ValueList& args, kroll::KValueRef result)
{
    Bounds bounds = this->GetBounds();
    KObjectRef b(new StaticBoundObject());
    b->SetDouble("x", bounds.x);
    b->SetDouble("y", bounds.y);
    b->SetDouble("width", bounds.width);
    b->SetDouble("height", bounds.height);
    result->SetObject(b);
}

Bounds UserWindow::GetBounds()
{
    if (this->active)
        return this->GetBoundsImpl();

    Bounds bounds;
    bounds.x = this->config->GetX();
    bounds.y = this->config->GetY();
    bounds.width = this->config->GetWidth();
    bounds.height = this->config->GetHeight();
    return bounds;
}

void UserWindow::_SetBounds(const kroll::ValueList& args, kroll::KValueRef result)
{
    if (args.size() < 1 || !args.at(0)->IsObject())
    {
        throw ValueException::FromString("setBounds takes an object with x, y, width and height properties.");
        return;
    }

    KObjectRef o = args.at(0)->ToObject();
    if (!o->Get("x")->IsNumber()
        || !o->Get("y")->IsNumber()
        || !o->Get("width")->IsNumber()
        ||!o->Get("height")->IsNumber())
    {
        throw ValueException::FromString("setBounds takes an object with x, y, width and height properties.");
        return;
    }

    double x = o->Get("x")->ToNumber();
    double y = o->Get("y")->ToNumber();
    double w = o->Get("width")->ToNumber();
    double h = o->Get("height")->ToNumber();
    
    Bounds bounds;
    bounds.x = x;
    bounds.y = y;
    bounds.width = w;
    bounds.height = h;

    this->SetBounds(bounds);
}

void UserWindow::SetBounds(Bounds b)
{
    double w = Constrain(b.width, config->GetMinWidth(), config->GetMaxWidth());
    double h = Constrain(b.height, config->GetMinHeight(), config->GetMaxHeight());
    this->config->SetX((int)b.x);
    this->config->SetY((int)b.y);
    this->config->SetWidth((int)w);
    this->config->SetHeight((int)h);

    if (this->active)
    {
        this->SetBoundsImpl(b);
    }
}

void UserWindow::_GetTitle(const kroll::ValueList& args, kroll::KValueRef result)
{
    std::string title;
    if (this->active)
    {
        title = this->GetTitle();
    }
    else
    {
        title = this->config->GetTitle();
    }
    result->SetString(title);
}

void UserWindow::_SetTitle(const kroll::ValueList& args, kroll::KValueRef result)
{
    args.VerifyException("setTitle", "s");
    std::string newTitle = args.at(0)->ToString();
    this->SetTitle(newTitle);
}

void UserWindow::SetTitle(const std::string& newTitle)
{
    this->config->SetTitle(newTitle);
    if (this->active)
    {
        this->SetTitleImpl(newTitle);
    }
}

void UserWindow::_GetURL(const kroll::ValueList& args, kroll::KValueRef result)
{
    string url;
    if (this->active)
    {
        url = this->GetURL();
    }
    else
    {
        url = this->config->GetURL();
    }
    result->SetString(url);
}

void UserWindow::_SetURL(const kroll::ValueList& args, kroll::KValueRef result)
{
    args.VerifyException("setURL", "s");

    std::string url = args.at(0)->ToString();
    if (url.empty())
        url = URLUtils::BlankPageURL();
    else
        url = URLUtils::NormalizeURL(url);

    this->config->SetURL(url);
    if (this->active)
    {
        this->SetURL(url);
    }
}

void UserWindow::_IsResizable(const kroll::ValueList& args, kroll::KValueRef result)
{
    if (this->active)
    {
        result->SetBool(this->IsResizable());
    }
    else
    {
        result->SetBool(this->config->IsResizable());
    }
}

void UserWindow::_SetResizable(const kroll::ValueList& args, kroll::KValueRef result)
{
    args.VerifyException("setResizable", "b");
    this->SetResizable(args.at(0)->ToBool());
}

void UserWindow::SetResizable(bool resizable)
{
    this->config->SetResizable(resizable);
    if (this->active)
    {
        this->SetResizableImpl(resizable);
    }
}

void UserWindow::_IsMaximizable(const kroll::ValueList& args, kroll::KValueRef result)
{
    if (this->active)
    {
        result->SetBool(this->IsMaximizable());
    }
    else
    {
        result->SetBool(this->config->IsMaximizable());
    }
}

void UserWindow::_SetMaximizable(const kroll::ValueList& args, kroll::KValueRef result)
{
    args.VerifyException("setMaximizable", "b");
    bool b = args.at(0)->ToBool();
    this->config->SetMaximizable(b);
    if (this->active)
    {
        this->SetMaximizable(b);
    }
}

void UserWindow::_IsMinimizable(const kroll::ValueList& args, kroll::KValueRef result)
{
    if (this->active)
    {
        result->SetBool(this->IsMinimizable());
    }
    else
    {
        result->SetBool(this->config->IsMinimizable());
    }
}

void UserWindow::_SetMinimizable(const kroll::ValueList& args, kroll::KValueRef result)
{
    args.VerifyException("setMinimizable", "b");
    bool b = args.at(0)->ToBool();
    this->config->SetMinimizable(b);
    if (this->active)
    {
        this->SetMinimizable(b);
    }
}

void UserWindow::_IsCloseable(const kroll::ValueList& args, kroll::KValueRef result)
{
    if (this->active)
    {
        result->SetBool(this->IsCloseable());
    }
    else
    {
        result->SetBool(this->config->IsCloseable());
    }
}

void UserWindow::_SetCloseable(const kroll::ValueList& args, kroll::KValueRef result)
{
    args.VerifyException("setCloseable", "b");
    bool b = args.at(0)->ToBool();
    this->config->SetCloseable(b);
    if (this->active)
    {
        this->SetCloseable(b);
    }
}

void UserWindow::_IsVisible(const kroll::ValueList& args, kroll::KValueRef result)
{
    if (this->active)
    {
        result->SetBool(this->IsVisible());
    }
    else
    {
        result->SetBool(false);
    }
}

void UserWindow::_IsActive(const kroll::ValueList& args, kroll::KValueRef result)
{
    result->SetBool(this->active);
}

void UserWindow::_SetVisible(const kroll::ValueList& args, kroll::KValueRef result)
{
    args.VerifyException("setVisible", "b");
    bool b = args.at(0)->ToBool();
    this->config->SetVisible(b);

    if (this->active)
    {
        if (b)
        {
            this->Show();
        }
        else
        {
            this->Hide();
        }
    }
}

void UserWindow::_GetTransparency(const kroll::ValueList& args, kroll::KValueRef result)
{
    if (this->active)
    {
        result->SetDouble(this->GetTransparency());
    }
    else
    {
        result->SetDouble(this->config->GetTransparency());
    }
}

void UserWindow::_SetTransparency(const kroll::ValueList& args, kroll::KValueRef result)
{
    args.VerifyException("setTransparency", "n");
    double t = args.at(0)->ToNumber();
    t = Constrain(t, 0.0, 1.0);

    this->config->SetTransparency(t);
    if (this->active)
    {
        this->SetTransparency(t);
    }
}

void UserWindow::_SetMenu(const kroll::ValueList& args, kroll::KValueRef result)
{
    args.VerifyException("setMenu", "o|0");
    AutoPtr<Menu> menu(args.GetObject(0, 0).cast<Menu>());

    if (this->active)
        this->SetMenu(menu);
}

void UserWindow::_GetMenu(const kroll::ValueList& args, kroll::KValueRef result)
{
    AutoPtr<Menu> menu = this->GetMenu();
    if (!menu.isNull())
    {
        result->SetObject(menu);
    }
    else
    {
        result->SetNull();
    }
}

void UserWindow::_SetContextMenu(const kroll::ValueList& args, kroll::KValueRef result)
{
    args.VerifyException("setMenu", "o|0");
    AutoPtr<Menu> menu(args.GetObject(0, 0).cast<Menu>());

    if (this->active)
        this->SetContextMenu(menu);
}

void UserWindow::_GetContextMenu(const kroll::ValueList& args, kroll::KValueRef result)
{
    AutoPtr<Menu> menu = this->GetContextMenu();
    if (!menu.isNull())
    {
        result->SetObject(menu);
    }
    else
    {
        result->SetNull();
    }
}

void UserWindow::_SetIcon(const kroll::ValueList& args, kroll::KValueRef result)
{
    args.VerifyException("setIcon", "s|0");
    std::string iconPath;
    if (args.size() > 0)
    {
        this->iconURL = args.GetString(0);
        iconPath = URLUtils::URLToPath(this->iconURL);
    }
    else
    {
        this->iconURL = iconPath = "";
    }

    if (this->active)
    {
        this->SetIcon(iconPath);
    }
}

void UserWindow::_GetIcon(const kroll::ValueList& args, kroll::KValueRef result)
{
    result->SetString(this->iconURL);
}

void UserWindow::_GetParent(const kroll::ValueList& args, kroll::KValueRef result)
{
    if (this->parent.isNull())
    {
        result->SetNull();
    }
    else
    {
        result->SetObject(this->parent);
    }
}

void UserWindow::_GetChildren(const kroll::ValueList& args, kroll::KValueRef result)
{
    KListRef childList = new StaticBoundList();

    std::vector<AutoPtr<UserWindow> >::iterator i = this->children.begin();
    while (i != this->children.end())
    {
        KObjectRef child = *i++;
        childList->Append(Value::NewObject(child));
    }

    result->SetList(childList);
}

void UserWindow::_CreateWindow(const ValueList& args, KValueRef result)
{
    AutoPtr<WindowConfig> config(0);
    if (args.size() > 0 && args.at(0)->IsObject())
    {
        config = WindowConfig::FromProperties(args.GetObject(0));
    }
    else if (args.size() > 0 && args.at(0)->IsString())
    {
        std::string url(args.GetString(0));
        config = ApplicationConfig::Instance()->GetWindowByURL(url);
        if (config.isNull())
        {
            config = WindowConfig::Default();
            config->SetURL(url);
        }
    }

    // If we still do not have a configuration, just use the default.
    if (config.isNull())
        config = WindowConfig::Default();

    result->SetObject(UserWindow::CreateWindow(config, AutoPtr<UserWindow>(this, true)));
}

void UserWindow::UpdateWindowForURL(std::string url)
{
    AutoPtr<WindowConfig> config(ApplicationConfig::Instance()->GetWindowByURL(url));

    // If no configuration regex matched, don't change
    // the properties of the window at all.
    if (config.isNull())
        return;

    // TODO: This should set up more properties than those listed here.
    Bounds b;
    b.x = config->GetX();
    b.y = config->GetY();
    b.width = config->GetWidth();
    b.height = config->GetHeight();
    this->SetBounds(b);
    this->SetMinimizable(config->IsMinimizable());
    this->SetMaximizable(config->IsMaximizable());
    this->SetCloseable(config->IsCloseable());
}

void UserWindow::ReadChooserDialogObject(
    KObjectRef o,
    bool& multiple,
    std::string& title,
    std::string& path,
    std::string& defaultName,
    std::vector<std::string>& types,
    std::string& typesDescription)

{
    // Pass in a set of properties for chooser dialogs like this:
    // var selected = Titanium.UI.OpenFileChooserDialog(callback,
    // {
    //    multiple:true,
    //    title: "Select file to delete...",
    //    defaultFile: "autoexec.bat",
    //    path: "C:\"
    //    types:['js','html']
    // });
    multiple = o->GetBool("multiple", true);
    title = o->GetString("title", title);
    path = o->GetString("path", path);
    defaultName = o->GetString("defaultName", defaultName);

    KListRef listTypes = new StaticBoundList();
    listTypes = o->GetList("types", listTypes);
    for (size_t i = 0; i < listTypes->Size(); i++)
    {
        if (listTypes->At(i)->IsString())
        {
            types.push_back(listTypes->At(i)->ToString());
            std::cout << "Found " << listTypes->At(i)->ToString() << std::endl;
        }
    }
    typesDescription = o->GetString("typesDescription", defaultName);

}

void UserWindow::_OpenFileChooserDialog(const ValueList& args, KValueRef result)
{
    args.VerifyException("openFileChooserDialog", "m ?o");

    KMethodRef callback = args.at(0)->ToMethod();
    bool multiple = false;
    std::string path;
    std::string defaultName;
    std::string title = "Choose File";
    std::vector<std::string> types;
    std::string typesDescription;

    KObjectRef props;
    if (args.size() > 1)
    {
        KObjectRef props = args.at(1)->ToObject();
        ReadChooserDialogObject(props,
            multiple,
            title,
            path,
            defaultName,
            types,
            typesDescription);
    }
    if (this->active)
    {
        this->OpenFileChooserDialog(
            callback, multiple, title, path, defaultName, types, typesDescription);
    }
    else
    {
        throw ValueException::FromString("Tried to open file chooser on an inactive window.");
    }
}

void UserWindow::_OpenFolderChooserDialog(const ValueList& args, KValueRef result)
{
    args.VerifyException("openFolderChooserDialog", "m ?o");
    KMethodRef callback = args.at(0)->ToMethod();
    bool multiple = false;
    std::string path;
    std::string defaultName;
    std::string title = "Choose Directory";
    std::vector<std::string> types;
    std::string typesDescription;

    KObjectRef props;
    if (args.size() > 1)
    {
        KObjectRef props = args.at(1)->ToObject();
        ReadChooserDialogObject(props,
            multiple,
            title,
            path,
            defaultName,
            types,
            typesDescription);
    }

    if (this->active)
    {
        this->OpenFolderChooserDialog(callback, multiple, title, path, defaultName);
    }
    else
    {
        throw ValueException::FromString("Tried to open folder chooser on an inactive window.");
    }
}

void UserWindow::_OpenSaveAsDialog(const ValueList& args, KValueRef result)
{
    args.VerifyException("openFolderChooserDialog", "m ?o");
    KMethodRef callback = args.at(0)->ToMethod();
    bool multiple = false;
    std::string path;
    std::string defaultName;
    std::string title = "Save File";
    std::vector<std::string> types;
    std::string typesDescription;

    KObjectRef props;
    if (args.size() > 1)
    {
        KObjectRef props = args.at(1)->ToObject();
        ReadChooserDialogObject(props,
            multiple,
            title,
            path,
            defaultName,
            types,
            typesDescription);
    }
    if (this->active)
    {
        this->OpenSaveAsDialog(
            callback, title, path, defaultName, types, typesDescription);
    }
    else
    {
        throw ValueException::FromString("Tried to save dialog on an inactive window.");
    }
}

void UserWindow::_ShowInspector(const ValueList& args, KValueRef result)
{
    if (!this->active)
        return;

    if (args.size() > 0 && args.at(0)->IsBool())
    {
        bool console = args.at(0)->ToBool();
        this->ShowInspector(console);
    }
    else
    {
        this->ShowInspector();
    }
}

void UserWindow::_SetContents(const ValueList& args, KValueRef result)
{
    args.VerifyException("setContents", "s ?s");
    this->SetContents(args.GetString(0), args.GetString(1));
}

void UserWindow::SetContents(const std::string& content, const std::string& baseURL)
{
    std::string normalizedURL(baseURL);
    if (baseURL.empty())
        normalizedURL = "app://_blank_.html";
    normalizedURL = URLUtils::NormalizeURL(normalizedURL);

    this->config->SetContents(content);
    this->config->SetBaseURL(normalizedURL);

    if (!this->active)
        return;

    this->SetContentsImpl(content, normalizedURL);
}

void UserWindow::_SetPluginsEnabled(const ValueList& args, KValueRef result)
{
    args.VerifyException("setPluginsEnabled", "b");
    this->SetPluginsEnabled(args.GetBool(0));
}

AutoPtr<UserWindow> UserWindow::GetParent()
{
    return this->parent;
}

void UserWindow::AddChild(AutoPtr<UserWindow> child)
{
    this->children.push_back(child);
}

void UserWindow::RemoveChild(AutoPtr<UserWindow> child)
{
    std::vector<AutoPtr<UserWindow> >::iterator iter = this->children.begin();
    while (iter != this->children.end())
    {
        if ((*iter).get() == child.get())
        {
            iter = children.erase(iter);
        }
        else
        {
            iter++;
        }
    }
}

static bool ShouldHaveTitaniumObject(JSGlobalContextRef ctx, JSObjectRef global)
{
    // We really only want URLs that are loaded via the
    // app, ti or file protocol to have the Titanium object.
    // Other URLs won't have access for security considerations.
    JSStringRef docPropName = JSStringCreateWithUTF8CString("document");
    JSValueRef docValue = JSObjectGetProperty(ctx, global, docPropName, NULL);
    JSStringRelease(docPropName);

    if (!docValue)
    {
        return false;
    }

    JSObjectRef docObject = JSValueToObject(ctx, docValue, NULL);
    if (!docObject)
    {
        return false;
    }

    JSStringRef locPropName = JSStringCreateWithUTF8CString("location");
    JSValueRef locValue = JSObjectGetProperty(ctx, docObject, locPropName, NULL);
    JSStringRelease(locPropName);

    if (!locValue)
    {
        return false;
    }

    JSStringRef locString = JSValueToStringCopy(ctx, locValue, NULL);
    if (!locString)
    {
        return false;
    }

    string url(KJSUtil::ToChars(locString));
    transform(url.begin(), url.end(), url.begin(), tolower);
    return url.find("app://") == 0 || url.find("ti://") == 0 ||
        url.find("file://") == 0 || url.find("data:text/html;") == 0;
}

static bool IsMainFrame(JSGlobalContextRef ctx, JSObjectRef global)
{
    // If this global objects 'parent' property is equal to the object
    // itself, it is likely the main frame. There might be a better way
    // to do this determination, but at this point we've left the port-
    // -dependent code and this should generally work cross-platform.
    JSStringRef parentPropName = JSStringCreateWithUTF8CString("parent");
    JSValueRef parentValue = JSObjectGetProperty(ctx, global, parentPropName, NULL);
    JSStringRelease(parentPropName);

    if (!parentValue)
    {
        return false;
    }

    JSObjectRef parentObject = JSValueToObject(ctx, parentValue, NULL);
    if (!parentObject)
    {
        return false;
    }

    return parentObject == global;
}

void UserWindow::InsertAPI(KObjectRef frameGlobal)
{
    // Produce a delegating object to represent the top-level Titanium object.
    // When a property isn't found in this object it will look for it globally.
    KObjectRef windowTiObject(new KAccessorObject());
    KObjectRef windowUIObject(new KAccessorObject());

    // Place currentWindow in the delegate base.
    windowUIObject->Set("getCurrentWindow", this->Get("getCurrentWindow"));

    // Place currentWindow.createWindow in the delegate base.
    windowUIObject->Set("createWindow", this->Get("createWindow"));

    // Place currentWindow.openFiles in the delegate base.
    windowUIObject->Set("openFileChooserDialog", this->Get("openFileChooserDialog"));
    windowUIObject->Set("openFolderChooserDialog", this->Get("openFolderChooserDialog"));
    windowUIObject->Set("openSaveAsDialog", this->Get("openSaveAsDialog"));

    // Create a delegate object for the UI API. When a property cannot be
    // found in binding, KDelegatingObject will search for it in
    // the base. When developers modify this object, it will be modified
    // globally.
    KObject* delegateUIAPI = new KDelegatingObject(binding, windowUIObject);
    windowTiObject->Set("UI", Value::NewObject(delegateUIAPI));

    // Provide method for including other scripts within this window's context.
    windowTiObject->Set("include", this->Get("include"));

    // Place the Titanium object into the window's global object
    KObjectRef delegateGlobalObject = new KDelegatingObject(
        Host::GetInstance()->GetGlobalObject(), windowTiObject);
    frameGlobal->SetObject(GLOBAL_NS_VARNAME, delegateGlobalObject);
}

static KValueRef DeferredGarbageCollection(const ValueList& args)
{
    JavaScriptModuleInstance::GarbageCollect();
    return Value::Undefined;
}

void UserWindow::RegisterJSContext(JSGlobalContextRef context)
{
    JSObjectRef globalObject = JSContextGetGlobalObject(context);
    KJSUtil::RegisterGlobalContext(globalObject, context);

    // Get the global object as a KKJSObject
    KObjectRef frameGlobal = new KKJSObject(context, globalObject);

    // We only want to set this UserWindow's DOM window property if the
    // particular frame that just loaded was the main frame. Each frame
    // that loads on a page will follow this same code path.
    if (IsMainFrame(context, globalObject))
        this->domWindow = frameGlobal->GetObject("window", 0);

    // Only certain pages should get the Titanium object. This is to prevent
    // malicious sites from always getting access to the user's system. This
    // can be overridden by any other API that calls InsertAPI on this DOM window.
    bool hasTitaniumObject = ShouldHaveTitaniumObject(context, globalObject);
    if (hasTitaniumObject)
    {
        this->InsertAPI(frameGlobal);
        UserWindow::LoadUIJavaScript(context);
    }

    AutoPtr<Event> event = this->CreateEvent(Event::PAGE_INITIALIZED);
    event->SetObject("scope", frameGlobal);
    event->SetString("url", config->GetURL());
    event->SetBool("hasTitaniumObject", hasTitaniumObject);
    this->FireEvent(event);

    // The page location has changed, but JavaScriptCore may have references
    // to old DOMs still in memory waiting on garbage collection. Force a GC
    // here so that memory usage stays reasonable.
    RunOnMainThread(new KFunctionPtrMethod(&DeferredGarbageCollection),
        ArgList(), false);
}

void UserWindow::LoadUIJavaScript(JSGlobalContextRef context)
{
    std::string modulePath = UIModule::GetInstance()->GetPath();
    std::string jsPath = FileUtils::Join(modulePath.c_str(), "ui.js", NULL);
    try
    {
        KJSUtil::EvaluateFile(context, (char*) jsPath.c_str());
    }
    catch (kroll::ValueException &e)
    {
        SharedString ss = e.DisplayString();
        Logger* logger = Logger::Get("UIModule");
        logger->Error("Error loading %s: %s", jsPath.c_str(), (*ss).c_str());
    }
    catch (...)
    {
        Logger* logger = Logger::Get("UIModule");
        logger->Error("Unexpected error loading %s", jsPath.c_str());
    }
}

void UserWindow::PageLoaded(
    KObjectRef globalObject, std::string &url, JSGlobalContextRef context)
{
    AutoPtr<Event> event = this->CreateEvent(Event::PAGE_LOADED);
    event->SetObject("scope", globalObject);
    event->SetString("url", url);
    this->FireEvent(event);
}

bool UserWindow::IsToolWindow()
{
    return config->IsToolWindow();
}

void UserWindow::SetToolWindow(bool toolWindow)
{
    config->SetToolWindow(toolWindow);
}

bool UserWindow::HasTransparentBackground()
{
    return config->HasTransparentBackground();
}

void UserWindow::SetTransparentBackground(bool transparentBackground)
{
    config->SetTransparentBackground(transparentBackground);
}

std::string UserWindow::GetId()
{
    return config->GetID();
}

void UserWindow::_SetDocumentEdited(const ValueList& args, KValueRef result)
{
    args.VerifyException("setDocumentEdited", "b");
    this->SetDocumentEdited(args.GetBool(0));
}

void UserWindow::_IsDocumentEdited(const ValueList& args, KValueRef result)
{
    result->SetBool(this->IsDocumentEdited());
}

void UserWindow::_IncludeScript(const ValueList& args, KValueRef result)
{
    args.VerifyException("include", "s");
    const char* filepath = args.at(0)->ToString();

    // Convert relative path to the Resources folder to an absolute path.
    std::string resourcesPath = Host::GetInstance()->GetApplication()->GetResourcesPath();
    std::string absoluteFilePath = FileUtils::Join(resourcesPath.c_str(), filepath, NULL);

    ScriptController* script = Host::GetInstance()->script();
    KValueRef scriptResult = script->EvaluateFile(absoluteFilePath.c_str(), domWindow);
    result->SetValue(scriptResult);
}

} // namespace Titanium
