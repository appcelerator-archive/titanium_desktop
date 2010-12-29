/*
 * Copyright (c) 2010 Appcelerator, Inc. All Rights Reserved.
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

#include "../Notification.h"

#include "SnarlInterface.h"

namespace Titanium {

/*static*/
bool Notification::InitializeImpl()
{
    Snarl::SnarlInterface snarlInterface;
    WORD major, minor;
    return snarlInterface.GetVersion(&major, &minor);
}

/*static*/
void Notification::ShutdownImpl()
{
}

static UINT snarlWindowMessage = ::RegisterWindowMessageA("TitaniumSnarlMessage");
static std::map<long, KMethodRef> snarlCallbacks;
static bool MessageHandler(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message != snarlWindowMessage)
        return false;

    long id = (long) lParam;
    std::map<long, KMethodRef>::iterator i = snarlCallbacks.find(id);

    if (i != snarlCallbacks.end())
    {
        if (wParam == Snarl::SNARL_NOTIFICATION_CLICKED)
        {
            RunOnMainThread(i->second, ValueList(), false);
        }
        else if (wParam == Snarl::SNARL_NOTIFICATION_TIMED_OUT)
        {
            snarlCallbacks.erase(i);
        }
    }

    return true;
}

void Notification::CreateImpl()
{
    this->notification = -1;
}

void Notification::DestroyImpl()
{
}

bool Notification::ShowImpl()
{
    Snarl::SnarlInterface snarlInterface;

    std::string iconPath;
    if (!iconURL.empty())
        iconPath = URLUtils::URLToPath(iconURL);

    HWND replyWindow = Host::GetInstance()->AddMessageHandler(&MessageHandler);
    long id = snarlInterface.ShowMessage(::UTF8ToWide(this->title).c_str(),
        ::UTF8ToWide(this->message).c_str(), this->timeout,
         ::UTF8ToWide(iconPath).c_str(), replyWindow, snarlWindowMessage);

    if (!clickedCallback.isNull())
        snarlCallbacks[id] = clickedCallback;

    return true;
}

bool Notification::HideImpl()
{
    if (this->notification == -1)
        return true;

    Snarl::SnarlInterface snarlInterface;
    return snarlInterface.HideMessage(this->notification);
}

} // namespace Titanium
