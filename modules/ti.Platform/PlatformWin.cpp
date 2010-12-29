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

#include "Platform.h"

#include <windows.h>
#include <commdlg.h>
#include <shellapi.h>
#include <shlobj.h>

#include <cairo/cairo.h>
#include <cairo/cairo-win32.h>
#include <kroll/kroll.h>
#include <Poco/Environment.h>

namespace Titanium {

static std::string GetFailureReason(long code)
{
    switch (code)
    {
        case 0:
        case SE_ERR_OOM:
            return "out of memory";
        case SE_ERR_FNF:
            return  "file not found";
        case SE_ERR_PNF:
            return "path not found";
        case ERROR_BAD_FORMAT:
            return "bad format error";
        case SE_ERR_ACCESSDENIED:
            return "access denied";
        case SE_ERR_ASSOCINCOMPLETE:
            return "association incomplete";
        case SE_ERR_DDEBUSY:
            return "DDE busy";
        case SE_ERR_DDEFAIL:
            return "DDE failed";
        case SE_ERR_DDETIMEOUT:
            return "DDE timed out";
        case SE_ERR_DLLNOTFOUND:
            return "DLL not found";
        case SE_ERR_NOASSOC:
            return "no application associated";
        case SE_ERR_SHARE:
            return "Sharing violation";
    }
    return "unknown";
}

std::string Platform::GetVersionImpl()
{
    return Poco::Environment::osVersion();
}

bool Platform::OpenApplicationImpl(const std::string& name)
{
    std::wstring wideName(::UTF8ToWide(name));
    long response = reinterpret_cast<long>(ShellExecuteW(
        0, L"open", wideName.c_str(), 0, 0, SW_SHOWNORMAL));

    if (response <= 32)
    {
        std::string reason(GetFailureReason(response));
        throw ValueException::FromFormat("openApplication failed: %s", reason.c_str());
        return false;
    }

    return true;
}

bool Platform::OpenURLImpl(const std::string& name)
{
    return this->OpenApplicationImpl(name);
}

void Platform::TakeScreenshotImpl(const std::string& targetFile)
{
    // Ensure filename ends in .bmp.
    // TODO: This seems broken.
    std::string screenshotFile = targetFile;
    if (screenshotFile.rfind(".") == std::string::npos)
        screenshotFile.append(".png");

    HWND desktop = GetDesktopWindow();
    RECT desktopRect;
    GetWindowRect(desktop, &desktopRect);

    int width = desktopRect.right;
    int height = desktopRect.bottom;

    cairo_surface_t* surface = cairo_win32_surface_create_with_dib(CAIRO_FORMAT_RGB24, width, height);
    cairo_surface_flush(surface);

    HDC hdc = cairo_win32_surface_get_dc(surface);
    BitBlt(hdc, 0, 0, width, height, GetDC(0), 0, 0, SRCCOPY);

    std::string filename(::UTF8ToSystem(screenshotFile));
    cairo_status_t result = cairo_surface_write_to_png(surface, filename.c_str());

    cairo_surface_destroy(surface);

    if (result != CAIRO_STATUS_SUCCESS)
        throw ValueException::FromString("Could not save screenshot.");
}

} // namespace Titanium
