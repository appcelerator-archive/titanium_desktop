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

#ifndef UIModule_h
#define UIModule_h

#include <kroll/kroll.h>

namespace Titanium {

class UI;

class UIModule : public kroll::Module {
    KROLL_MODULE_CLASS(UIModule)

public:
    static UIModule* GetInstance() { return instance_; }
    AutoPtr<UI> GetUIBinding() { return uiBinding; }
    void Start();
    static bool IsResourceLocalFile(std::string string);

protected:
    static UIModule* instance_;
    AutoPtr<UI> uiBinding;

    DISALLOW_EVIL_CONSTRUCTORS(UIModule);
};

} // namespace Titanium

#endif
