/*
 * Copyright (c) 2009-2010 Appcelerator, Inc. All Rights Reserved.
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

#include "TrayItemWin.h"

#include "MenuItemWin.h"
#include "MenuWin.h"
#include "UIWin.h"

using namespace std;

namespace Titanium {

vector<AutoPtr<TrayItemWin> > TrayItemWin::trayItems;
UINT TrayItemWin::trayClickedMessage =
    ::RegisterWindowMessageA(PRODUCT_NAME"TrayClicked");

TrayItemWin::TrayItemWin(string& iconURL, KMethodRef cbSingleClick)
    : TrayItem(iconURL)
    , oldNativeMenu(0)
    , trayIconData(0)
{
    this->AddEventListener(Event::CLICKED, cbSingleClick);

    HWND hwnd = Host::GetInstance()->AddMessageHandler(
        &TrayItemWin::MessageHandler);

    NOTIFYICONDATA* notifyIconData = new NOTIFYICONDATA;
    notifyIconData->cbSize = sizeof(NOTIFYICONDATA);
    notifyIconData->hWnd = hwnd;
    notifyIconData->uID = ++UIWin::nextItemId;
    notifyIconData->uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    notifyIconData->uCallbackMessage = trayClickedMessage;

    HICON icon = UIWin::LoadImageAsIcon(iconPath,
        GetSystemMetrics(SM_CXSMICON),
        GetSystemMetrics(SM_CYSMICON));
    notifyIconData->hIcon = icon;

    lstrcpy(notifyIconData->szTip, L"Titanium Application");
    Shell_NotifyIcon(NIM_ADD, notifyIconData);
    this->trayIconData = notifyIconData;

    trayItems.push_back(this);
}

TrayItemWin::~TrayItemWin()
{
    this->Remove();
}

void TrayItemWin::SetIcon(string& iconPath)
{
    if (!this->trayIconData)
        return;

    HICON icon = UIWin::LoadImageAsIcon(iconPath,
        GetSystemMetrics(SM_CXSMICON),
        GetSystemMetrics(SM_CYSMICON));
    this->trayIconData->hIcon = icon;
    Shell_NotifyIcon(NIM_MODIFY, this->trayIconData);
}

void TrayItemWin::SetMenu(AutoPtr<Menu> menu)
{
    this->menu = menu;
}

void TrayItemWin::SetHint(string& hint)
{
    if (this->trayIconData)
    {
        // NotifyIconData.szTip has 128 character limit.
        ZeroMemory(this->trayIconData->szTip, 128);

        // make sure we don't overflow the static buffer.
        wstring hintW = ::UTF8ToWide(hint);
        lstrcpyn(this->trayIconData->szTip, hintW.c_str(), 128);

        Shell_NotifyIcon(NIM_MODIFY, this->trayIconData);
    }
}

void TrayItemWin::Remove()
{
    if (this->trayIconData)
    {
        Shell_NotifyIcon(NIM_DELETE, this->trayIconData);
        this->trayIconData = 0;
    }
}

void TrayItemWin::HandleRightClick()
{
    if (this->oldNativeMenu)
    {
        DestroyMenu(this->oldNativeMenu);
        this->oldNativeMenu = 0;
    }

    if (this->menu.isNull())
        return;

    AutoPtr<MenuWin> win32menu = this->menu.cast<MenuWin>();
    if (win32menu.isNull())
        return;

    this->oldNativeMenu = win32menu->CreateNative(false);
    POINT pt;
    GetCursorPos(&pt);
    
    SetForegroundWindow(this->trayIconData->hWnd);  
    TrackPopupMenu(this->oldNativeMenu, TPM_BOTTOMALIGN, 
        pt.x, pt.y, 0, this->trayIconData->hWnd, NULL);
    PostMessage(this->trayIconData->hWnd, WM_NULL, 0, 0);
}

void TrayItemWin::HandleLeftClick()
{
    try
    {
        this->FireEvent(Event::CLICKED);
    }
    catch (ValueException& e)
    {
        Logger* logger = Logger::Get("UI.TrayItemWin");
        SharedString ss = e.DisplayString();
        logger->Error("Tray icon single click callback failed: %s", ss->c_str());
    }
}

void TrayItemWin::HandleDoubleLeftClick()
{
    try
    {
        this->FireEvent(Event::DOUBLE_CLICKED);
    }
    catch (ValueException& e)
    {
        Logger* logger = Logger::Get("UI.TrayItemWin");
        SharedString ss = e.DisplayString();
        logger->Error("Tray icon double left click callback failed: %s", ss->c_str());
    }
}

UINT TrayItemWin::GetId()
{
    return this->trayIconData->uID;
}

/*static*/
bool TrayItemWin::MessageHandler(
    HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message == trayClickedMessage)
    {
        UINT button = (UINT) lParam;
        int id = LOWORD(wParam);
        bool handled = false;

        for (size_t i = 0; i < trayItems.size(); i++)
        {
            AutoPtr<TrayItemWin> item = trayItems[i];

            item->is_double_clicked = false;
            if(item->GetId() == id && button == WM_LBUTTONDBLCLK)
            {
                item->is_double_clicked = true;
                KillTimer(hWnd, 100);
                item->HandleDoubleLeftClick();
                handled = true;
            }
            if (item->GetId() == id && button == WM_LBUTTONDOWN)
            {
                SetTimer(hWnd, 100, GetDoubleClickTime(), (TIMERPROC)DoubleClickTimerProc); 
                handled = true;
            }
            else if (item->GetId() == id && button == WM_RBUTTONDOWN)
            {
                item->HandleRightClick();
                handled = true;
            }
        }
        return handled;
    }
    else if (message == WM_MENUCOMMAND)
    {
        HMENU nativeMenu = (HMENU) lParam;
        UINT position = (UINT) wParam;
        return MenuItemWin::HandleClickEvent(nativeMenu, position);
    }
    else
    {
        // Not handled;
        return false;
    }
}

/*static*/
LRESULT CALLBACK TrayItemWin::DoubleClickTimerProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int id = LOWORD(wParam);
    bool handled = false;

    KillTimer(hWnd, 100);
    for (size_t i = 0; i < trayItems.size(); i++)
    {
        AutoPtr<TrayItemWin> item = trayItems[i];
        if (!(item->is_double_clicked))
        {
            item->HandleLeftClick();
        }

        item->is_double_clicked = false;
    }
    return 0;
}

} // namespace Titanium
