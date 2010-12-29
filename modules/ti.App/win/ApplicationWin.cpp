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

#include "../Application.h"

#include <windows.h>

namespace Titanium {

void Application::Restart(const ValueList& args, KValueRef result)
{
    Host* host = Host::GetInstance();
    std::wstring cmdline(::UTF8ToWide(host->GetApplication()->arguments.at(0)));

    STARTUPINFO si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));

    ::CreateProcessW(NULL,
        (LPWSTR) cmdline.c_str(),
        NULL, /*lpProcessAttributes*/
        NULL, /*lpThreadAttributes*/
        FALSE, /*bInheritHandles*/
        NORMAL_PRIORITY_CLASS,
        NULL,
        NULL,
        &si,
        &pi);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    host->Exit(0);
}

void Application::Setup() 
{
}

} // namespace Titanium
