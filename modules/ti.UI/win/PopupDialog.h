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

#ifndef PopupDialog_h
#define PopupDialog_h

#include <map>
#include <string>

#include <kroll/kroll.h>
#include <windowsx.h>

#define MAX_INPUT_LENGTH 1024

namespace Titanium {

class PopupDialog {
public:
    PopupDialog(HWND windowHandle);
    virtual ~PopupDialog();

    void SetShowInputText(bool showInputText)
    {
        this->showInputText = showInputText;
    }
    void SetTitle(std::string title) { this->title = title; }
    void SetMessage(std::string message) { this->message = message; }
    void SetInputText(std::string inputText) { this->inputText = inputText; }
    std::string GetInputText() { return this->inputText; }
    void SetShowCancelButton(bool showCancelButton)
    {
        this->showCancelButton = showCancelButton;
    }
    
    int CountMatches(std::string& message, const char *token);

    int Show();
private:
    HWND windowHandle;

    bool showInputText;
    std::string title;
    std::string message;
    std::string inputText;
    bool showCancelButton;
    int result;

    BOOL ShowMessageBox(HWND hwnd);

    static std::map<DWORD, PopupDialog*> popups;

    static void HandleOKClick(HWND hDlg);
    static INT_PTR CALLBACK CALLBACK Callback(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam);
};

} // namespace Titanium

#endif
