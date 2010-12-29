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

#ifndef Application_h
#define Application_h

#include <kroll/kroll.h>

namespace Titanium {

class Application : public KAccessorObject {
public:
    Application(Host *host,KObjectRef);
    virtual ~Application();

private:
    void GetID(const ValueList& args, KValueRef result);
    void GetName(const ValueList& args, KValueRef result);
    void GetVersion(const ValueList& args, KValueRef result);
    void GetPublisher(const ValueList& args, KValueRef result);
    void GetCopyright(const ValueList& args, KValueRef result);
    void GetDescription(const ValueList& args, KValueRef result);
    void GetURL(const ValueList& args, KValueRef result);
    void GetGUID(const ValueList& args, KValueRef result);
    void GetIcon(const ValueList& args, KValueRef result);
    void GetStreamURL(const ValueList& args, KValueRef result);
    void GetPath(const ValueList& args, KValueRef result);
    void GetHome(const ValueList& args, KValueRef result);
    void GetArguments(const ValueList& args, KValueRef result);
    void AppURLToPath(const ValueList& args, KValueRef result);
    void SetMenu(const ValueList& args, KValueRef result);
    void Exit(const ValueList& args, KValueRef result);
    void Restart(const ValueList& args, KValueRef result);
    void StdOut(const ValueList& args, KValueRef result);
    void StdErr(const ValueList& args, KValueRef result);
    void StdIn(const ValueList& args, KValueRef result);

    void CreateProperties(const ValueList& args, KValueRef result);
    void LoadProperties(const ValueList& args, KValueRef result);
    void GetSystemProperties(const ValueList& args, KValueRef result);

    Host* host;
    KObjectRef global;

protected:
    void Setup();
};

} // namespace Titanium

#endif
